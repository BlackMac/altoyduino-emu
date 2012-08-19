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

#include <stdio.h>
#include <SDL/SDL.h>
#include "util.h"
#include "display.h"

static bool bit = 0;
static bool inverted = 0;
static Uint16 in = 0x0000;
static Uint16 cur_byte = 0x0000;
static char command = 1;
static SDL_Surface *screen;
static Uint16 x = 0;
static Uint16 scale = 0;

static void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
    if (x>DISPLAY_WIDTH-1 || x<0) return;
    if (y>DISPLAY_HEIGHT-1 || y<0) return;
    SDL_Rect rect = {x*scale,y*scale,scale,scale};
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, R, G, B));
}

void display_init(int display_scale)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("PCD8544", NULL);
    scale = display_scale;
    screen = SDL_SetVideoMode(DISPLAY_WIDTH*scale, DISPLAY_HEIGHT*scale, 8, SDL_HWSURFACE | SDL_DOUBLEBUF);
}

void display_update()
{
    SDL_Flip(screen);
}

static void display_reset()
{
    SDL_Rect rect = {0,0, DISPLAY_WIDTH*scale, DISPLAY_HEIGHT*scale};
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 157, 166, 157));
}

static void display_serial_clock() {
    if (in) {
        cur_byte = cur_byte | ( 1 << (7-bit) );
    }
    bit ++;
}

static void display_data_clock() {
    if (!command) {
        int a;
        for (a=0;a<8;a++) {
            if ((cur_byte) & (1<<a)) {
                if (!inverted)
                    DrawPixel(screen, x%DISPLAY_WIDTH, a+8*((int)(x/DISPLAY_WIDTH)), 48, 45, 34);
                else
                    DrawPixel(screen, x%DISPLAY_WIDTH, a+8*((int)(x/DISPLAY_WIDTH)), 157, 166, 157);
            } else {
                if (!inverted)
                    DrawPixel(screen, x%DISPLAY_WIDTH, a+8*((int)(x/DISPLAY_WIDTH)), 157, 166, 157);
                else
                    DrawPixel(screen, x%DISPLAY_WIDTH, a+8*((int)(x/DISPLAY_WIDTH)), 48, 45, 34);
            }
        }
        x++;


    } else {
        int curx = x%DISPLAY_WIDTH;
        int cury = ((int)x/DISPLAY_WIDTH)/8;
        if ((cur_byte >> 6) << 6 == 0x80) { // Set X-Position
            int newx=(cur_byte-0x80);
            x=cury*DISPLAY_WIDTH*8+newx;
        } else if ((cur_byte >> 6) << 6 == 0x40) { // set Y-Position
            int newy=(cur_byte-0x40);
            x=newy*DISPLAY_WIDTH+curx;
        } else if (cur_byte == 0x0D) {
            inverted = true;
        } else if (cur_byte == 0x0C) {
            inverted = false;
        }
    }
    cur_byte = 0x0;
    bit = 0;
}

static void display_serial_data_in( int value )
{
    in = value;
}

void display_pin_callback(int pin, bool state)
{
    switch (pin) {
        case DISPLAY_PIN_RST:
            if (state == 1) display_reset();
            break;
        case DISPLAY_PIN_DC:
            command = !state;
            break;
        case DISPLAY_PIN_SCLK:
            if (state == 1) display_serial_clock();
            break;
        case DISPLAY_PIN_SCE:
            if (state == 1) display_data_clock();
            break;
        case DISPLAY_PIN_SDIN:
            display_serial_data_in( state==1 );
            break;
        default:
            break;
    }
}
