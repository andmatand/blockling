SHELL = /bin/sh

DATADIR = data/
srcdir = src/

CC = g++
CFLAGS = -ggdb -Wall -Wextra `sdl-config --cflags --libs` -lSDL_mixer -DDATA_PATH="\"$(DATADIR)\""

# Comment out the line below to disable debug mode
#DEFINE=-DDEBUG

default: all

all:

	$(CC) $(CFLAGS) $(DEFINE) -o ./blockling $(srcdir)main.cpp
