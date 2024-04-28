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

// #define SIMULATION

#include "../../../libs/gpu.h"
#include "r_draw.h"

extern int gpu_enabled;

void
I_GPUEnable_Changed()
{
  if (gpu_enabled) {
    // nothing to do
  } else {
    // wait for current frame to finish
    gpu_sync_frame();
  }
}

void
I_InitGraphics(void)
{

  // initialize screen
  screen_init();

  // clear text buffer
  gpu_txt_start();
  for (int i = 0; i<TEXT_BUFFER_LEN ; ++i) {
    *GPU = 0;
    GPU_COM_WAIT;
  }
  gpu_txt_end();

	/* Don't need to do anything else really ... */

	/* Ok, maybe just set gamma default */
	usegamma = 0;
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

  gpu_pal_start();
  for (int i=0 ; i<256 ; i++) {
    r = gammatable[usegamma][*palette++];
    g = gammatable[usegamma][*palette++];
    b = gammatable[usegamma][*palette++];
    gpu_pal_rgb(r,g,b);
  }
  gpu_pal_end();

}

void
I_UpdateNoBlit(void)
{
}

extern fixed_t  viewcos;
extern fixed_t  viewsin;
extern fixed_t  viewx;
extern fixed_t  viewy;
extern int current_overlay;

void I_GPUFrame_Start()
{
  if (gpu_enabled) {

    unsigned int before = cpu_time();
    gpu_sync_frame();
    unsigned int after  = cpu_time();
    // printf("%d wait cycles\n",after-before);

    static unsigned int tm_tot_cycles  = 0;
    static int          tm_frame_count = 0;

    tm_tot_cycles += after - before;
    ++ tm_frame_count;
    if (tm_frame_count == 64) {
      printf("%d wait cycles/frame\n",tm_tot_cycles>>6);
      tm_frame_count = 0;
      tm_tot_cycles  = 0;
    }

    gpu_col_select(0);

    // FIXME: why? needed only in hardware
    gpu_col_send(
      PARAMETER_UV_OFFSET(0),
      PARAMETER_UV_OFFSET_EX(0) | PARAMETER
    );

    // send view params
    gpu_col_send(
        PARAMETER_UV_OFFSET(-viewy>>6),
        PARAMETER_UV_OFFSET_EX(-viewx>>6) | PARAMETER
    );

    current_overlay = 0;

    // produce plane parameters for all columns
    for (int c = 0 ; c != 320 ; ++c) {
      const int flat_scale = 2583; // 3100; // might need small adjustments
      int rz = flat_scale;
      int cx = (c - SCREENWIDTH/2) * flat_scale * 2 / (SCREENWIDTH);
      int du = dot3( cx,0,rz, -viewsin>>4,0,-viewcos>>4 ) >> 14;
      int dv = dot3( cx,0,rz,  viewcos>>4,0,-viewsin>>4 ) >> 14;
      gpu_col_send(
          PARAMETER_PLANE_A(256,0,0),
          PARAMETER_PLANE_A_EX(du,dv) | PARAMETER
      );
      gpu_col_send(0, COLDRAW_INC);
    }
  }
}

void
I_FinishUpdate (void)
{
  if (!gpu_enabled) {

    const int W = 320;
    const int H = 200;

    gpu_frame_start();
    const int *row = (int*)screens[0];
    for (int j=0;j<H;j++) {
      const int *ptr = row;
      for (int i=0;i<W/4;i++) {
        *GPU = *(ptr++);
        GPU_COM_WAIT;
      }
      row += W/4;
    }
    gpu_frame_end();

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
I_GPUFrame_End()
{
  if (!gpu_enabled) {
    return;
  }

  static unsigned int tm_frame_start = 0;
  static unsigned int tm_tot_cycles  = 0;
  static int tm_frame_count = 0;

  unsigned int now = cpu_time();
  if (tm_frame_count > 0) {
    tm_tot_cycles += now - tm_frame_start;
  }
  ++ tm_frame_count;
  if (tm_frame_count == 64) {
    printf("%d cycles/frame\n",tm_tot_cycles>>6);
    tm_frame_count = 0;
    tm_tot_cycles  = 0;
  }
  tm_frame_start   = now;

  gpu_draw_frame();

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
