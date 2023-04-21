/*
 * i_video.c
 *
 * Video system support code
 *
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

 // SL: modified for the icestick doom

 */

#include <stdint.h>
#include <string.h>

#include "doomdef.h"

#include "i_system.h"
#include "v_video.h"
#include "i_video.h"

#include "config.h"
#include <stdlib.h>

static volatile int * const SPISCREEN	    = PTR_SPISCREEN;
static volatile int * const SPISCREEN_RST	= PTR_SPISCREEN_RST;

// Palette is at a fixed address in fastram
static uint16_t *video_pal = 0x03800000 + 1024 + 512;
// static uint16_t *video_pal = NULL;

#include "spiscreen.h"

void
I_InitGraphics(void)
{

  // initialize SPIscreen
  spiscreen_init();
  spiscreen_fullscreen();
  spiscreen_clear(0xff);

  // video_pal = (uint16_t *)malloc(sizeof(uint16_t)*256);

	/* Don't need to do anything else really ... */

	/* Ok, maybe just set gamma default */
	usegamma = 1;
}

void
I_ShutdownGraphics(void)
{
	/* Don't need to do anything really ... */
}

void
I_SetPalette(byte* palette)
{
	byte r, g, b;

	for (int i=0 ; i<256 ; i++) {
		r = gammatable[usegamma][*palette++];
		g = gammatable[usegamma][*palette++];
		b = gammatable[usegamma][*palette++];
		video_pal[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
	}
}

void
I_UpdateNoBlit(void)
{
}

__attribute__((section(".fastram"))) void
I_FinishUpdate (void)
{
  /// painstakingly send frame to the SPIscreen
  const unsigned char *ptr_col = screens[0];
  for (int v=0;v<SCREEN_WIDTH;v++) {
    const unsigned char *ptr = (ptr_col++);
    int dupl = 0;
    for (int u=0;u<SCREEN_HEIGHT;u++) {
      uint16_t clr = video_pal[*ptr];
      uint8_t  h   = clr>>8;
      uint8_t  l   = clr&255;
      *(SPISCREEN) = h; // first byte
      ++ dupl;          // increment source pointer
      if (dupl == 6) {
        dupl = 0;
      } else {
        ptr += SCREEN_WIDTH;
      }
      *(SPISCREEN) = l; // second byte (done after to absorb sending delay...)
    }
  }

	/* Very crude FPS measure (time to render 100 frames */
#if 0
	static int frame_cnt = 0;
	static int tick_prev = 0;

	if (++frame_cnt == 100)
	{
		int tick_now = I_GetTime();
		printf("%d\n", tick_now - tick_prev);
		tick_prev = tick_now;
		frame_cnt = 0;
	}
#endif
}


void
I_WaitVBL(int count)
{
	/* Buys-Wait for VBL status bit */
  // SL: TODO
}


__attribute__((section(".fastram")))
void
I_ReadScreen(byte* scr)
{
  unsigned int *to = (unsigned int *)scr;
  const unsigned int *from = screens[0];
  for (int i=0;i<(SCREENHEIGHT * SCREENWIDTH)>>2;++i) {
    *(to++) = *(from++);
  }

}


#if 0	/* WTF ? Not used ... */
void
I_BeginRead(void)
{
}

void
I_EndRead(void)
{
}
#endif
