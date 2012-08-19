/*
 * altoyduino Emulator
 * Copyright 2012 Stefan Lange-Hegermann
 * *
 * altoyduino Emulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * altoyduino Emulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with altoyduino Emulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef emulino_display_h

#include "util.h"
#include "arduino.h"

#define emulino_display_h

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 48

#define DISPLAY_PIN_SCLK ARDUINO_PIN_13  // serial clock (Arduino 13)
#define DISPLAY_PIN_SDIN ARDUINO_PIN_4 // serial input (Arduino 4)
#define DISPLAY_PIN_RST ARDUINO_PIN_6  // Reset All (Arduino 6)
#define DISPLAY_PIN_DC ARDUINO_PIN_2   // Data/Command selector (Arduino 2)
#define DISPLAY_PIN_SCE ARDUINO_PIN_7  // Data Clock (Arduino 7)

void display_init(int display_scale);
void display_update();
void display_pin_callback(int pin, bool state);

#endif
