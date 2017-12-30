#include <webots/accelerometer.h>
#include <webots/camera.h>
#include <webots/device.h>
#include <webots/differential_wheels.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/remote_control.h>
#include <webots/robot.h>
#include <webots/robot_wwi.h>
#include <webots/utils/default_robot_window.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../remote_controls/e-puck_bluetooth/UploaderData.hpp"

static WbDeviceTag ps[8], ls[8], accelerometer, camera, gs[3];
static const int N_SENSORS = sizeof(ps) / sizeof(WbDeviceTag);
static int gs_sensors_count = 0;
static bool configured = false;
static int n_ports;
static char **ports = NULL;

static void cleanup_ports() {
  if (ports)
    free(ports[0]);
  free(ports);
}

static void get_ports() {
  cleanup_ports();
  char *p = (char *)wb_remote_control_custom_function(NULL);
  // return a list of COM port files separated by `\n`
  if (!p)
    return;
  int l = strlen(p);
  if (l == 0) {
    n_ports = 0;
    ports = NULL;
    free(p);
    return;
  }
  ports = (char **)malloc(sizeof(char *) * l);
  ports[0] = p;
  n_ports = 1;
  int i;
  for(i = 0; i < l; i++)
    if (p[i] == '\n') {
      p[i] = '\0';
      ports[n_ports] = &p[i+1];
      n_ports++;
    }
}

static void send_ports() {
  get_ports();
  char text[1024];
  text[0] = '\0';
  strcat(text, "ports");
  int i, j, k = strlen(text);
  for(i = 0 ; i < n_ports; i++) {
    text[k++] = ' ';
    for(j = 0; j < strlen(ports[i]); j++) {
      if (ports[i][j] == '\\')
        text[k++] = '\\'; // escape character
      text[k++] = ports[i][j];
    }
  }
  text[k++] = '\0';
  wb_robot_wwi_send_text(text);
}

void wb_robot_window_init() {
  char device[32];
  int i;
  for (i = 0; i < N_SENSORS; i++) {
    snprintf(device, 32, "ps%d", i);
    ps[i] = wb_robot_get_device(device);
    snprintf(device, 32, "ls%d", i);
    ls[i] = wb_robot_get_device(device);
  }
  accelerometer = wb_robot_get_device("accelerometer");
  camera = wb_robot_get_device("camera");

  // optional ground sensors
  for (i = 0; i< 3; i++)
    gs[i] = 0;
  int groundSensorIndex;
  int numberOfDevices = wb_robot_get_number_of_devices();
  for (i = 0; i < numberOfDevices; i++) {
    WbDeviceTag tag = wb_robot_get_device_by_index(i);
    WbNodeType deviceType = wb_device_get_node_type(tag);
    if (deviceType == WB_NODE_DISTANCE_SENSOR) {
      const char *deviceName = wb_device_get_name(tag);
      int matchedItems = sscanf(deviceName, "gs%d", &groundSensorIndex);
      if (matchedItems > 0) {
        // init ground sensors
        if (groundSensorIndex < 3 && gs[groundSensorIndex] == 0) {
          gs[groundSensorIndex] = tag;
          ++gs_sensors_count;
        }
      }
    }
  }
  if (gs_sensors_count != 3)
    // all the 3 ground sensors have to be available
    gs_sensors_count = 0;
}

static void upload_progress_callback(int i, int j) {
  static int jj = -1;
  if (i==2) {
    printf("Press the reset button of the e-puck robot to start the upload.\n");
  } else {
    if (j != jj) {
      printf("Uploading: %d%%\n", j);
      if (j==99)
        printf("Upload complete.\n");
      jj = j;
    }
  }
}

void wb_robot_window_step(int time_step) {
  int i;
  const char *message = wb_robot_wwi_receive_text();
  if (message) {
    if (strcmp(message, "configure") == 0) {
      send_ports();
      wbu_default_robot_window_configure();
      configured = true;
    } else if (strcmp(message, "enable") == 0) {
      wb_camera_enable(camera, time_step);
      wb_accelerometer_enable(accelerometer, time_step);
      wb_differential_wheels_enable_encoders(time_step);
      for (i = 0; i < N_SENSORS; i++) {
        wb_distance_sensor_enable(ps[i], time_step);
        wb_light_sensor_enable(ls[i], time_step);
      }
      // optional ground sensors
      for (i = 0; i < gs_sensors_count; i++)
        wb_distance_sensor_enable(gs[i], time_step);
    } else if (strcmp(message, "refresh") == 0)
      send_ports();
    else if (strcmp(message, "simulation") == 0)
      wb_robot_set_mode(WB_MODE_SIMULATION, NULL);
    else if (strncmp(message, "remote control ", 15) == 0)
      wb_robot_set_mode(WB_MODE_REMOTE_CONTROL, (char *)&message[15]);
    else if (strncmp(message, "upload ", 7) == 0) {
      char *port;
      const char *p = &message[7];
      int n = strchr(p, ' ') - p;
      port = (char *)malloc(n + 1);
      strncpy(port, p, n);
      port[n] = '\0';
      struct UploaderData upload;
      upload.command = UPLOADER_DATA_CONNECT;
      upload.data = port;
      wb_remote_control_custom_function(&upload);
      free(port);
      const char *data = &message[8 + n];
      const char *path = getenv("WEBOTS_TMP_PATH");
      const char *filename = "e-puck.hex";
      char *full_path = (char *)malloc(strlen(path) + strlen(filename) + 1);
      sprintf(full_path, "%s%s", path, filename);
      FILE *fd = fopen(full_path,"wb");
      if (fd == NULL)
        fprintf(stderr, "Cannot open %s for writting\n", full_path);
      else {
        fwrite(data, 1, strlen(data), fd);
        fclose(fd);
        upload.command = UPLOADER_DATA_SEND_FILE;
        upload.robot_id = 100;
        upload.data = full_path;
        upload.progress_callback = upload_progress_callback;
        wb_remote_control_custom_function(&upload);
        upload.command = UPLOADER_DATA_DISCONNECT;
        wb_remote_control_custom_function(&upload);
      }
      free(full_path);
    } else
      fprintf(stderr, "received unknown message from robot window: %s\n", message);
  }
  if (!configured)
    return;
  char update_message[256];
  char update[256];
  update_message[0] = '\0';
  for (i = 0; i < 8; i++) {
    double v;
    if (wb_distance_sensor_get_sampling_period(ps[i]))
      v = wb_distance_sensor_get_value(ps[i]);
    else
      v = NAN;
    if (isnan(v))
      snprintf(update, sizeof(update), "ps%d ", i);
    else
      snprintf(update, sizeof(update), "%d ", (int)v);
    if (strlen(update) + strlen(update_message) < sizeof(update_message))
      strcat(update_message, update);
  }
  for (i = 0; i < 8; i++) {
    double v;
    if (wb_light_sensor_get_sampling_period(ls[i]))
      v = wb_light_sensor_get_value(ls[i]);
    else
      v = NAN;
    if (isnan(v))
      snprintf(update, sizeof(update), "ls%d ", i);
    else
      snprintf(update, sizeof(update), "%d ", (int)v);
    if (strlen(update) + strlen(update_message) < sizeof(update_message))
      strcat(update_message, update);
  }
  
  bool areDevicesReady = true;
  double left_speed = wb_differential_wheels_get_left_speed();
  if (isnan(left_speed)) {
    snprintf(update, sizeof(update), "speed ");
    areDevicesReady = false;
  } else
    snprintf(update, sizeof(update), "%d ", (int)left_speed);
  if (strlen(update) + strlen(update_message) < sizeof(update_message))
    strcat(update_message, update);

  double right_speed = wb_differential_wheels_get_right_speed();
  if (isnan(right_speed)) {
    snprintf(update, sizeof(update), "speed ");
    areDevicesReady = false;
  } else
    snprintf(update, sizeof(update), "%d ", (int)right_speed);
  if (strlen(update) + strlen(update_message) < sizeof(update_message))
    strcat(update_message, update);

  if (areDevicesReady && wb_differential_wheels_get_encoders_sampling_period())
    snprintf(update, sizeof(update), "%d %d ",
             (int)wb_differential_wheels_get_left_encoder(),
             (int)wb_differential_wheels_get_right_encoder());
  else
    snprintf(update, sizeof(update), "encoder encoder ");
  if (strlen(update) + strlen(update_message) < sizeof(update_message))
    strcat(update_message, update);

  if (wb_accelerometer_get_sampling_period(accelerometer)) {
    const double *values = wb_accelerometer_get_values(accelerometer);
    const char name[3] = "XYZ";
    update[0] = '\0';
    for (i = 0; i < 3; ++i) {
      char s[8];
      if (isnan(values[i]))
        sprintf(s, "%c ", name[i]);
      else
        sprintf(s, "%.3f ", values[i]);
      strcat(update, s);
    }
  } else
    snprintf(update, sizeof(update), "X Y Z ");
  if (strlen(update) + strlen(update_message) < sizeof(update_message))
    strcat(update_message, update);

  if (areDevicesReady && wb_camera_get_sampling_period(camera)) {
    const char *path = getenv("WEBOTS_TMP_PATH");
    const char *filename = "camera.jpg";
    int pid = getpid();
    int l = strlen(path) + strlen(filename) + 1 + 11; // 11 = max length of an integer: -2147483648
    char *full_path = (char *)malloc(l);
    snprintf(full_path, l, "%s%d%s", path, pid, filename);
    wb_camera_save_image(camera, full_path, 90);
    for(i = 0; i < l; i++)
      if (full_path[i]=='\\')
        full_path[i] = '/';
    snprintf(update, sizeof(update), "file:///%s", full_path);
    free(full_path);
  } else
    snprintf(update, sizeof(update), "camera.jpg");
  if (strlen(update) + strlen(update_message) < sizeof(update_message))
    strcat(update_message, update);

  for (i = 0; i < gs_sensors_count; i++) {
    double v;
    if (wb_distance_sensor_get_sampling_period(gs[i]))
      v = wb_distance_sensor_get_value(gs[i]);
    else
      v = NAN;
    if (isnan(v))
      snprintf(update, sizeof(update), " -1");
    else {
      int c = (v - 300.0) * 255.0 / 700.0;
      if (c > 255)
        c = 255;
      else if (c < 0)
        c = 0;
      snprintf(update, sizeof(update), " %d", (int)c);
    }
    if (strlen(update) + strlen(update_message) < sizeof(update_message))
      strcat(update_message, update);
  }

  wb_robot_wwi_send_text(update_message);
}

void wb_robot_window_cleanup() {
  cleanup_ports();
}
