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
	
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);


	// Initialize SDL_mixer
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) < 0) {
    		printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
    		exit(1);
	}

	char musicFile[256];
	//sprintf(musicFile, "%s%s%s", DATA_PATH, MUSIC_PATH, "lizard_-_hip_chip.mod");
	sprintf(musicFile, "%s%s%s", DATA_PATH, MUSIC_PATH, "test.xm");
	bgMusic = Mix_LoadMUS(musicFile);
	if (bgMusic == NULL) {
		fprintf(stderr, "Unable to load audio file: %s\n", Mix_GetError());
	}

	// Fade-in the music
	if (Mix_FadeInMusic(bgMusic, -1, 500) == -1) {
		fprintf(stderr, "Unable to play music file: %s\n", Mix_GetError());
	}


	// Initialize Video
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
				if (Game() == -2) {
					quitProgram = true;
				}
				break;
			case 1: // Options
				switch(OptionsMenu(false)) {
					case -2: // Close window
						quitProgram = true;
						break;
				}
				break;
			case -1: // Esc
			case -2: // Close Window
			case 2: // Quit
				quitProgram = true;
				break;
		}
		if (quitProgram) break;	
	}
	
	
	DeInit();

	// Wait for sound to finish playing
	//while(Mix_Playing() != 0) {}
 


	return 0;
}







void Init() {
	
	/*** Graphics ***/
	LoadTileset("default");
	LoadFont("font.bmp");
	
	
	/*** Sound ***/
	LoadSound("pickup.wav", 0);
	LoadSound("setdown.wav", 1);
	LoadSound("push.wav", 2);
	LoadSound("teleport.wav", 3);
	LoadSound("undo.wav", 4);
	LoadSound("menu_sel.wav", 5);
	LoadSound("menu_enter.wav", 6);
	LoadSound("menu_back.wav", 7);
	LoadSound("win.wav", 8);
	
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

	ResetDefaultKeys();
}




void DeInit() {
	/*** Clean up surfaces ***/
	UnloadTileset();
	UnloadFont();
	SDL_FreeSurface(screenSurface);
	


	/*** Clean up SDL_mixer **/
	for (int i = 0; i < NUM_SOUNDS; i++) {
		Mix_FreeChunk(sounds[i]);
	}
	
	Mix_FreeMusic(bgMusic);
 	Mix_CloseAudio();
}
