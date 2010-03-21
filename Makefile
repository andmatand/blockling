SHELL = /bin/sh

DATADIR = data/
srcdir = src/

CC = g++
CFLAGS = -Wall -Wextra -lSDL -lSDL_mixer -DDATA_PATH="\"$(DATADIR)\""

default: all

all:

	$(CC) $(CFLAGS) -o ./blockling $(srcdir)main.cpp
