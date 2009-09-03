/*
 *   menu.cpp
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

#include "menus.h"


int ReplaySelectMenu(uint levelNum, uint levelReplays, replay * replays, uint numReplays) {
	uint i;
	
	/*** Make the Level's menu ***/
	menu menu2(levelReplays + 1);
	
	// Set the menu title
	char tempString[10];
	sprintf(tempString, "LEVEL %d", levelNum);
	menu2.SetTitle(tempString);

	// Menu item number
	uint mi = 0;
	
	// For holding menu items' names, e.g. "I'm Stuck" (4294967296 moves)
	char tempMenuString[2 + MAX_REPLAY_TITLE_LENGTH + 19 + 1]; 

	// For remembering which replay each menuItem is referring to
	uint menuItemReplay[levelReplays];
	
	// List each level and its total number of replays
	for (i = 0; i < numReplays; i++) {
		// If this replay is for the current level
		if (replays[i].GetLevel() == levelNum) {
			// Create the menu item name
			sprintf(tempMenuString, "\"%s\" (%d ", replays[i].GetTitle(), levelReplays);
			strcat(tempMenuString, (replays[i].GetMoves() > 1) ? "moves)" : "move)");
			
			// Remember which replay this menuItem is referring to
			menuItemReplay[mi] = i;
			
			// Add it as a menu item
			menu2.NameItem(mi++, tempMenuString);
		}
	}

	menu2.NameItem(mi, "Cancel");
	menu2.Move(75, 100);
	menu2.AutoArrange(0);

	/*** Input loop ***/
	int action;
	uint whichReplay;
	while (true) {
		action = menu2.Input();

		// If Esc or Close Window was pushed
		if (action == -1 || action == -2) {
			return action;
		}
		
		// If ENTER was pushed
		if (action == 1) {
			
			// If a replay was chosen
			if (menu2.GetSel() < static_cast<int>(levelReplays)) {
				whichReplay = menuItemReplay[menu2.GetSel()];
				
				// Start the game engine, passing the filename of the replay to load
				Game (replays[whichReplay].GetFilename());
			}
			// "Cancel" was chosen
			else {
				return -1;
			}
		}
				
		DrawBackground();
		menu2.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}
}





int LoadReplayMenu() {
	/* Menu design:
	
	// Only show levels which have existing replays
	
	LOAD REPLAY
	 Level 0 (2 replays)
	 Level 1 (1 replay)
	 Level 2 (1 replay)
	 Back
	
	// If the user chooses Level 0, this menu will be displayed.
	// Each replay's title is displayed, along with the number of moves it contains

	LEVEL 0 REPLAYS
	 "Test Replay" (23 moves)
	 "speed run!" (12 moves)
	 Back
	*/
	

	
	// Scan directory to count all replays
	uint numReplays = 0;
	FILE * fp;
	const char path[] = "data/replays/";
	char filename[strlen(path) + 8];
	uint levelNum = 0, fileNum = 0;
	while (levelNum < 1000) {
		filename[0] = '\0';
		// Get filename, e.g. "data/replays/001-000"
		sprintf(filename, "%s%03d-%03d", path, levelNum, fileNum);
		
		//printf("Trying to open %s...\n", filename);
		
		fp = fopen(filename, "r");
		
		// If the file was able to be opened
		if (fp != NULL) {
			// Close the file
			fclose(fp);

			// Increment the total
			numReplays++;
		}
		else {
			// Move on to the next level
			levelNum++;
			// And start there at level 0
			fileNum = 0;
		}
		
		fileNum++;
		
		if (fileNum > 999) {
			// Move on to the next level
			levelNum++;
			// And start there at level 0
			fileNum = 0;
		}
	}
	
	printf("Number of replays found: %d\n", numReplays);
	
	// Create an array of replays, now that the number is known ^
	replay *replays;
	//strcpy(filename, "000-000"); // Fake filename to get the string the right size
	replays = new replay[numReplays]; // Array to hold meta-data of all replays
	
	levelNum = 0;
	fileNum = 0;
	
	// Scan directory again, but this time to get meta data
	uint i = 0; //  Index number in the replays array
	while (levelNum < 1000) {
		filename[0] = '\0';
		// Get filename, e.g. "data/replays/001-000"
		sprintf(filename, "%s%03d-%03d", path, levelNum, fileNum);
		
		printf("Trying to open %s...\n", filename);
		
		fp = fopen(filename, "r");
		
		// If the file was able to be opened
		if (fp != NULL) {
			// Close the file
			fclose(fp);

			replays[i].SetFilename(filename);
			
			// This will read the meta-data header and set
			// replay's title, level, and date members.
			replays[i].InitRead();
			
			// Clean up now that we have what we need.
			replays[i].DeInitRead();
			
			// Increment the array index
			i++;
			
			// Save time & avoid out-of-bounds array access by
			// stopping the scan if we already know there
			// are no more replay files.
			if (i == numReplays) break;
		}
		else {
			// Move on to the next level
			levelNum++;
			// And start there at level 0
			fileNum = 0;
		}
		
		fileNum++;
		
		if (fileNum > 999) {
			// Move on to the next level
			levelNum++;
			// And start there at level 0
			fileNum = 0;
		}
	}


	/*** Get the each level's total number of replays ***/
	uint maxLevels = 1000;
	uint levelTotal[maxLevels];
	// Zero the array
	for (i = 0; i < maxLevels; i++) {
		levelTotal[i] = 0;
	}
	
	// Get the total replays for each level
	for (i = 0; i < numReplays; i++) {
		levelTotal[replays[i].GetLevel()] ++;
	}
	
	// Get the number of levels with at least one replay
	uint numLevels = 0;
	for (i = 0; i < maxLevels; i++) {
		if (levelTotal[i] > 0) numLevels++;
	}
		



	/*** Make the first menu ***/
	int numItems = numLevels + 1; // One for each level that has a replay + "Back"
	menu replayMenu(numItems);
	replayMenu.SetTitle("LOAD A REPLAY");

	// For remembering which level each menuItem is referring to
	uint menuItemLevel[numItems];
	
	char tempString[] = "Level 999 (999 replays)"; // String that will hold the menu item's name
	uint mi = 0; // Menu item number

	// List each level and its total number of replays
	for (i = 0; i < numLevels; i++) {
		// If this level has at least 1 replay
		if (levelTotal[i] > 0) {
			// Create the menu item name
			sprintf(tempString, "Level %d (%d ", i, levelTotal[i]);
			strcat(tempString, (levelTotal[i] > 1) ? "replays)" : "replay)");
			
			// Keep track of which level this menuItem is referring to.
			menuItemLevel[mi] = i;
			
			// Add the menu item for this level
			replayMenu.NameItem(mi++, tempString);
		}
	}

	replayMenu.NameItem(mi, "Cancel");
	
	replayMenu.Move(75, 100);
	replayMenu.AutoArrange(0);
	
	int action;
	int returnVal = -1;
	while (true) {
		action = replayMenu.Input();
		
		// If Esc or Close Window was pushed
		if (action == -1 || action == -2) {
			returnVal = action;
			break;
		}
		
		// If enter was pushed
		if (action == 1) {
			// If a level was chosen
			if (replayMenu.GetSel() < static_cast<int>(numLevels)) {
				// Get the level number
				levelNum = menuItemLevel[replayMenu.GetSel()];
				
				printf("Level %d was chosen\n", levelNum);
				
				// Show the menu for this level's replays
				ReplaySelectMenu(levelNum, levelTotal[levelNum], replays, numReplays);
			}
			// "Back" was chosen
			else {
				returnVal = -1;
				break;
			}
		}
		
		DrawBackground();
		replayMenu.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}
	
	
	// Collect garbage before returning
	delete [] replays;
	replays = NULL;
	
	return returnVal;
}





int ReplayPauseMenu(bool finished) {
	int numItems = 4;
	
	menu pauseMenu(numItems);
	
	if (finished) {
		pauseMenu.SetTitle("REPLAY FINISHED");
	}
	else {
		pauseMenu.SetTitle("REPLAY PAUSED");
	}
	if (finished) {
		pauseMenu.NameItem(0, "Replay");
	}
	else {
		pauseMenu.NameItem(0, "Resume");
	}
	pauseMenu.NameItem(1, "Take Control From Here");
	pauseMenu.NameItem(2, "Help");
	pauseMenu.NameItem(3, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	
	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return pauseMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0);
		pauseMenu.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}

}





int MainMenu() {
	int numItems = 4;
	menu mainMenu(numItems);
	
	mainMenu.SetTitle("BLOCKMAN");
	mainMenu.NameItem(0, "Play");
	mainMenu.NameItem(1, "Load Replay");
	mainMenu.NameItem(2, "Options");
	mainMenu.NameItem(3, "Quit");
	
	mainMenu.Move(75, 100);
	mainMenu.AutoArrange(0);
	
	int action;
	
	while (true) {
		action = mainMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return mainMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		
		DrawBackground();
		mainMenu.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}
}



int PauseMenu() {
	int numItems = 3;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Help");
	pauseMenu.NameItem(2, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	
	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return pauseMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0);
		pauseMenu.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}
}
