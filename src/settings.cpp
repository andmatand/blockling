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


// Reads the next line from the file, ignoring # comments or lines which exceed
// maxLineLength
char* ReadLine(FILE *file, uint maxLineLength) {
	maxLineLength += 1; // Make room for newline character on the end
	char *line = new char[maxLineLength]; // For holding the current line
	bool lineHasBreak;

	while (!feof(file)) {
		fgets(line, maxLineLength, file);
		if (strlen(line) < 1) continue;

		lineHasBreak = false;

		// Remove the trailing newline character(s), LF or CR
		while (line[strlen(line) - 1] == '\n' ||
			line[strlen(line) - 1] == 13)
		{
			lineHasBreak = true;
			line[strlen(line) - 1] = '\0';
		}

		// Remove any # comment from the end of the line
		char *commentPos = strchr(line, '#');
		if (commentPos) {
			if (line[commentPos - line] == '#') {
				line[commentPos - line] = '\0';
			}
		}

		// If there is no linebreak on this "line" (i.e. the line is
		// too long and thus invalid) or this line is a comment.
		if (lineHasBreak == false || line[0] == '#') {
			// Skip over the rest of this line until we get to the
			// end of it
			while (!feof(file)) {
				fgets(line, maxLineLength, file);
				if (strchr(line, '\n')) {
					break;
				}
			}

			// Back to top of while-loop to get next line
			continue;
		}

		return line;
	}

	return NULL;
}


void SaveSettings() {
	char filename[256];
	sprintf(filename, "%s%s", SETTINGS_PATH, SETTINGS_FILE);

	#ifdef DEBUG
	printf("\nWriting to settings file \"%s\"...\n", filename);
	#endif

	FILE *f = fopen(filename, "wt");
	
	if (f == NULL) {
		fprintf(stderr, "Error: Could not write settings to \"%s\"\n",
			filename);

		return;
	}
	
	char line[32];
	char name[12];
	char value[sizeof(option_tileset) + 1];
	
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
			case 10:
				strcpy(name, "tileset");
				break;
			case 11:
				strcpy(name, "helpSpeech");
				break;
			case 12:
				strcpy(name, "levelMax0");
				break;
			case 13:
				strcpy(name, "levelMax1");
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
			case 10:
				sprintf(value, "%s", option_tileset);
				break;
			case 11:
				sprintf(value, "%u", option_helpSpeech);
				break;
			case 12:
				sprintf(value, "%u", option_levelMax0);
				break;
			case 13:
				sprintf(value, "%u", option_levelMax1);
				break;
		}
		
		// Construct the line
		sprintf(line, "%s=%s\n", name, value);
		
		// Write the line to the file
		fputs(line, f);
	}
	
	// Write the game controls
	for (uint i = 0; i < NUM_GAME_KEYS; i++) {
		sprintf(line, "gameKeySym%d=%d\ngameKeyMod%d=%d\n", i,
			gameKeys[i].sym, i, gameKeys[i].mod);
		
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

	FILE *f = fopen(filename, "rt");
	
	if (f == NULL) {
		fprintf(stdout, "Notice: Could not open settings file \"%s\"; "
			"Using defaults...\n", filename);

		return;
	}
	
	char *line = NULL;
	char name[13];
	char value[sizeof(option_tileset) + 1]; // String form of the value
	uint uintVal = 0;  // uint form of the value
	uint midpoint; // Position of '=' in the string
	uchar n;       // Holds string position when filling string, and parsed
	               // key number (e.g. 3 at end of "gameKeySym3")
	char c[2];     // holds one character
	
	while ((line = ReadLine(f, 16 + sizeof(option_tileset))) != NULL) {
		// If this line doesn't contain an equals sign, skip it
		if (strchr(line, '=') == NULL)
			continue;

		// Find where the "=" is
		midpoint = static_cast<uint>(strchr(line, '=') - line);
		
		#ifdef DEBUG
			printf("\n\nline: \"%s\"\nposition of = is %d\n", line,
				midpoint);
		#endif

		/** Get the part of the string before the "=" ****/
		n = 0; // Position in the "name" string
		for (uint i = 0; i < midpoint; i++) {
			// If this character is valid (alphanumeric)
			if ((line[i] >= 48 && line[i] <= 57) ||     // numb3r5
				(line[i] >= 65 && line[i] <= 90) || // CAPITAL
				(line[i] >= 97 && line[i] <= 122))  // lowercase
			{
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
			// If this character is valid (alphanumeric)
			if ((line[i] >= 48 && line[i] <= 57) ||     // numb3r5
				(line[i] >= 65 && line[i] <= 90) || // CAPITAL
				(line[i] >= 97 && line[i] <= 122))  // lowercase
			{
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
			printf(" name = \"%s\"\n value = \"%s\"\n intVal = "
				"%d\n", name, value, uintVal);
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
		else if (strcmp(name, "tileset") == 0) {
			strncpy(option_tileset, value, sizeof(option_tileset));
		}
		else if (strcmp(name, "helpSpeech") == 0) {
			option_helpSpeech = static_cast<bool>(uintVal);
		}
		else if (strcmp(name, "levelMax0") == 0) {
			option_levelMax0 = static_cast<uchar>(uintVal);
		}
		else if (strcmp(name, "levelMax1") == 0) {
			option_levelMax1 = static_cast<uchar>(uintVal);
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
				option_playerKeys[n].sym =
					static_cast<SDLKey>(uintVal);
			}
		}

		// Free heap memory
		delete [] line;
		line = NULL;
	}

	fclose(f);
}
