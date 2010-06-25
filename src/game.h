/*
 *   Copyright 2010 Andrew Anderson <www.billamonster.com>
 *
 *   This file is part of Blockling.
 *
 *   Blockling is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Blockling is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.*
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Blockling.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "camera.h"

/** Function Prototypes ****/
int Game ();
void TutorialSpeech(bool reset);
void Undo(char action);
char* LoadLevel(uint level);
FILE* OpenLevel(uint level);


/** Variables ****/
bool showingReplay;


/** Game Classes **/
class level {
	public:
		// Constructor
		level():
			bricks(NULL),
			blocks(NULL),
			torches(NULL),
			spikes(NULL),
			telepads(NULL)
			{};


		// Accessor Methods
		int GetX() const { return x; };
		int GetY() const { return y; };
		int GetW() const { return w; };
		int GetH() const { return h; };

		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		void SetW(int width) { w = width; };
		void SetH(int height) { h = height; };


	private:
		int x, y;
		int w, h;

		bool timeRunning; // Has the timer started yet?
		uint time;        // How long the level has been running (in
		                  // milliseconds)

		uint numbricks;
		uint numblocks;
		uint numplayers;
		uint numtorches;
		uint numspikes;
		uint numtelepads;
		uint numitems;

		// Playfield objects
		brick *bricks;
		block *blocks;
		torch *torches;
		spike *spikes;
		telepad *telepads;

		// Other objects
		camera theCamera;
};


class game {
	public:

	private:
		SDL_Surface *screenSurf;

		level theLevel;
};
