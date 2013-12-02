SHELL=/bin/sh

DATADIR=data/
SRCDIR=src/

CC=/opt/gcw0-toolchain/usr/bin/mipsel-linux-g++
CFLAGS=-std=c++98 -Wall -DDATA_PATH="\"$(DATADIR)\"" `sdl-config --cflags`
LIBS=`sdl-config --libs` -lSDL_mixer
TARGET=blockling

# Comment out the line below to disable debug mode
#DEFINE=-DDEBUG

default: all

all:
	$(CC) $(CFLAGS) $(DEFINE) $(SRCDIR)main.cpp $(LIBS) -o $(TARGET)
