/*
 * Copyright(C) 1993-1996 Id Software, Inc.
 * Copyright(C) 2005-2014 Simon Howard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * DESCRIPTION:
 *  System specific interface stuff.
 */

#ifndef __I_VIDEO__
#define __I_VIDEO__

#include "doomtype.h"

// Screen width and height.

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

#define MAX_MOUSE_BUTTONS 8

typedef boolean (*grabmouse_callback_t)(void);

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void);

// Takes full 8 bit values.
void I_SetPalette(byte *palette);
int I_GetPaletteIndex(int r, int g, int b);

void I_FinishUpdate(void);

void I_ReadScreen(byte *scr);

void I_SetWindowTitle(const char *title);

void I_SetGrabMouseCallback(grabmouse_callback_t func);

void I_DisplayFPSDots(boolean dots_on);

// Called before processing any tics in a frame (just after displaying a frame).
// Time consuming syncronous operations are performed here (joystick reading).

void I_StartFrame(void);

// Called before processing each tic in a frame.
// Quick syncronous operations are performed here.

void I_StartTic(void);

extern char *video_driver;
extern boolean screenvisible;

extern float mouse_acceleration;
extern int mouse_threshold;
extern int vanilla_keyboard_mapping;
extern boolean screensaver_mode;
extern int usegamma;
extern byte *I_VideoBuffer;

extern int screen_width;
extern int screen_height;
extern int screen_bpp;
extern int fullscreen;
extern int aspect_ratio_correct;

#endif
