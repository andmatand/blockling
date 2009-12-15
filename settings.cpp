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


void SaveSettings() {
	char filename[256];
	sprintf(filename, "%s%s", SETTINGS_PATH, SETTINGS_FILE);

	#ifdef DEBUG
	printf("\nWriting to settings file \"%s\"...\n", filename);
	#endif

	FILE *f = fopen(filename, "w");
	
	if (f == NULL) {
		fprintf(stderr, "\nFile error: Could not write settings to \"%s\"\n", filename);
		return;
	}
	
	char line[17];
	char name[12];
	char value[4];
	
	// Write the options
	for (uchar i = 0; i < NUM_OPTIONS; i++) {
		// Find the name of this option
		switch (i) {
			case 0:
				strcpy(name, "undoSize");
				break;
			case 1:
				strcpy(name, "soundOn");
				break;
			case 2:
				strcpy(name, "musicOn");
				break;
			case 3:
				strcpy(name, "levelSet");
				break;
			case 4:
				strcpy(name, "replayOn");
				break;
			case 5:
				strcpy(name, "replaySpeed");
				break;
			case 6:
				strcpy(name, "background");
				break;
			case 7:
				strcpy(name, "timerOn");
				break;
			case 8:
				strcpy(name, "cameraMode");
				break;
			case 9:
				strcpy(name, "fullscreen");
				break;
		}		

		// Get the string form of this option's value
		switch (i) {
			case 0:
				sprintf(value, "%u", option_undoSize);
				break;
			case 1:
				sprintf(value, "%u", option_soundOn);
				break;
			case 2:
				sprintf(value, "%u", option_musicOn);
				break;
			case 3:
				sprintf(value, "%u", option_levelSet);
				break;
			case 4:
				sprintf(value, "%u", option_replayOn);
				break;
			case 5:
				sprintf(value, "%u", option_replaySpeed);
				break;
			case 6:
				sprintf(value, "%u", option_background);
				break;
			case 7:
				sprintf(value, "%u", option_timerOn);
				break;
			case 8:
				sprintf(value, "%u", option_cameraMode);
				break;
			case 9:
				sprintf(value, "%u", option_fullscreen);
				break;
		}
		
		// Construct the line
		sprintf(line, "%s=%s\n", name, value);
		
		// Write the line to the file
		fputs(line, f);
	}
	
	// Write the game controls
	for (uint i = 0; i < NUM_GAME_KEYS; i++) {
		sprintf(line, "gameKeySym%d=%d\ngameKeyMod%d=%d\n", i, gameKeys[i].sym, i, gameKeys[i].mod);
		
		fputs(line, f);
	}

	// Write the player controls
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
		sprintf(line, "playerKey%d=%d\n", i, option_playerKeys[i].sym);
		
		fputs(line, f);
	}
	
	// Write the current level
	sprintf(line, "currentLevel=%d\n", currentLevel);
	fputs(line, f);
	
	
	fclose(f);
}



void LoadSettings() {
	char filename[256];
	sprintf(filename, "%s%s", SETTINGS_PATH, SETTINGS_FILE);

	#ifdef DEBUG
	printf("\nLoading settings from file \"%s\"...\n", filename);
	#endif

	FILE *f = fopen(filename, "r");
	
	if (f == NULL) {
		fprintf(stderr, "\nFile error: Could not open settings file \"%s\"\n", filename);
		return;
	}
	
	char line[32];
	char name[13];
	char value[4]; // string form of the value
	uint uintVal = 0;  // uint form of the value
	uint midpoint; // position of '=' in the string
	uchar n;       // holds string position when filling string, and parsed key number (e.g. 3 at end of "gameKeySym3")
	char c[2];     // holds one character
	
	while (!feof(f)) {
		fgets(line, sizeof(line), f);
		
		// If this line doesn't contain an equals sign, skip it
		if (strchr(line, '=') == NULL)
			continue;
		
		// Remove the trailing newline character(s), LF or CR
		while (line[strlen(line) - 1] == '\n' or line[strlen(line) - 1] == 13) {
			line[strlen(line) - 1] = '\0';
		}
		
		// Find where the "=" is
		midpoint = static_cast<uint>(strchr(line, '=') - line);
		
		#ifdef DEBUG
			printf("\n\nline: \"%s\"\nposition of = is %d\n", line, midpoint);
		#endif

		/** Get the part of the string before the "=" ****/
		n = 0; // Position in the "name" string
		for (uint i = 0; i < midpoint; i++) {
			// If this character is not a space or a tab
			if (line[i] != 32 && line[i] != 9) {
				// Add this character to the end
				name[n] = line[i];
				
				// Limit string length
				if (++n == sizeof(name) - 1)
					break;
			}
		}
		// Add null terminator
		name[n] = '\0';
		
		
		/** Get the part after the "=" ****/
		n = 0; // position in the "value" string
		for (uint i = midpoint + 1; i < strlen(line); i++) {
			// If this character is not a space or a tab
			if (line[i] != 32 && line[i] != 9) {
				// Add this character to the end
				value[n] = line[i];
				
				// Limit string length
				if (++n == sizeof(value) - 1)
					break;
			}
		}
		// Add null terminator
		value[n] = '\0';
		
		// Store integer form of the value
		uintVal = static_cast<uint>(atoi(value));
		
		#ifdef DEBUG
			printf(" name = \"%s\"\n value = \"%s\"\n intVal = %d\n", name, value, uintVal);
		#endif
		
		// Set the correct option
		if (strcmp(name, "undoSize") == 0) {
			option_undoSize = uintVal;
		}
		else if (strcmp(name, "soundOn") == 0) {
			option_soundOn = static_cast<bool>(uintVal);
		}
		else if (strcmp(name, "musicOn") == 0) {
			option_musicOn = static_cast<bool>(uintVal);
		}
		else if (strcmp(name, "levelSet") == 0) {
			option_levelSet = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "replayOn") == 0) {
			option_replayOn = static_cast<bool>(uintVal);
		}
		else if (strcmp(name, "replaySpeed") == 0) {
			option_replaySpeed = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "background") == 0) {
			option_background = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "timerOn") == 0) {
			option_timerOn = static_cast<bool>(uintVal);
		}
		else if (strcmp(name, "cameraMode") == 0) {
			option_cameraMode = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "currentLevel") == 0) {
			currentLevel = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "fullscreen") == 0) {
			option_fullscreen = static_cast<bool>(uintVal);
		}
		else {
			// Get the number on the end of the setting name
			c[0] = name[strlen(name) - 1];
			c[1] = '\0';
			n = static_cast<uchar>(atoi(c));

			// Set the correct gameKeySym
			if (strncmp(name, "gameKeySym", 10) == 0) {
				gameKeys[n].sym = static_cast<SDLKey>(uintVal);
			}

			// Set the correct option_playerKey
			if (strncmp(name, "playerKey", 9) == 0) {
				option_playerKeys[n].sym = static_cast<SDLKey>(uintVal);
			}
		}

	}
	
	fclose(f);
}
