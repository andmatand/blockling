SHELL=/bin/sh

DATADIR=data/
SRCDIR=../../src/

CROSS=i686-pc-mingw32-
CXX=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
CFLAGS=-ggdb -Wall -D_GNU_SOURCE=1 -Dmain=SDL_main -DDATA_PATH="\"$(DATADIR)\""
SDL_INCLUDE=-I$(MXE_PATH)/include/SDL
INCLUDE=-I$(MXE_PATH)/include
LIB_PATH=-L$(MXE_PATH)/lib
LIBS=-lmingw32 -mwindows -lSDLmain SDL.dll SDL_mixer.dll -lwinmm

default: all

all:

	$(CXX) $(CFLAGS) $(SRCDIR)main.cpp $(SDL_INCLUDE) $(INCLUDE) $(LIB_PATH) $(LIBS) -o ./blockling.exe
	$(CROSS)strip blockling.exe
