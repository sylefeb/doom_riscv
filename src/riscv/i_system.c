/*
 * i_system.c
 *
 * System support code
 *
 * Copyright (C) 1993-1996 by id Software, Inc.
 * Copyright (C) 2021 Sylvain Munaut
 * All rights reserved.
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
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomdef.h"
#include "doomstat.h"

#include "d_main.h"
#include "g_game.h"
#include "m_misc.h"
#include "i_sound.h"
#include "i_video.h"

#include "i_system.h"

#include "console.h"
#include "config.h"

#include "../../libs/gpu.h"
#include "../../libs/kb.h"
// #include "../../libs/buttons.h"

extern int      gpu_enabled;

void
I_Init(void) { }


byte *
I_ZoneBase(int *size)
{
	/* Give 6M to DOOM */
	*size = 6 * 1024 * 1024;
	return (byte *) malloc (*size);
}

static int time = 0;

int
I_GetTime(void)
{
  return ++time;  /////// SL: FIXME: this is just a hack to get things running
}


static void
I_GetRemoteEvent(void)
{
	event_t event;

	//static byte s_btn = 0;
	//boolean mupd = false;
	//int mdx = 0;
	//int mdy = 0;

  // from keyboard
  unsigned int ch = console_getchar_nowait();
  if (ch != KB_NONE) {
    if (ch == '*') {
    /*
      // boom!
      printf("GPU reset\n");
      gpu_warmboot();
      printf("CPU reset\n");
      // flush the cache
      volatile int *flush = (volatile int*)0; // sure
      for (int i = 0; i < 65536 ; ++i) {
        *LEDS = *(flush++);
      }
      *LEDS  = 1;
      *RESET = 1;
    */
    } else if (ch == '/') {
      gpu_enabled = 1 - gpu_enabled;
      I_GPUEnable_Changed();
    }
    event.type = (ch & KB_UNPRESSED) ? ev_keyup : ev_keydown;
    ch         = ch & ~KB_UNPRESSED;
    switch (ch) {
      case KB_UP:     event.data1 = KEY_UPARROW;    break;
      case KB_LEFT:   event.data1 = KEY_LEFTARROW;  break;
      case KB_DOWN:   event.data1 = KEY_DOWNARROW;  break;
      case KB_RIGHT:  event.data1 = KEY_RIGHTARROW; break;
      case KB_RSHIFT: event.data1 = KEY_RSHIFT; break;
      case KB_LSHIFT: event.data1 = KEY_RSHIFT; break;
      case KB_LCTRL:  event.data1 = KEY_RCTRL; break;
      case KB_LALT:   event.data1 = KEY_RALT; break;
      case KB_ESC:    event.data1 = KEY_ESCAPE; break;
      case '\n':      event.data1 = KEY_ENTER; break;
      case KB_TAB:    event.data1 = KEY_TAB; break;
      default: event.data1 = ch;
    }
    D_PostEvent(&event);
  }
  // buttons
  static int prev_btns = 0;
  int        btns      = 0; // get_buttons();
  for (int b ; b<8 ; ++b) {
    int prev = prev_btns & (1<<b);
    int curr = btns      & (1<<b);
    if (curr ^ prev) {
      // changed
      event.type = curr ? ev_keyup : ev_keydown;
      // set data
      switch (ch) {
        case 1:         event.data1 = KEY_UPARROW;    break;
        case 2:         event.data1 = KEY_LEFTARROW;  break;
        case 4:         event.data1 = KEY_DOWNARROW;  break;
        case 8:         event.data1 = KEY_RIGHTARROW; break;
        case 16:        event.data1 = KEY_RSHIFT;     break;
        case 32:        event.data1 = KEY_RCTRL;      break;
        case 64:        event.data1 = KEY_RALT;       break;
        case 128:       event.data1 = KEY_ESCAPE;     break;
      }
      // post
      D_PostEvent(&event);
    }
  }
  prev_btns = btns;

}

  /*
      KEY_LEFTARROW,  // 0
      KEY_RIGHTARROW, // 1
      KEY_DOWNARROW,  // 2
      KEY_UPARROW,    // 3
      KEY_RSHIFT,     // 4
      KEY_RCTRL,      // 5
      KEY_RALT,       // 6
      KEY_ESCAPE,     // 7
      KEY_ENTER,      // 8
      KEY_TAB,        // 9
      KEY_BACKSPACE,  // 10
      KEY_PAUSE,      // 11
      KEY_EQUALS,     // 12
      KEY_MINUS,      // 13
      KEY_F1,         // 14
      KEY_F2,         // 15
      KEY_F3,         // 16
      KEY_F4,         // 17
      KEY_F5,         // 18
      KEY_F6,         // 19
      KEY_F7,         // 20
      KEY_F8,         // 21
      KEY_F9,         // 22
      KEY_F10,        // 23
      KEY_F11,        // 24
      KEY_F12,        // 25
  */
  /*
	if (mupd) {
		event.type = ev_mouse;
		event.data1 = s_btn;
		event.data2 =   mdx << 2;
		event.data3 = - mdy << 2;	// Doom is sort of inverted ...
		D_PostEvent(&event);
	}
  */

extern fixed_t  viewcos;
extern fixed_t  viewsin;
extern fixed_t  viewx;
extern fixed_t  viewy;

void
I_StartFrame(void)
{
}

void
I_StartTic(void)
{
	I_GetRemoteEvent();
}

ticcmd_t *
I_BaseTiccmd(void)
{
	static ticcmd_t emptycmd;
	return &emptycmd;
}


void
I_Quit(void)
{
	D_QuitNetGame();
	M_SaveDefaults();
	I_ShutdownGraphics();
	exit(0);
}


byte *
I_AllocLow(int length)
{
	byte*	mem;
	mem = (byte *)malloc (length);
	memset (mem,0,length);
	return mem;
}


void
I_Tactile
( int on,
  int off,
  int total )
{
	// UNUSED.
	on = off = total = 0;
}


void
I_Error(char *error, ...)
{
	va_list	argptr;

	// Message first.
	va_start (argptr,error);
	fprintf (stderr, "Error: ");
	vfprintf (stderr,error,argptr);
	fprintf (stderr, "\n");
	va_end (argptr);

	fflush( stderr );

	// Shutdown. Here might be other errors.
	if (demorecording)
		G_CheckDemoStatus();

	D_QuitNetGame ();
	I_ShutdownGraphics();

	exit(-1);
}
