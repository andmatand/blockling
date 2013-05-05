SHELL=/bin/sh

DATADIR=data/
SRCDIR=src/

CC=g++
CFLAGS=-ggdb -Wall -DDATA_PATH="\"$(DATADIR)\""
LIBS=`sdl-config --libs` -lSDL_mixer

# Comment out the line below to disable debug mode
#DEFINE=-DDEBUG

default: all

all:

	$(CC) $(CFLAGS) $(DEFINE) $(SRCDIR)main.cpp $(LIBS) -o ./blockling
