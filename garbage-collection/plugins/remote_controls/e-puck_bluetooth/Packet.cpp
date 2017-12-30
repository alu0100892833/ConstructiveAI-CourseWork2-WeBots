#include "Packet.hpp"

#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

using namespace std;

Packet::Packet(int maxSize, int initSize) :
  mMaxSize(maxSize),
  mSize(initSize)
{
  mData = (char *) malloc(maxSize);
  if (!mData)
    throw runtime_error("Malloc failed");
}

Packet::~Packet() {
  free(mData);
}

void Packet::append(char v) {
  if (mSize + (int) sizeof(char) >= mMaxSize)
    throw overflow_error("Packet max size overflow");

  mData[mSize++] = v;
}

void Packet::append(short v) {
  if (mSize + (int) sizeof(short) >= mMaxSize)
    throw overflow_error("Packet max size overflow");

  mData[mSize++] = v & 0xFF;
  mData[mSize++] = v >> 8;
}

unsigned short Packet::readUShortAt(int pos) {
  if (pos + (int) sizeof(unsigned short) > mMaxSize)
    throw overflow_error("Packet max size overflow");

  unsigned char c1 = static_cast <unsigned char> (mData[pos]);
  unsigned char c2 = static_cast <unsigned char> (mData[pos + 1]);
  return c1 + (c2 << 8);
}

short Packet::readShortAt(int pos) {
  if (pos + (int) sizeof(short) > mMaxSize)
    throw overflow_error("Packet max size overflow");

  unsigned char c1 = static_cast <unsigned char> (mData[pos]);
  unsigned char c2 = static_cast <unsigned char> (mData[pos + 1]);
  return c1 + (c2 << 8);
}

unsigned char Packet::readUCharAt(int pos) {
  if (pos + (int) sizeof(unsigned char) > mMaxSize)
    throw overflow_error("Packet max size overflow");

  return static_cast <unsigned char> (mData[pos]);
}

void Packet::print() {
  cout << "Content:" << endl;
  for (int i=0; i<mSize; i++) {
    cout << setw(6) << i << ": ";
    cout << uppercase << setw(2) << setfill('0') << hex << (((int) mData[i]) & 0xFF);
    if (-mData[i] >= 'A' && -mData[i] <= 'Z')
      cout << " (" << (char) -mData[i] << ")";
    cout << nouppercase << dec << setw(0) << setfill(' '); // restore cout
    cout << endl;
  }
}
