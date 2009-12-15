/*
 *   Copyright 2009 Andrew Anderson <www.billamonster.com>
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
	if (Init() != 0) return 1;

	bool quitProgram = false;
	while (quitProgram == false) {
		switch (MainMenu()) {
			case 0: // Start Game
				if (Game() == -2) {
					// Close window
					quitProgram = true;
				}
				break;
			case 1: // Options
				if (OptionsMenu(false) == -2) {
					// Close window
					quitProgram = true;
				}
				break;
			case 2: // Credits
				if (Credits() == -2) {
					// Close window
					quitProgram = true;
				}
				break;
			case -1: // Esc
			case -2: // Close Window
			case 3: // Quit
				quitProgram = true;
				break;
		}
	}
	
	SaveSettings();
	DeInit();

	return 0;
}







int Init() {
	/** Set default game keyboard layout ****/
	ResetDefaultKeys();
	
	/** Set the default settings ****/
	option_soundOn = true;
	option_musicOn = true;
	option_undoSize = 300;
	option_levelSet = 0;
	option_replayOn = true;
	option_replaySpeed = 2;
	option_background = 2;
	option_timerOn = true;
	option_cameraMode = 0;
	option_fullscreen = false;
	currentLevel = 0;

	/** Load any settings found in the settings file ***/
	LoadSettings();
	

	/** Initialize SDL ****/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/** Initialize SDL_mixer ****/
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) < 0) {
    		printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
    		exit(1);
	}

/*
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
*/


	/** Initialize SDL Video ****/
	screenSurface = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_HWSURFACE | (option_fullscreen ? SDL_FULLSCREEN : 0));
	if (screenSurface == NULL) {
		fprintf(stderr, "Unable to set up video: %s\n", SDL_GetError());
		exit(1);
	}
	
	SDL_WM_SetCaption(GAME_TITLE, NULL);
	SDL_ShowCursor(SDL_DISABLE);

	

	/*** Load Graphics ***/
	LoadFont("font.bmp");
	LoadTileset(DEFAULT_TILESET);

	
	/*** Load Sound ***/
	LoadSound("pickup.wav", 0);
	LoadSound("setdown.wav", 1);
	LoadSound("push.wav", 2);
	LoadSound("teleport.wav", 3);
	LoadSound("undo.wav", 4);
	LoadSound("menu_sel.wav", 5);
	LoadSound("menu_enter.wav", 6);
	LoadSound("menu_back.wav", 7);
	LoadSound("win.wav", 8);
	
	return 0;
}




void DeInit() {
	/** Free speech bubble memory ****/
	delete [] bubbles;
	
	/** Clean up SDL surfaces ****/
	UnloadTileset();
	UnloadFont();
	SDL_FreeSurface(screenSurface);

	/** Clean up SDL_mixer ****/
	for (int i = 0; i < NUM_SOUNDS; i++) {
		Mix_FreeChunk(sounds[i]);
	}
	
	// Wait for sound to finish playing
	//while(Mix_Playing() != 0) {}

	//Mix_FreeMusic(bgMusic);
 	Mix_CloseAudio();


	/** Shut down SDL ****/
	SDL_Quit();
}
