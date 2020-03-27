#include "serial_handling.h"

extern shared_vars shared;

String readline() {
  // max size of buffer, including null terminator
  const uint16_t buf_size = 256;

  // current number of chars in buffer, not counting null terminator
  uint16_t buf_len = 0;

  // input buffer
  char* buffer = (char *) malloc(buf_size);

  // set up buffer as empty string
  buf_len = 0;
  buffer[buf_len] = 0;

  char in_char;

  // set up loop to read all characters
  while (true) {
    if (Serial.available()) {
      // read the incoming byte:
      char in_char = Serial.read();

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
        // now we process the buffer
        return buffer;
      } else if (buf_len < buf_size-1) {
        // add character to buffer, provided that we don't overflow.
        // drop any excess characters.
        buffer[buf_len] = in_char;
        buf_len++;
        buffer[buf_len] = 0;
      } else {
        return "nope";
      }
    }
  }
}

uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment

  // print client request to serial monitor
  Serial.print("R");
  Serial.print(" ");
  Serial.print(start.lat);
  Serial.print(" ");
  Serial.print(start.lon);
  Serial.print(" ");
  Serial.print(end.lat);
  Serial.print(" ");
  Serial.println(end.lon);
  
  String currLine = readline();

  // read input from Serial protocol
  if (currLine[0] == 'N') {
    // read in the number of waypoints
    shared.num_waypoints = (currLine.substring(2)).toInt();
    // acknowledge receipt of number of waypoints
    Serial.println('A');
  } else {
    // read was unsuccesful
    return 0;
  }

  // acknowledge receipt of each waypoint
  for (int i = 0; i < shared.num_waypoints; i++) {
    currLine = readline();
    if (currLine[0] == 'W') {
      // then we are reading a waypoint
      String coordinates = currLine.substring(2);
      // retrive latitude
      int firstSpace = currLine.indexOf(" ", 2);
      (shared.waypoints[i]).lat = (currLine.substring(2, firstSpace)).toInt();
      // retrive longitude
      (shared.waypoints[i]).lon = (currLine.substring(firstSpace + 1)).toInt();
      // acknowledge receipt of waypoint
      Serial.println('A');
    } else {
      // read was unsuccessful
      return 0;
    }
  }
  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
}
