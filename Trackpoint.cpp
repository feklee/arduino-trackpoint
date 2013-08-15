// For interfacing with a TrackPoint 4, or a compatible device.
//
// Based on the Arduino PS/2 library (`ps2.h`, `ps2.cpp`), written by Chris J.
// Kiick in January 2008 and released into the public domain. As of August
// 2013, it is available on:
//
// <http://playground.arduino.cc/componentLib/Ps2mouse> 
//
// Limitations according to `ps2.cpp`:
//
// * We do not handle parity errors.
//
// * The timing constants are hard coded from the spec. Data rate is not
//   impressive.
//
// * Probably lots of room for optimization.

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

#include "Trackpoint.h"

Trackpoint::Trackpoint(byte clkPin, byte dataPin, byte resetPin) :
  _resetPin(resetPin), _clkPin(clkPin), _dataPin(dataPin)
{
  gohi(_clkPin);
  gohi(_dataPin);
}

// errors are ignored
void Trackpoint::writeToRamLocation(byte location, byte value) {
  write(0xe2);
  read(); // ACK
  write(0x81);
  read(); // ACK
  write(location);
  read(); // ACK
  write(value);
  read(); // ACK
}

// undefined in case of error
byte Trackpoint::readFromRamLocation(byte location) {
  write(0xe2);
  read(); // ACK
  write(0x80);
  read(); // ACK
  write(location);
  read(); // ACK
  return read();
}

void Trackpoint::setSensitivityFactor(byte sensitivityFactor) {
  writeToRamLocation(0x4a, sensitivityFactor);
}

byte Trackpoint::sensitivityFactor() {
  return readFromRamLocation(0x4a);
}

void Trackpoint::setRemoteMode() {
  write(0xf0);
  read();
}

void Trackpoint::reset() {
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, HIGH);
  delay(2000); // empirical value
  digitalWrite(_resetPin, LOW);
}

Trackpoint::DataReport Trackpoint::readData() {
  DataReport d;

  write(0xeb);
  read(); // ACK
  d.state = read();
  d.x = read();
  d.y = read();

  return d;
}

// Comments and code below mostly unchanged from `ps2.h`.

/*
 * according to some code I saw, these functions will
 * correctly set the clock and data pins for
 * various conditions.  It's done this way so you don't need
 * pullup resistors.
 */
void Trackpoint::gohi(byte pin) {
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void Trackpoint::golo(byte pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

/* write a byte to the PS2 device */
void Trackpoint::write(byte data) {
  byte i;
  byte parity = 1;
	
  gohi(_dataPin);
  gohi(_clkPin);
  delayMicroseconds(300);
  golo(_clkPin);
  delayMicroseconds(300);
  golo(_dataPin);
  delayMicroseconds(10);
  gohi(_clkPin);	// start bit
  /* wait for device to take control of clock */
  while (digitalRead(_clkPin) == HIGH)
    ;	// this loop intentionally left blank
  // clear to send data
  for (i=0; i < 8; i++)
	{
      if (data & 0x01)
		{
          gohi(_dataPin);
		} else {
        golo(_dataPin);
      }
      // wait for clock
      while (digitalRead(_clkPin) == LOW)
        ;
      while (digitalRead(_clkPin) == HIGH)
        ;
      parity = parity ^ (data & 0x01);
      data = data >> 1;
	}
  // parity bit
  if (parity)
	{
      gohi(_dataPin);
	} else {
    golo(_dataPin);
  }
  // clock cycle - like ack.
  while (digitalRead(_clkPin) == LOW)
    ;
  while (digitalRead(_clkPin) == HIGH)
    ;
  // stop bit
  gohi(_dataPin);
  delayMicroseconds(50);
  while (digitalRead(_clkPin) == HIGH)
    ;
  // mode switch
  while ((digitalRead(_clkPin) == LOW) || (digitalRead(_dataPin) == LOW))
    ;
  // hold up incoming data
  golo(_clkPin);
}


/*
 * read a byte of data from the ps2 device.  Ignores parity.
 */
byte Trackpoint::read(void) {
  byte data = 0x00;
  byte i;
  byte bit = 0x01;

  // start clock
  gohi(_clkPin);
  gohi(_dataPin);
  delayMicroseconds(50);
  while (digitalRead(_clkPin) == HIGH)
    ;
  delayMicroseconds(5);	// not sure why.
  while (digitalRead(_clkPin) == LOW)
    ;	// eat start bit
  for (i=0; i < 8; i++)
	{
      while (digitalRead(_clkPin) == HIGH)
        ;
      if (digitalRead(_dataPin) == HIGH)
		{
          data = data | bit;
		}
      while (digitalRead(_clkPin) == LOW)
        ;
      bit = bit << 1;
	}
  // eat parity bit, ignore it.
  while (digitalRead(_clkPin) == HIGH)
    ;
  while (digitalRead(_clkPin) == LOW)
    ;
  // eat stop bit
  while (digitalRead(_clkPin) == HIGH)
    ;
  while (digitalRead(_clkPin) == LOW)
    ;
  golo(_clkPin);	// hold incoming data

  return data;
}
