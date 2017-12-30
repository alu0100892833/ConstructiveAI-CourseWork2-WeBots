/*
 * Description:  Defines an interface to communicate on the serial port
 */

#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class Serial {
public:
  Serial(const std::string &port);
  ~Serial();

  void drain(); // read data until no more data
  void write(const char *packet, int size);
  int read(char *packet, int size, bool wait);
  char *talk(const char *source);
  char *readLine();

  static void updatePorts();
  static const std::vector<std::string> *availablePorts() { return &portNames; }

private:
  static std::vector<std::string> portNames;

  void throwFatalException(const std::string &errorMessage, bool displayLastError=false);

  std::string mName;
#ifdef _WIN32
  static DWORD readFileThread(void *param);
  HANDLE mFd;
#else
  int mFd;
#endif

};

#endif
