// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      System specific interface stuff.
//
//-----------------------------------------------------------------------------


#ifndef __R_DRAW__
#define __R_DRAW__


#ifdef __GNUG__
#pragma interface
#endif


extern lighttable_t*    dc_colormap;
extern int              dc_x;
extern int              dc_yl;
extern int              dc_yh;
extern fixed_t          dc_iscale;
extern fixed_t          dc_texturemid;
extern short            dc_texid;
extern int              dc_u;
extern int              dc_voffset;
extern byte             dc_light;
extern int              dc_is_overlay;

// first pixel in a column
extern byte*            dc_source;

//
// Wall span nfo
//
typedef struct {
  int    vstep;
  short  vinit;
  short  u;
} t_wallspan;

//
// Floor span nfo
//
typedef struct {
  short  height;
  short  yshift;
} t_flatspan;

#define SPAN_WALL 0
#define SPAN_FLAT 1

//
// Span record
//
typedef struct s_spanrecord {
  byte   type; // 0: wall, 1: flat
  short  yl;
  short  yh;
  short  texid;
  byte   light;
  union {
    t_wallspan wall;
    t_flatspan flat;
  };
  struct s_spanrecord *next;
} t_spanrecord;

extern t_spanrecord **dc_spanrecords;

// Span records for the GPU
void          R_ClearSpanRecords(void);
void          R_InitSpanRecords(void);
t_spanrecord *R_AddSpanRecord(int col);

// The span blitting interface.
// Hook in assembler or system specific BLT
//  here.
void    R_DrawColumn (void);
void    R_DrawColumnLow (void);

// The Spectre/Invisibility effect.
void    R_DrawFuzzColumn (void);
void    R_DrawFuzzColumnLow (void);

// Draw with color translation tables,
//  for player sprite rendering,
//  Green/Red/Blue/Indigo shirts.
void    R_DrawTranslatedColumn (void);
void    R_DrawTranslatedColumnLow (void);

void
R_VideoErase
( unsigned      ofs,
  int           count );

extern int              ds_y;
extern int              ds_x1;
extern int              ds_x2;

extern lighttable_t*    ds_colormap;

extern fixed_t          ds_xfrac;
extern fixed_t          ds_yfrac;
extern fixed_t          ds_xstep;
extern fixed_t          ds_ystep;

// start of a 64*64 tile image
extern byte*            ds_source;

extern byte*            translationtables;
extern byte*            dc_translation;


// Span blitting for rows, floor/ceiling.
// No Sepctre effect needed.
void    R_DrawSpan (void);

// Low resolution mode, 160x200?
void    R_DrawSpanLow (void);


void
R_InitBuffer
( int           width,
  int           height );


// Initialize color translation tables,
//  for player rendering etc.
void    R_InitTranslationTables (void);



// Rendering function.
void R_FillBackScreen (void);

// If the view size is not full screen, draws a border around it.
void R_DrawViewBorder (void);



#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
