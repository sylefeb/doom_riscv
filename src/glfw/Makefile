CC=gcc
CFLAGS=-g -m32 -Wall -funsigned-char
LDFLAGS=
LIBS=-lopengl32 -lglfw3 -lgdi32

CFLAGS+=\
	-DRANGECHECK \
	-DGLFW \
	$(NULL)

include ../sources.mk

SOURCES_doom_arch = \
	i_main.c \
	i_net.c \
	i_sound.c \
	i_system.c \
	i_video.c \
	d_main.c \
	s_sound.c \
	$(NULL)

# Filter out d_main, we provide our own simplified one
SOURCES_doom := $(filter-out d_main.c,$(SOURCES_doom))

# Filter out s_sound, we provide a dummy one
SOURCES_doom := $(filter-out s_sound.c,$(SOURCES_doom))

OBJS += $(addprefix objs/, \
	$(SOURCES_doom:.c=.o) \
	$(SOURCES_doom_arch:.c=.o) \
)

all: doom-glfw

doom-glfw: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f objs/*.o doom-glfw

objs/%.o: %.c | objs
	$(CC) $(CFLAGS) -I.. -c -o $@ $<

objs/%.o: ../%.c | objs
	$(CC) $(CFLAGS) -c -o $@ $<

objs:
	mkdir objs

.PHONY: all clean
