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


void LoadSound(char *file, int n) {
	char fullPath[256];
	sprintf(fullPath, "%s%s%s", DATA_PATH, SOUND_PATH, file);
	
	sounds[n] = Mix_LoadWAV(fullPath);
	if (sounds[n] == NULL) {
		fprintf(stderr, "Unable to load audio file: %s\n", Mix_GetError());
	}

	Mix_VolumeChunk(sounds[n], MIX_MAX_VOLUME / 8);
}

void LoadSound(const char *file, int n) {
	char *fn = new char[strlen(file) + 1];
	strcpy(fn, file);
	
	LoadSound(fn, n);
	
	delete [] fn;
}



void PlaySound(int n) {
	if (option_soundOn == false) return;
	
	int channel;
	channel = Mix_PlayChannel(-1, sounds[n], 0);
	if(channel == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}

}


void ToggleSound() {
	if (option_soundOn == true) {
		option_soundOn = false;
		Mix_Volume(-1, 0);
	}
	else {
		option_soundOn = true;
		Mix_Volume(-1, 128);
	}
}



void ToggleMusic() {
	if (option_musicOn == true) {
		option_musicOn = false;
		Mix_PauseMusic();
	}
	else {
		option_musicOn = true;
		Mix_ResumeMusic();
	}
}
