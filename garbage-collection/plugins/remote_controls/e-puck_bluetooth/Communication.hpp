#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <webots/types.h>
#include <string>

class Serial;
class Packet;

class Communication {
public:
  Communication();
  virtual ~Communication();

  bool initialize(const std::string &port);
  void cleanup();

  bool isInitialized() const { return mInitialized; }

  bool sendPacket(const Packet *packet);
  bool receivePacket(Packet *packet);
  char *talk(const char *source);
  char *readLine();

private:
  bool mInitialized;

  Serial *mSerial;
};

#endif
