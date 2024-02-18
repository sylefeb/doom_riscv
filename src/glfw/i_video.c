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
#include "d_event.h"

// #include "i_system.h"
// #include "v_video.h"

extern  byte*   screens[5];
extern  byte    gammatable[5][256];
extern  int     usegamma;

// #include "i_video.h"

#include <stdlib.h>

static uint32_t *video_pal = NULL;

static GLFWwindow* window = NULL;

static GLuint texture = 0;

int     glfw_num_events = 0;
#define MAX_EVENTS 128
event_t glfw_events[MAX_EVENTS];

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const char map[] = {
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
	};
  if (glfw_num_events == MAX_EVENTS) {
    return;
  }
  event_t *event = &glfw_events[glfw_num_events++];
  event->type  = action == GLFW_PRESS ? ev_keydown : ev_keyup;
  switch (key)
  {
    case GLFW_KEY_F1:     event->data1 = KEY_F1; break;
    case GLFW_KEY_ENTER:  event->data1 = KEY_ENTER; break;
    case GLFW_KEY_UP:     event->data1 = KEY_UPARROW; break;
    case GLFW_KEY_DOWN:   event->data1 = KEY_DOWNARROW; break;
    case GLFW_KEY_LEFT:   event->data1 = KEY_LEFTARROW; break;
    case GLFW_KEY_RIGHT:  event->data1 = KEY_RIGHTARROW; break;
    case GLFW_KEY_ESCAPE: event->data1 = KEY_ESCAPE; break;
    default: event->data1 = key; break;
  }
}

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
  glfwSetKeyCallback(window, key_callback);
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

    unsigned short rgb565 = ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);
    r = (rgb565 & 31) << 3;
    g = ((rgb565>>5)  & 63) << 2;
    b = ((rgb565>>11) & 63) << 3;

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
  for (int e = 0;e < glfw_num_events; ++e) {
    D_PostEvent(&glfw_events[e]);
  }
  glfw_num_events = 0;
}
