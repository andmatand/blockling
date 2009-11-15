/*
 *   settings.cpp
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
	
	char line[64];
	char *name = NULL;
	char *val = NULL;
	for (uchar i = 0; i < NUM_OPTIONS; i++) {
		name = OptionName(i);
		val = OptionValue(i);
		sprintf(line, "%s=%s\n", name, val);
		
		delete [] name; name = NULL;
		delete [] val; val = NULL;
		
		fputs(line, f);
	}
	
	fclose(f);
}


char * OptionName(uchar n) {
	char *name = new char[16];
	
	switch (n) {
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
	}
	
	return name;
}

char * OptionValue(uchar n) {
	char *val = new char[6];
	
	switch (n) {
		case 0:
			sprintf(val, "%u", option_undoSize);
			break;
		case 1:
			sprintf(val, "%u", option_soundOn);
			break;
		case 2:
			sprintf(val, "%u", option_musicOn);
			break;
		case 3:
			sprintf(val, "%u", option_levelSet);
			break;
		case 4:
			sprintf(val, "%u", option_replayOn);
			break;
		case 5:
			sprintf(val, "%u", option_replaySpeed);
			break;
		case 6:
			sprintf(val, "%u", option_background);
			break;
		case 7:
			sprintf(val, "%u", option_timerOn);
			break;
		case 8:
			sprintf(val, "%u", option_cameraMode);
			break;
	}
	
	return val;
}
