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


int ControlSetupMenu(bool inGame) {
	int numItems = NUM_PLAYER_KEYS + 4 + 2;
	menu csMenu(numItems); // Create the menu object
	char text[256]; // For temporarily holding menu items' text as it is formed
	char tempString[32];
	
	int i;
	int action;
	bool gettingKey = false;
	
	/** Set static menu items **/
	csMenu.Move(inGame ? SCREEN_W / 2 : 75, FONT_H * 5);
	csMenu.SetTitle("CONTROL SETUP");
	csMenu.NameItem(numItems - 2, "Reset To Defaults");
	csMenu.NameItem(numItems - 1, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/
		for (i = 0; i < numItems - 2; i++) {
			// Determine text
			switch (i) {
				case 0:
					sprintf(tempString, "Move Left:\t");
					break;
				case 1:
					sprintf(tempString, "Move Right:\t");
					break;
				case 2:
					sprintf(tempString, "Pick Up Block:");
					break;
				case 3:
					sprintf(tempString, "Drop Block:\t");
					break;
				case 4:
					sprintf(tempString, "Push Block:\t");
					break;
				case 5:
					sprintf(tempString, "Move Camera Left:");
					break;
				case 6:
					sprintf(tempString, "Move Camera Right:");
					break;
				case 7:
					sprintf(tempString, "Move Camera Up:\t");
					break;
				case 8:
					sprintf(tempString, "Move Camera Down:");
					break;
			}
			sprintf(text, "%s\t%s",
				tempString,
				(gettingKey && csMenu.GetSel() == static_cast<int>(i)) ?
					"Press a key..." :
					(i < static_cast<int>(NUM_PLAYER_KEYS) ? KeyName(option_playerKeys[i].sym) : KeyName(gameKeys[i - 5].sym))
				);

			// Set it
			csMenu.NameItem(i, text);
		}


		/** Render *****************/
		do {
			if (inGame) {
				Render(3);
			}
			else {
				DrawBackground();
			}
			csMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
			
			// Cheatingly position the items at a fixed x, if using the inGame text centering
			if (inGame) {
				for (i = 0; i < numItems - 2; i++) {
					csMenu.MoveItem(i, (SCREEN_W / 2) - (FONT_W * 10), csMenu.GetItemY(i));
				}
			}
			csMenu.SpaceItems(numItems - 2);
			csMenu.Display();
			UpdateScreen();
			
			if (gettingKey) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym != SDLK_ESCAPE) {
							if (csMenu.GetSel() < static_cast<int>(NUM_PLAYER_KEYS)) {
								option_playerKeys[csMenu.GetSel()].sym = event.key.keysym.sym;
							}
							else {
								gameKeys[csMenu.GetSel() - 5].sym = event.key.keysym.sym;
							}
						}
						gettingKey = false;
						break;
					}
				}
			}
		} while (gettingKey);

		
		
		/** Input ******************/
		action = csMenu.Input();
		
		switch (action) {
			case 1: // Enter
				// Reset to defaults
				if (csMenu.GetSel() == numItems - 2) {
					ResetDefaultKeys();
					break;
				}
				
				// Done
				if (csMenu.GetSel() == numItems - 1) return -1;

				// Change a key
				gettingKey = true;
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
	}
}



int MainMenu() {
	int numItems = 3;
	menu mainMenu(numItems);
	
	mainMenu.SetTitle(GAME_TITLE);
	mainMenu.NameItem(0, "Play");
	mainMenu.NameItem(1, "Options");
	mainMenu.NameItem(2, "Quit");
	
	mainMenu.Move(75, FONT_H * 7);
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
		UpdateScreen();
	}
}



int OptionsMenu(bool inGame) {
	int numItems = 8;
	menu optMenu(numItems); // Create the menu object
	char text[70]; // For temporarily holding menu items' text as it is formed
	
	uint maxUndoSize = 500;
	char change_undoSize;
	
	char maxBackground = 2;
	
	char maxCameraMode = 1;
	
	int action;
	
	/** Set static menu items **/
	optMenu.Move(inGame ? SCREEN_W / 2 : 75, FONT_H * 6);
	optMenu.SetTitle("OPTIONS");
	optMenu.NameItem(6, "Control Setup");
	optMenu.NameItem(7, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/

		// Determine Sound text
		sprintf(text, "Sound: ");
		strcat(text, (option_soundOn ? "ON" : "OFF"));
		optMenu.NameItem(0, text);

		optMenu.SetLeftArrow(0,	(option_soundOn > 0));
		optMenu.SetRightArrow(0, (option_soundOn < 1));

		
		// Determine Music text
		sprintf(text, "Music: ");
		strcat(text, (option_musicOn ? "ON" : "OFF"));
		optMenu.NameItem(1, text);

		optMenu.SetLeftArrow(1,	(option_musicOn > 0));
		optMenu.SetRightArrow(1, (option_musicOn < 1));

		// Determine Timer on/off text
		sprintf(text, "Game Timer: ");
		strcat(text, (option_timerOn ? "ON" : "OFF"));
		optMenu.NameItem(2, text);

		optMenu.SetLeftArrow(2,	(option_timerOn > 0));
		optMenu.SetRightArrow(2, (option_timerOn < 1));


		// Determine Camera Setting text
		sprintf(text, "Camera: ");
		switch (option_cameraMode) {
			case 0:
				strcat(text, "AUTO");
				break;
			case 1:
				strcat(text, "MANUAL");
				break;
		}
		optMenu.NameItem(3, text);

		optMenu.SetLeftArrow(3, (option_cameraMode > 0));
		optMenu.SetRightArrow(3, (option_cameraMode < maxCameraMode));


		// Determine Background text
		sprintf(text, "Background: ");
		switch (option_background) {
			case 0:
				strcat(text, "OFF");
				break;
			case 1:
				strcat(text, "STATIC");
				break;
			case 2:
				strcat(text, "SCROLLING");
				break;
		}
		optMenu.NameItem(4, text);

		optMenu.SetLeftArrow(4, (option_background > 0));
		optMenu.SetRightArrow(4, (option_background < maxBackground));


		// Determine Undo text
		sprintf(text, "Undo Memory: %d move", option_undoSize);
		if (option_undoSize != 1) strcat(text, "s");
		optMenu.NameItem(5, text);
		
		optMenu.SetLeftArrow(5,	(option_undoSize > 0));
		optMenu.SetRightArrow(5, (option_undoSize < maxUndoSize));
		

		
		/** Render *****************/
		if (inGame) {
			Render(3);
		}
		else {
			DrawBackground();
		}
		optMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
		optMenu.SpaceItems(6);
		optMenu.SpaceItems(7);
		optMenu.Display();
		// If the undo option is selected
		if (inGame && optMenu.GetSel() == 5) {
			sprintf(text, "Note: This setting will not take effect\nuntil a new level is loaded.");
			DrawText((SCREEN_W / 2) - (GetTextW(text, 0) / 2), FONT_H * 20, text, 0, 1);
		}
		UpdateScreen();
		
		
		/** Input ******************/
		action = optMenu.Input();
		change_undoSize = 0;
		
		switch (action) {
			case 1: // Enter
				switch (optMenu.GetSel()) {
					case 0:
						ToggleSound();
						break;
					case 1:
						option_musicOn = (option_musicOn ? false : true);
						break;
					case 2:
						option_timerOn = (option_timerOn ? false : true);
						break;
					case 3:
						if (option_cameraMode < maxCameraMode) {
							option_cameraMode++;
						}
						else {
							option_cameraMode = 0;
						}
						break;
					case 4:
						if (option_background < maxBackground) {
							option_background++;
						}
						else {
							option_background = 0;
						}
						break;
					case 5:
						change_undoSize = 1;
						if (option_undoSize == maxUndoSize) option_undoSize = 0;
						break;
					case 6:
						if (ControlSetupMenu(inGame) == -2) return -2;
						break;
					case 7:
						return -1;
						break;
				}
				break;
			case 3: // Left
				switch (optMenu.GetSel()) {
					case 0:
						if (option_soundOn) ToggleSound();
						break;
					case 1:
						option_musicOn = false;
						break;
					case 2:
						option_timerOn = false;
						break;
					case 3:
						if (option_cameraMode > 0) option_cameraMode--;
						break;
					case 4:
						if (option_background > 0) option_background--;
						break;
					case 5:
						change_undoSize = -1;						
						break;
				}
				break;
			case 4: // Right
				switch (optMenu.GetSel()) {
					case 0:
						if (!option_soundOn) {
							ToggleSound();
							PlaySound(5); // Play sound again here, since the sound in the menu selection was muted.
						}
						break;
					case 1:
						option_musicOn = true;
						break;
					case 2:
						option_timerOn = true;
						break;
					case 3:
						if (option_cameraMode < maxCameraMode) option_cameraMode++;
						break;
					case 4:
						if (option_background < maxBackground) option_background++;
						break;
					case 5:
						change_undoSize = 1;
						break;
				}
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}

		// Adjust undoSize according to which key was pushed,
		// handling irregular sequence of 0, 1, 50, 100, etc.
		if (change_undoSize == 1) { // Increase
			if (option_undoSize == 0) {
				option_undoSize = 1;
			}
			else if (option_undoSize == 1) {
				option_undoSize = 50;
			}
			else {
				option_undoSize += 50;
			}
		}
		if (change_undoSize == -1) { // Decrease
			if (option_undoSize == 50) {
				option_undoSize = 1;
			}
			else if (option_undoSize == 1) {
				option_undoSize = 0;
			}
			else if (option_undoSize > 50) {
				option_undoSize -= 50;
			}
		}
		
		// Limit undoSize
		if (option_undoSize > maxUndoSize) option_undoSize = maxUndoSize;
	}
}





int PauseMenu() {
	int numItems = 6;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Options");
	pauseMenu.NameItem(2, "Help");
	
	pauseMenu.NameItem(3, "Resart Level");
	pauseMenu.NameItem(4, "Change Level");
	pauseMenu.NameItem(5, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(3);

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
		
		Render(3);
		pauseMenu.Display();
		UpdateScreen();
	}
}




int EndOfLevelMenu() {
	int numItems = 2;
	
	menu theMenu(numItems);
	
	theMenu.SetTitle("NOW WHAT?");
	theMenu.NameItem(0, "Next Level");
	theMenu.NameItem(1, "View Replay");
	
	theMenu.Move(SCREEN_W / 2, 100);
	theMenu.AutoArrange(1);
	
	int action;
	
	while (true) {
		action = theMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return theMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0);
		theMenu.Display();
		UpdateScreen();
	}
}




int ReplayPauseMenu() {
	int numItems = 4;
	
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("REPLAY PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Take Control From Here");
	pauseMenu.NameItem(2, "Next Level");
	pauseMenu.NameItem(3, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(2);
	
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
		
		Render(3);
		pauseMenu.Display();
		UpdateScreen();
	}

}





int SelectLevelMenu() {
	uint i;
	int numItems = 2;
	menu lvlMenu(numItems); // Create the menu object
	char text[512]; // For temporarily holding menu items' text as it is formed
	int bottomY = SCREEN_H - FONT_H - 4;
	
	FILE *testFile;
	bool refreshLevel = true;
	uint numLevels = 999;
	char *levelError = NULL;
	
	int action;
	
	lvlMenu.Move(SCREEN_W / 2, 4);
	lvlMenu.SetTitle("");
	lvlMenu.SetSel(1); // Select the level number menu item by default

	while (true) {
		/** Load Level *****************/
		if (refreshLevel) {
			while (true) {
				// Load Level
				delete [] levelError;
				levelError = LoadLevel(currentLevel);
				
				// If the level could not be loaded
				sprintf(text, "!");
				if (levelError != NULL && strcmp(levelError, text) == 0) {
					numLevels = currentLevel;
					
					if (currentLevel <= 0) break;
					currentLevel--;
					continue;
				}
				
				
				testFile = OpenLevel(currentLevel + 1);
				if (testFile != NULL) {
					fclose(testFile);
				}
				else {
					numLevels = currentLevel + 1;
				}
				
				/*** Turn off all the status lights on the telepads ***/
				if (levelError == NULL) {
					for (i = 0; i < numTelepads; i++) {
						telepads[i].NeedsToTeleport();
					}
				}
				
				break;
			}
			
			refreshLevel = false;
		}


		/** Set dynamic menu items' text *****************/
		// Determine Leveset text
		sprintf(text, "Set: ");
		switch (option_levelSet) {
			case 0:
				strcat(text, GAME_TITLE);
				break;
			case 1:
				strcat(text, "BLOCK-MAN 1");
				break;
			case 2:
				strcat(text, "CUSTOM");
				break;
		}
		lvlMenu.NameItem(0, text);
		lvlMenu.SetLeftArrow(0,	(option_levelSet > 0));
		lvlMenu.SetRightArrow(0, (option_levelSet < NUM_LEVEL_SETS - 1));
		
		// Determine Level text
		sprintf(text, "Level %d", currentLevel);
		lvlMenu.NameItem(1, text);
		lvlMenu.SetLeftArrow(1,	(currentLevel > 0));
		lvlMenu.SetRightArrow(1, (currentLevel < numLevels - 1));
		
		
		/** Render *****************/
		if (levelError == NULL) {
			Render(3);
		}
		else {
			DrawBackground();
			sprintf(text, "Syntax errors exist in the level file:\n\n%s\nPlease fix the errors and try\nselecting the level again.", levelError);
			DrawText(FONT_W * 3, FONT_H * 3, text, 0, 1);
		}
		lvlMenu.AutoArrange(static_cast<char>(1));
		
		// Position the menu items to be stacked at the bottom of the screen
		lvlMenu.MoveItem(0, lvlMenu.GetItemX(0), bottomY - FONT_H - 2);
		lvlMenu.MoveItem(1, lvlMenu.GetItemX(1), bottomY);
		
		// Display the menu
		lvlMenu.Display();
		
		// Update the screen
		UpdateScreen();
		
		
		
		/** Input ******************/
		action = lvlMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (lvlMenu.GetSel()) {
					case 0:
					case 1:
						delete [] levelError;
						return lvlMenu.GetSel();
						break;
				}
				break;
			case 3: // Left
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet > 0) option_levelSet--;
						numLevels = 999;
						refreshLevel = true;
						break;
					case 1:
						if (currentLevel > 0) currentLevel--;
						refreshLevel = true;
						break;
				}
				break;
			case 4: // Right
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet < NUM_LEVEL_SETS - 1) option_levelSet++;
						numLevels = 999;
						refreshLevel = true;
						break;
					case 1:
						currentLevel++;
						refreshLevel = true;
						break;
				}
				break;
			case -1: // Esc
				CollectLevelGarbage();
				delete [] levelError;
				return -1;
				break;
			case -2: // Close window
				CollectLevelGarbage();
				delete [] levelError;
				return -2;
				break;
		}

		// Limit currentLevel
		if (currentLevel > numLevels) currentLevel = numLevels;
	}
}
