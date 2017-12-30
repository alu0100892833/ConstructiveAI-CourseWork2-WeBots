/*
 * Description:  Implementation of Communication.hpp functions
 */

#include "Communication.hpp"
#include "Serial.hpp"
#include "Packet.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>
#include <cstring>

using namespace std;

static int trial = 3;

Communication::Communication() :
  mInitialized(false),
  mSerial(NULL)
{
}

Communication::~Communication() {
  cleanup();
}

bool Communication::initialize(const string &port) {
  try
  {
    mSerial = new Serial(port);
    char *answer;

    // Note: the first talk always fail because the first command is badly interpreted
    // A first write has to be performed to test
    mSerial->write("V\n", 2);
    mSerial->drain();

    answer = talk("V\n");
    if (!answer)
      throw runtime_error("Cannot talk with the robot");
    cout << "Running real e-puck (" << &answer[2] << ")" << endl;

    static const char *expectedFirmware = "Version 1.5.3 November 2016 (Webots)";
    if (strcmp(&answer[2], expectedFirmware) != 0) {
      cerr << "The firmware installed on the e-puck (" << &answer[2] << ") is not the expected one." << endl;
      cerr << "Please install the expected firmware: " << expectedFirmware << endl;
      free(answer);
      return false;
    }

    free(answer);

    answer = mSerial->readLine();
    free(answer);

    answer = talk("S\n");
    if (!answer)
      throw runtime_error("Cannot talk with the robot");
    free(answer);

    mInitialized = true;
    return true;
  }
  catch (const runtime_error &e)
  {
    cerr << "Bluetooth connection failed with this error" << endl;
    cerr << e.what() << endl;
    cerr << endl;
    cerr << "Please check:" << endl;
    cerr << "- your Bluetooth interface is setup properly on your computer" << endl;
    cerr << "- the ID of the robot corresponds to the bluetooth connection" << endl;
    cerr << "- the robot runs the latest version of the sercom program" << endl;
    cerr << "- the robot is not too far away from the Bluetooth antenna of the computer" << endl;
    cerr << "- the robot is switched on (the green led should be on)" << endl;
    cerr << "- the battery of the robot is fully charged (the red led should off)" << endl;
    cerr << "- the robot reset button was pressed" << endl;
    cleanup();
    return false;
  }
}

void Communication::cleanup() {
  mInitialized = false;
  if (mSerial) {
    delete mSerial;
    mSerial = NULL;
  }
}

bool Communication::sendPacket(const Packet *packet) {
  try {
    if (mSerial) {
      int packetSize = packet->size();
      if (packetSize > 0)
        mSerial->write(packet->data(), packetSize);
      return true;
    }
  }
  catch (const runtime_error &e)
  {
    cerr << "Cannot send packet to e-puck: " << e.what() << endl;
  }
  return false;
}

bool Communication::receivePacket(Packet *packet) {
  try
  {
    if (mSerial) {
      int packetSize = packet->size();
      if (packetSize > 0) {
        int readed = mSerial->read(packet->data(), packetSize, true);
        if (readed != packetSize) {
          stringstream s;
          s << "Expected data not received (expected=" << packetSize << ", received=" << readed << ")" << endl;
          mSerial->drain();
          throw runtime_error(s.str());
        }
      }
      return true;
    }
  }
  catch (const runtime_error &e)
  {
    cerr << "Cannot receive packet from e-puck: " << e.what() << endl;
  }
  return false;
}

char *Communication::talk(const char *source) {
  static const char *errorPrefix = "Cannot talk to e-puck: ";
  if (!mSerial) {
    cerr << errorPrefix << "serial failed" << endl;
    return NULL;
  }

  char expectedFirstChar = source[0] + 32; // lower case answer

  for (int i=0; i<trial; i++) { // several trials
    try {
      char *answer = mSerial->talk(source);
      if (answer && strlen(answer) > 0 && answer[0] == expectedFirstChar)
        return answer;

      if (answer)
        free(answer);

      throw runtime_error("Unexpected result");
    }
    catch (const runtime_error &e)
    {
      cerr << errorPrefix << e.what() << endl;
    }
    cerr << "Talk to e-puck failed... Retry (" << (i+1) << ")" << endl;
  }

  return NULL;
}

char *Communication::readLine() {
  static const char *errorPrefix = "Cannot read line: ";
  if (!mSerial) {
    cerr << errorPrefix << "serial failed" << endl;
    return NULL;
  }

  try {
    return mSerial->readLine();
  }
  catch (const runtime_error &e)
  {
    cerr << errorPrefix << e.what() << endl;
  }
  return NULL;
}
