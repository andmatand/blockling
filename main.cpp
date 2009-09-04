/*
 *   main.cpp
 *      
 *   Copyright 2009 Andrew Anderson <billamonster.com>
 *      
 *   This file is part of Blockman.
 *
 *   Blockman is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Blockman is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.*
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Blockman.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "main.h"


int main(int argc, char** argv) {
	// Initialize SDL's subsystems
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);
	
	screenSurface = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_SWSURFACE);
	
	if (screenSurface == NULL) {
		fprintf(stderr, "Unable to set up video: %s\n", SDL_GetError());
		exit(1);
	}
	
	SDL_WM_SetCaption("Blockman", NULL);
	SDL_ShowCursor(SDL_DISABLE);
	
	Init();

	bool quitProgram = false;
	while (true) {
		switch (MainMenu()) {
			case 0: // Start Game
				Game(); // (with no replay file)
				break;
			case 1: // Options
				//switch(OptionsMenu()) {
				//}
				break;
			case -1: // Esc
			case -2: // Close Window
			case 2: // Quit
				quitProgram = true;
				break;
		}
		if (quitProgram) break;	
	}
	
	return 0;
}







void Init() {
	
	LoadTileset("default");
	LoadFont("data/font/font.bmp");
	
	
	/*** Keyboard ***/

	// Default Game keyboard layout
	gameKeys[0].sym = SDLK_ESCAPE;	// Quit
	gameKeys[0].mod = KMOD_NONE;	// modifier
	
	gameKeys[1].sym = SDLK_u;	// Undo
	gameKeys[1].mod = KMOD_NONE;	// modifier
	
	//gameKeys[2].sym = SDLK_LEFT;	// Move camera left
	//gameKeys[2].mod = KMOD_LSHIFT;	// modifier
	gameKeys[2].sym = SDLK_a;	// Move camera left
	gameKeys[2].mod = KMOD_NONE;	// modifier

	//gameKeys[3].sym = SDLK_RIGHT;	// Move camera right
	//gameKeys[3].mod = KMOD_LSHIFT;	// modifier
	gameKeys[3].sym = SDLK_d;	// Move camera right
	gameKeys[3].mod = KMOD_NONE;	// modifier
	
	//gameKeys[4].sym = SDLK_UP;	// Move camera up
	//gameKeys[4].mod = KMOD_LSHIFT;	// modifier
	gameKeys[4].sym = SDLK_w;	// Move camera up
	gameKeys[4].mod = KMOD_NONE;	// modifier
	
	//gameKeys[5].sym = SDLK_DOWN;	// Move camera down
	//gameKeys[5].mod = KMOD_LSHIFT;	// modifier
	gameKeys[5].sym = SDLK_s;	// Move camera down
	gameKeys[5].mod = KMOD_NONE;	// modifier

	
	
	
}
