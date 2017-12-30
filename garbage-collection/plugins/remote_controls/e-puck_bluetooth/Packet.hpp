/*
 * Description:  Defines a packet
 */

#ifndef PACKET_HPP
#define PACKET_HPP

class Packet {
public:
  Packet(int maxSize, int initSize=0);
  virtual ~Packet();

  char *data() const { return mData; }

  virtual void clear() { mSize = 0; }
  int size() const { return mSize; }

  void append(char v);
  void append(short i);

  short readShortAt(int pos);
  unsigned short readUShortAt(int pos);
  unsigned char readUCharAt(int pos);

  void print();

private:
  int mMaxSize;
  int mSize;
  char *mData;
};

#endif
