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

static uint16_t *video_pal = NULL;

// #define SIMULATION
// #define NO_DISPLAY

#include "../../../libs/gpu.h"

void
I_InitGraphics(void)
{

#ifndef NO_DISPLAY
  // initialize GPU
  gpu_init();
#endif

  video_pal = (uint16_t *)malloc(sizeof(uint16_t)*256);

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
		video_pal[i] = ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);
	}
}

void
I_UpdateNoBlit(void)
{
}

static inline send_screen_byte(unsigned int by)
{
  *GPU = 0x00000000; GPU_COM_WAIT; *GPU = 0x00000000; GPU_COM_WAIT;
  *GPU = 0x00000001; GPU_COM_WAIT; *GPU = (by<<24);   GPU_COM_WAIT;
}

I_FinishUpdate (void)
{
#ifndef NO_DISPLAY
  const int W = 320;
  const int H = 240;
  /// painstakingly send frame to the gpu
  const unsigned char *ptr_col = screens[0];
  cpu_frame_start();
  for (int v=0;v<W;v++) {
    const unsigned char *ptr = (ptr_col++);
    int dupl = 0;
    for (int u=0;u<H;u++) {
      uint16_t clr = video_pal[*ptr];
      cpu_frame_pixel_565(clr);
      //uint8_t  h   = clr>>8;
      //uint8_t  l   = clr&255;
      //send_screen_byte(h); // first byte
      ++ dupl;          // increment source pointer
      if (dupl == 6) {
        dupl = 0;
      } else {
        ptr += W;
      }
      //send_screen_byte(l); // second byte
    }
  }
  cpu_frame_end();
#else
  printf("----- frame done -----\n");
#endif

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
