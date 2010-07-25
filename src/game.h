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

#include "level.h"
#include "camera.h"


// Function Prototypes
int Game ();
void TutorialSpeech(bool reset);
char* LoadLevel(uint level);
FILE* OpenLevel(uint level);


// Variables
bool showingReplay;


class game {
	public:
		// Constructor prototype
		game();

	private:
		SDL_Surface *screenSurf;

		level *theLevel;
		camera *Camera;
};

game::game() {
	screenSurf = NULL;
	theLevel = NULL;
}
