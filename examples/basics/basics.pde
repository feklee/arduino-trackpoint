// Allows control of the mouse cursor on the connected computer, via a
// TrackPoint.
//
// Tested with an Arduino Micro (Leonardo compatible).

// Copyright (C) 2013 Felix E. Klee <felix.klee@inka.de>
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

Trackpoint trackpoint(8, // CLK
                      9, // DATA
                      12); // RESET

void setup() {
  Mouse.begin();
  trackpoint.reset();
  trackpoint.setRemoteMode();
  trackpoint.setSensitivityFactor(0xc0); // more sensitive than by default
}

void sendButtonState(byte state) {
  static const char hidStates[] = {MOUSE_LEFT, MOUSE_RIGHT};

  for (byte i = 0; i < sizeof(hidStates); i++) {
    byte hidState = hidStates[i];
    if (state & (1 << i)) {
      Mouse.press(hidState);
    } else if (Mouse.isPressed(hidState)) {
      Mouse.release(hidState);
    }
  }
}

// Reads TrackPoint data and sends data to computer.
void loop() {
  Trackpoint::DataReport d = trackpoint.readData();

  if (d.state & (1 << 2)) { // middle button down => scroll
    Mouse.move(0, 0, d.y);
  } else {
    Mouse.move(d.x, -d.y, 0);
    sendButtonState(d.state);
  }
}
