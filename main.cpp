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
	
	Init();

	while (true) {
		
		//Menu();
		
		Game();
		
		if (gameKeys[0].on) break;
	}
	
	return 0;
}


void Init() {
	brickSurface = FillSurface("data/bmp/brick0.bmp", 1);
	blockSurface = FillSurface("data/bmp/block0.bmp", 1);
	spikeSurface = FillSurface("data/bmp/spike0.bmp", 1);
	exitSurface = FillSurface("data/bmp/exit0.bmp", 1);

	int a = 0, b = 0;
	char fn[32]; // filename
	int n;
	for (uint i = 0; i < NUM_PLAYER_SURFACES; i++) {
		n = sprintf(fn, "data/bmp/player%d_%d.bmp", a, b);
		playerSurface[i] = FillSurface(fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
		
		b++;
		if (b > (NUM_PLAYER_SURFACES / 3) - 1) {
			b = 0;
			a++;
		}
	}

	for (uint i = 0; i < NUM_TORCH_FLAMES; i++) {
		n = sprintf(fn, "data/bmp/torch%d.bmp", i);
		torchSurface[i] = FillSurface(fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}

	for (uint i = 0; i < NUM_TELEPAD_STATES; i++) {
		n = sprintf(fn, "data/bmp/telepad%d.bmp", i);
		telepadSurface[i] = FillSurface(fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}

	bgSurface = FillSurface("data/bmp/bg0.bmp", 0);
	bgW = bgSurface->w;
	bgH = bgSurface->h;

	
	/*** Keyboard ***/
	//if (SDL_EnableKeyRepeat(200, 10) != 0) {
	//	fprintf(stderr, "SDL_EnableKeyRepeat failed: %s", SDL_GetError());
	//}
	
	
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
