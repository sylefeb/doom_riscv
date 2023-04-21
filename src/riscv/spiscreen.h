// MIT license, see LICENSE_MIT in Silice repo root
// @sylefeb 2021
// https://github.com/sylefeb/Silice

#define SPISCREEN_CMD   (1<< 9)

#define DELAY      (1<<17)
static inline void wait()  { asm volatile ("nop;"); }
static inline void pause() { for (int i=0;i<DELAY;i++) { asm volatile ("nop;"); } }

#define WAIT wait()

void spiscreen_init()
{
  // reset high
  *(SPISCREEN_RST) = 0;
  // wait > 100 msec
  pause();
  // reset low
  *(SPISCREEN_RST) = 1;
  // wait > 300us
  pause();
  // reset high
  *(SPISCREEN_RST) = 0;
  // wait > 300 msec
  pause();
  // software reset
  *(SPISCREEN) = (0x01) | SPISCREEN_CMD;
  // long wait
  pause();
  // sleep out
  *(SPISCREEN) = (0x11) | SPISCREEN_CMD;
  // long wait
  pause();
  // colmod
  *(SPISCREEN) = (0x3A) | SPISCREEN_CMD;
  WAIT;
  *(SPISCREEN) = (0x55);
  // long wait
  pause();
  // madctl
  *(SPISCREEN) = (0x36) | SPISCREEN_CMD;
  WAIT;
  *(SPISCREEN) = (0x40);
  // long wait
  pause();
  // invon
  *(SPISCREEN) = (0x21) | SPISCREEN_CMD;
  pause();
  // noron
  *(SPISCREEN) = (0x13) | SPISCREEN_CMD;
  pause();
  // brightness
  *(SPISCREEN) = (0x51) | SPISCREEN_CMD;
  WAIT;
  *(SPISCREEN) = (0xFF);
  WAIT;
  // display on
  *(SPISCREEN) = (0x29) | SPISCREEN_CMD;
  pause();
  // done!
}

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

void spiscreen_fullscreen()
{

  // set col addr
  *(SPISCREEN) = (0x2A) | SPISCREEN_CMD;
  WAIT;
  *(SPISCREEN) = (   0);
  WAIT;
  *(SPISCREEN) = (   0);
  WAIT;
  *(SPISCREEN) = ( (((SCREEN_HEIGHT-1)>>8)&255) );
  WAIT;
  *(SPISCREEN) = (   (SCREEN_HEIGHT-1)  &255 );
  WAIT;
  // set row addr
  *(SPISCREEN) = (0x2B) | SPISCREEN_CMD;
  WAIT;
  *(SPISCREEN) = (   0);
  WAIT;
  *(SPISCREEN) = (   0);
  WAIT;
  *(SPISCREEN) = ( (((SCREEN_WIDTH-1)>>8)&255) );
  WAIT;
  *(SPISCREEN) = (   (SCREEN_WIDTH-1)    &255 );
  WAIT;
  // initiate write
  *(SPISCREEN) = (0x2C) | SPISCREEN_CMD;
  WAIT;
}

static inline void spiscreen_pix_565(unsigned char b5g3,unsigned char g3r5)
{
    *(SPISCREEN) = b5g3;
    *(SPISCREEN) = g3r5;
}

void spiscreen_clear(unsigned char c)
{
  for (int u=0;u<SCREEN_WIDTH;u++) {
    for (int v=0;v<SCREEN_HEIGHT;v++) {
      spiscreen_pix_565(u,u);
    }
  }
}
