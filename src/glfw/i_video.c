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

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <string.h>

#include "doomdef.h"

// #include "i_system.h"
// #include "v_video.h"

extern  byte*   screens[5];
extern  byte    gammatable[5][256];
extern  int     usegamma;

// #include "i_video.h"

#include <stdlib.h>

static uint32_t *video_pal = NULL;

#include "../../../libs/gpu.h"

static GLFWwindow* window = NULL;

static GLuint texture = 0;

void
I_InitGraphics(void)
{
  // glfw window
  if (!glfwInit()) {
    fprintf(stderr,"ERROR: cannot initialize glfw.");
    exit(-1);
  }
  window = glfwCreateWindow(320,200,"DOOM", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  // prepare texture
  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D,texture);
  glTexImage2D( GL_TEXTURE_2D,0, GL_RGBA,
                320,200,0,
                GL_RGBA,GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  // setup rendering
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f,1.0f,1.0f);
  // setup view
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  video_pal = (uint32_t *)malloc(sizeof(uint32_t)*256);

	/* Don't need to do anything else really ... */

	/* Ok, maybe just set gamma default */
	usegamma = 1;
}

void
I_ShutdownGraphics(void)
{
  // terminate
  glDeleteTextures(1,&texture);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void
I_SetPalette(byte* palette)
{
	byte r, g, b;

	for (int i=0 ; i<256 ; i++) {
		r = gammatable[usegamma][*palette++];
		g = gammatable[usegamma][*palette++];
		b = gammatable[usegamma][*palette++];
		video_pal[i] = r | (g << 8) | (b << 16);
	}
}

void
I_UpdateNoBlit(void)
{
}

static uint32_t frame[320*200];

void
I_FinishUpdate (void)
{
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  const unsigned char *ptr = screens[0];
  for (int i = 0; i < 320*200; ++i) {
    uint8_t pidx = *(ptr++);
    frame[i] = video_pal[pidx];
  }

  glTexSubImage2D( GL_TEXTURE_2D,0,0,0,
                   320,200,
                  GL_RGBA,GL_UNSIGNED_BYTE,
                  frame);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
  glEnd();

  glfwSwapBuffers(window);
  glfwPollEvents();
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

//
// I_StartFrame
//
void I_StartFrame (void)
{

}

void
I_StartTic(void)
{

}
