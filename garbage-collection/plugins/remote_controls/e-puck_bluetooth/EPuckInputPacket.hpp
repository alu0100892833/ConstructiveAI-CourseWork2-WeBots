/*
 * Description:  Defines a packet sending from the real e-puck to the remote control library
 */

#ifndef EPUCK_INPUT_PACKET_HPP
#define EPUCK_INPUT_PACKET_HPP

#include "Packet.hpp"

class EPuckOutputPacket;

class EPuckInputPacket : public Packet {
public:
  EPuckInputPacket(int maxSize);
  virtual ~EPuckInputPacket();

  void decode(int simulationTime, const EPuckOutputPacket &outputPacket);
};

#endif
