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



char Init() {
	/** Set default game keyboard layout ****/
	ResetDefaultKeys();
	
	/** Set the default settings ****/
	option_soundOn = true;
	option_musicOn = true;
	option_undoSize = 300;
	option_levelSet = 0;
	option_replayOn = true;
	option_replaySpeed = 3;
	option_background = 3;
	option_timerOn = true;
	option_cameraMode = 0;
	strcpy(option_tileset, DEFAULT_TILESET);
	currentLevel = 0;
	option_helpSpeech = true;
	option_levelMax0 = 0;
	option_levelMax1 = 0;

	/** Load any settings found in the settings file ***/
	LoadSettings();
	

	/** Initialize SDL ****/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
		fprintf(
			stderr,
			"Error: Unable to initialize SDL: %s\n",
			SDL_GetError());

		return 1;	
	}

        /** Initialize the joystick ****/
        if (SDL_NumJoysticks() > 0) {
            // Disable joystick events, since we will poll manually
            SDL_JoystickEventState(SDL_DISABLE);

            // Open the first joystick
            joystick = SDL_JoystickOpen(0);
        }

	/** Initialize SDL_mixer ****/
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) < 0) {
    		fprintf(
			stderr,
			"Error: Unable to initialize SDL_mixer: %s\n",
			Mix_GetError());
	}

/* The music part is commented out right now because I don't have any music yet =)

   	char musicFile[256];
	//sprintf(musicFile, "%s%s%s", DATA_PATH, MUSIC_PATH, "lizard_-_hip_chip.mod");
	sprintf(musicFile, "%s%s%s", DATA_PATH, MUSIC_PATH, "test.xm");
	bgMusic = Mix_LoadMUS(musicFile);
	if (bgMusic == NULL) {
		fprintf(
			stderr,
			"Error: Unable to load audio file: %s\n",
			Mix_GetError());
	}

	// Fade-in the music
	if (Mix_FadeInMusic(bgMusic, -1, 500) == -1) {
		fprintf(
			stderr,
			"Error: Unable to play music file: %s\n",
			Mix_GetError());
	}
*/


	/** Initialize SDL Video ****/
	screenSurface = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP,
                                         SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
	if (screenSurface == NULL) {
		fprintf(stderr, "Error: Unable to set up video: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_WM_SetCaption(GAME_TITLE, NULL);
	SDL_ShowCursor(SDL_DISABLE);

	
	/*** Load Graphics ***/
	if (LoadTileset(option_tileset) != 0 || LoadFont("font.bmp") != 0) {
		return 1;
	}

	// Make camera start in default position for a level that fits onscreen
	levelX = levelY = levelW = levelH = 0;
	CenterCamera(-1);

	
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
	// Free speech bubble-related memory
	delete [] triggers;

	/** Free notification string in case it hasn't disappeared yet **/
	delete [] notifyText;
	notifyText = NULL;
	
	/** Clean up SDL surfaces ****/
	UnloadTileset();
	UnloadFont();
	SDL_FreeSurface(screenSurface);

        /** Clean up the joystick ****/
        SDL_JoystickClose(0);
        joystick = NULL;

	/** Clean up SDL_mixer ****/
	for (uchar i = 0; i < NUM_SOUNDS; i++) {
		Mix_FreeChunk(sounds[i]);
	}
	
	// Wait for sound to finish playing
	//while(Mix_Playing() != 0) {}

	//Mix_FreeMusic(bgMusic);
 	Mix_CloseAudio();

	/** Shut down SDL ****/
	SDL_Quit();
}
