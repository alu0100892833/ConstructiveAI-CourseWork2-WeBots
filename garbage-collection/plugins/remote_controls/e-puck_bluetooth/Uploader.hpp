#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <string>

class Serial;

class Uploader {
public:
  static int connect(const std::string &port);
  static void disconnect();
  static int send(int robot_id, const std::string &hexfilename, void (*updateProgressFunction)(int, int));
  static void cancelUpload();

private:
  static Serial *cSerial;
};

#endif // UPLOADER_HPP
