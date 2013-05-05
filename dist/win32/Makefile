SHELL=/bin/sh

DATADIR=data/
SRCDIR=../../src/

CROSS=i686-pc-mingw32-
CC=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
CFLAGS=-ggdb -Wall -D_GNU_SOURCE=1 -Dmain=SDL_main -DDATA_PATH="\"$(DATADIR)\""
SDL_INCLUDE=-I$(MXE_PATH)usr/i686-pc-mingw32/include/SDL
INCLUDE=-I$(MXE_PATH)usr/i686-pc-mingw32/include
LIB_PATH=-L$(MXE_PATH)usr/i686-pc-mingw32/lib
LIBS=-lmingw32 -mwindows -lSDLmain SDL.dll SDL_mixer.dll -lwinmm

default: all

all:

	$(CC) $(CFLAGS) $(SRCDIR)main.cpp $(SDL_INCLUDE) $(INCLUDE) $(LIB_PATH) $(LIBS) -o ./blockling.exe
	$(MXE_PATH)usr/bin/i686-pc-mingw32-strip blockling.exe
