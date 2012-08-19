/*
 * emulino - arduino emulator
 * Copyright 2009 Greg Hewgill
 *
 * This file is part of Emulino.
 *
 * Emulino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emulino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emulino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "cpu.h"
#include "loader.h"
#include "arduino.h"
#include "display.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s image\n"
                        "       image is a raw binary or hex image file\n", argv[0]);
        exit(1);
    }

    /* Initialize the screen / window */
    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);
    display_init(8);
    SDL_WM_SetCaption("Altoyduino", NULL);

    int inf = 0;
    int outf = 1;

    int a = 1;
    while (a < argc) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-io") == 0) {
                a++;
                char fn[200];
                snprintf(fn, sizeof(fn), "%s.in", argv[a]);
                inf = open(fn, O_RDONLY);
                if (inf == -1) {
                    perror(fn);
                    exit(1);
                }
                snprintf(fn, sizeof(fn), "%s.out", argv[a]);
                outf = open(fn, O_WRONLY);
                if (outf == -1) {
                    perror(fn);
                    exit(1);
                }
            } else {
                fprintf(stderr, "Unknown option: %s\n", argv[a]);
                exit(1);
            }
        } else {
            break;
        }
        a++;
    }

    u8 prog[PROGRAM_SIZE_WORDS*2];
    u32 progsize = load_file(argv[a], prog, sizeof(prog));
    if (progsize == 0) {
        perror(argv[a]);
        exit(1);
    }

    u8 eeprom[512];
    u32 eepromsize = load_file("emulino.eeprom", eeprom, sizeof(eeprom));

    cpu_init();
    cpu_load_flash(prog, progsize);
    cpu_load_eeprom(eeprom, eepromsize);

    cpu_usart_set_input(inf);
    cpu_usart_set_output(outf);
    SDL_Event event;

    cpu_pin_callback(DISPLAY_PIN_DC, display_pin_callback);
    cpu_pin_callback(DISPLAY_PIN_RST, display_pin_callback);
    cpu_pin_callback(DISPLAY_PIN_SCE, display_pin_callback);
    cpu_pin_callback(DISPLAY_PIN_SCLK, display_pin_callback);
    cpu_pin_callback(DISPLAY_PIN_SDIN, display_pin_callback);


    int cnt=0;

    cpu_set_pin(ARDUINO_PIN_12, 1);
    cpu_set_pin(ARDUINO_PIN_11, 1);
    cpu_set_pin(ARDUINO_PIN_10, 1);
    cpu_set_pin(ARDUINO_PIN_8, 1);
    cpu_set_pin(ARDUINO_PIN_A0, 1);
    cpu_set_pin(ARDUINO_PIN_A1, 1);

    int val=0;
    for (;;) {
        //printf(".");
        if (cpu_run() == CPU_HALT) {
            break;
        }
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        if (event.type == SDL_KEYDOWN) {
            if (!val) {
                val=1;
                switch (event.key.keysym.scancode) {
                    case 36:  // ENTER (A)
                        cpu_set_pin(ARDUINO_PIN_11, 0);
                        break;
                    case 49:  // SPACE (B)
                        cpu_set_pin(ARDUINO_PIN_12, 0);
                        break;
                    case 125: // DOWN
                        cpu_set_pin(ARDUINO_PIN_10, 0);
                        break;
                    case 126: // UP
                        cpu_set_pin(ARDUINO_PIN_8, 0);
                        break;
                    case 123: // LEFT
                        cpu_set_pin(ARDUINO_PIN_A0, 0);
                        break;
                    case 124: // RIGHT
                        cpu_set_pin(ARDUINO_PIN_A1, 0);
                        break;
                    default:
                        break;
                }
            }
        } else if (event.type == SDL_KEYUP) {
            if (val) {
                val=0;
                switch (event.key.keysym.scancode) {
                    case 36:  // ENTER (A)
                        cpu_set_pin(ARDUINO_PIN_11, 1);
                        break;
                    case 49:  // SPACE (B)
                        cpu_set_pin(ARDUINO_PIN_12, 1);
                        break;
                    case 125: // DOWN
                        cpu_set_pin(ARDUINO_PIN_10, 1);
                        break;
                    case 126: // UP
                        cpu_set_pin(ARDUINO_PIN_8, 1);
                        break;
                    case 123: // LEFT
                        cpu_set_pin(ARDUINO_PIN_A0, 1);
                        break;
                    case 124: // RIGHT
                        cpu_set_pin(ARDUINO_PIN_A1, 1);
                        break;
                    default:
                        break;
                }
            }
        }
        if (60==cnt++) {
            cnt=0;
            display_update();
        }
    }
    fprintf(stderr, "cycles: %lu\n", cpu_get_cycles());
    return 0;
}
