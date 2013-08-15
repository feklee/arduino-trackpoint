// Copyright (C) 2013 [Felix E. Klee](mailto:felix.klee@inka.de)
//
// Based on Chris J. Kiick's Arduino PS/2 library, released in January 2008
// into the public domain.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef Trackpoint_h
#define Trackpoint_h

#include "Arduino.h"

class Trackpoint {
public:
  struct DataReport {
    byte state;
    byte x;
    byte y;
  };

  Trackpoint(byte clkPin, byte dataPin, byte resetPin);
  void writeToRamLocation(byte location, byte value); 
  byte readFromRamLocation(byte location);
  void setSensitivityFactor(byte sensitivity);
  byte sensitivityFactor();
  void setRemoteMode();
  void reset();
  DataReport readData();
  void write(byte data);
  byte read(void);

private:
  byte _resetPin;
  byte _clkPin;
  byte _dataPin;

  void golo(byte pin);
  void gohi(byte pin);
};

#endif
