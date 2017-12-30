#ifndef UPLOADER_DATA_HPP
#define UPLOADER_DATA_HPP

#ifdef __cplusplus
extern "C" {
#endif

#define UPLOADER_DATA_CONNECT    1
#define UPLOADER_DATA_SEND_FILE  2
#define UPLOADER_DATA_CANCEL     3
#define UPLOADER_DATA_DISCONNECT 4

struct UploaderData {
  int command;
  int robot_id;
  char *data;
  void (*progress_callback)(int, int);
};
#ifdef __cplusplus
}
#endif

#endif // UPLOADER_DATA_HPP
