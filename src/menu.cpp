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


int ControlSetupMenu(bool inGame) {
	int numItems = NUM_PLAYER_KEYS + 1 + 2;
	menu csMenu(numItems); // Create the menu object
	char text[256]; // For temporarily holding menu items' text as it is formed
	char tempString[32];
	
	int i;
	int action;
	bool gettingKey = false;
	SDLKey keySym;
	
	/** Set static menu items **/
	csMenu.Move(SCREEN_W / 2, FONT_H);
	csMenu.SetTitle("CONTROL SETUP");
	csMenu.NameItem(numItems - 2, "Reset To Defaults");
	csMenu.NameItem(numItems - 1, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/
		for (i = 0; i < numItems - 2; i++) {
			// Determine text
			switch (i) {
				case 0:
					sprintf(tempString, "Move Left:\t ");
					break;
				case 1:
					sprintf(tempString, "Move Right:\t ");
					break;
				case 2:
					sprintf(tempString, "Pick Up Block: ");
					break;
				case 3:
					sprintf(tempString, "Drop Block:\t ");
					break;
				case 4:
					sprintf(tempString, "Push Block:\t ");
					break;
				case 5:
					sprintf(tempString, "Undo: ");
					break;
			}
			
			
			if (i < static_cast<int>(NUM_PLAYER_KEYS)) {
				keySym = option_playerKeys[i].sym;
			}
			else if (i == 5) {
				keySym = gameKeys[4].sym;
			}
			
			// Store this menu item's string in the text variable
			sprintf(
				text,
				"%s%s",
				tempString,
				(gettingKey && csMenu.GetSel() == static_cast<int>(i)) ?
					"Press a key..." :
					KeyName(keySym)
			);

			// Set it
			csMenu.NameItem(i, text);
		}


		/** Render *****************/
		do {
			if (inGame) {
				MoveCamera();
				Render(RENDER_BG);
			}
			else {
				DrawBackground();
			}
			csMenu.AutoArrange(static_cast<char>(1));
			
                        // Cheatingly position the items at a fixed x, even
                        // though we're supposed to be centering the text
                        for (i = 0; i < numItems - 2; i++) {
                                csMenu.MoveItem(i, (SCREEN_W / 2) - (FONT_W * 9), csMenu.GetItemY(i));
                        }
			//csMenu.SpaceItems(5);
			csMenu.SpaceItems(numItems - 3);
			csMenu.SpaceItems(numItems - 2);
			csMenu.Display();
			UpdateScreen();
			
			if (gettingKey) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
                                                // If the key is not START
						if (event.key.keysym.sym != SDLK_RETURN) {
							if (csMenu.GetSel() < static_cast<int>(NUM_PLAYER_KEYS)) {
								option_playerKeys[csMenu.GetSel()].sym = event.key.keysym.sym;
							}
							else {
                                                                // Change the UNDO key
								gameKeys[4].sym = event.key.keysym.sym;
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




int Credits() {
	int y = SCREEN_H; // Position of the scrolling credits
	uint t = SDL_GetTicks();
	txt credits(SCREEN_W / 2, 0, "BLOCKLING\n\n"
		"Copyright 2010 Andrew Anderson <www.unfinishedblog.org>\n\n"
		"This is free software, and you are welcome to change and " 
			"redistribute it under the conditions of the GNU "
			"General Public License (GPL) version 3 or later "
			"<http://gnu.org/licenses/gpl.html>.  There is NO "
			"WARRANTY, to the extent permitted by law.\n\n\n"
		"PROGRAMMING\n"
		"Andrew Anderson\n\n\n"

		"GRAPHICS\n"
		"Default & \"SPACE\"\n"
		"Andrew Anderson\n\n"
		"\"Sealife\"\n"
		"Enrico Schettler\n\n\n"

		"SOUND\n"
		"Andrew Anderson\n\n\n"

		"LEVEL DESIGN\n"
		"Andrew Anderson\n\n\n"

		"TESTING/FEEDBACK\n"
		"Tyler Christensen\n"
		"Aubree Dinsfriend\n"
		"Scott Jonathan Dinsfriend\n"
		"Matthew Galla\n"
		"Margaret Gowen\n"
		"Anna Hyldahl\n"
		"JeTSpice <jetspicegames.com>\n"
		"Aaron McGinn\n"
		"Peter Norland\n"
		"Jeanette Ortiz\n\n\n"

		"BASED ON AN ORIGINAL CONCEPT\n"
		"found in\n"
		"BLOCK-MAN 1\n"
		"Copyright 1993 Soleau Software <www.soleau.com>\n\n\n"

		"SPECIAL THANKS TO\n"
		"the teams behind the free/open-source software which made "
		"this game possible:\n"
		"Audacity <audacity.sourceforge.net>\n"
		"GIMP <www.gimp.org>\n"
		"GCC <gcc.gnu.org>\n"
		"SDL <www.libsdl.org>\n"
		"Ubuntu <www.ubuntu.com>\n"
		"Valgrind <www.valgrind.org>\n"
		"Vim <www.vim.org>");

	credits.Center();
	credits.FitToScreen(false);
	credits.Wrap(SCREEN_W - 2);

	while (y > -static_cast<int>(credits.GetH()) - (FONT_H / 2)) {
		
		if (y > SCREEN_H) y = SCREEN_H;
		
		DrawBackground();
		credits.SetY(y);
		credits.Render();
		UpdateScreen();

		switch (MenuInput()) {
			case 1: // Up
				y += FONT_H;
				t = SDL_GetTicks();
				break;
			case 2: // Down
				y -= FONT_H;
				t = SDL_GetTicks();
				break;
			case 3: // Left
				y += SCREEN_H;
				t = SDL_GetTicks();
				break;
			case 4: // Right
				y -= SCREEN_H;
				t = SDL_GetTicks();
				break;
			case 8: // Esc
				return -1;
				break;
			case 9: // Close window
				return -2;
				break;
		}

		if (SDL_GetTicks() >= t + 250) {
			t = SDL_GetTicks();
			y -= (FONT_H / 2);
		}
	}
	
	return 0;
}




int HelpMenu(bool inGame) {
	int numItems = 1;
	menu helpMenu(numItems); // Create the menu object
	
	int x2 = (SCREEN_W / 2) - (FONT_W * 9); // Position of body text
	
	int action;
	
	/** Set menu items **/
	helpMenu.Move(SCREEN_W / 2, 0);
	helpMenu.SetTitle("HELP");
	helpMenu.NameItem(0, "Done");
	helpMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));

	// Postion the "Done" button
	helpMenu.MoveItem(0, helpMenu.GetItemX(0), (FONT_H + 2) * 11); 


	/** Create help texts **/
        txt help1(x2, (FONT_H + 2) * 1, "The young BLOCKLING must pick "
                  "up blocks, stack them, and climb them to get to the exit!");
        help1.Wrap(static_cast<int>(SCREEN_W) - x2 - 2);

	txt help2(x2, (FONT_H + 2) * 7,
		"Press L/R to change tileset.  See OPTIONS for CONTROL SETUP.");
	help2.Wrap(static_cast<int>(SCREEN_W) - x2 - 2);

	while (true) {
		/** Render **/
		if (inGame) {
			MoveCamera();
			Render(RENDER_BG);
		}
		else {
			DrawBackground();
		}
		helpMenu.Display();

		/** Show the help text ****/
		help1.Render();

		help2.Render();
		
		UpdateScreen();
		
		
		/** Input **/
		action = helpMenu.Input();

		switch (action) {
			case 1: // Enter
				switch (helpMenu.GetSel()) {
					case 0:
						return -1;
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
	}
}




int MainMenu() {
	int numItems = 4;
	menu mainMenu(numItems);
	
	mainMenu.SetTitle(GAME_TITLE);
	mainMenu.NameItem(0, "Play");
	mainMenu.NameItem(1, "Options");
	mainMenu.NameItem(2, "Credits");
	mainMenu.NameItem(3, "Quit");
	
	mainMenu.Move(75, (SCREEN_H / 2) - FONT_H * 3);
	mainMenu.AutoArrange(0);
	mainMenu.SpaceItems(3);
	
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
	int numItems = 9;
	menu optMenu(numItems); // Create the menu object
	char text[72]; // For temporarily holding menu items' text as it is
	               // formed
	
	char maxBackground = 3;
	
	char maxCameraMode = 1;
	
	int action;
	
	/** Set static menu items **/
	optMenu.Move(SCREEN_W / 2, 0);
	optMenu.SetTitle("OPTIONS");
	optMenu.NameItem(7, "Control Setup");
	optMenu.NameItem(8, "Done");

	// Make the note text
	txt note(SCREEN_W / 2, FONT_H * 20,
		"(Note: This setting will not take effect\n"
		"until a new level is loaded.)");
	note.Center();
	note.Wrap(SCREEN_W * .9);

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

		// Determine Sound text
		sprintf(text, "Hints: ");
		strcat(text, (option_helpSpeech ? "ON" : "OFF"));
		optMenu.NameItem(3, text);

		optMenu.SetLeftArrow(3,	(option_helpSpeech > 0));
		optMenu.SetRightArrow(3, (option_helpSpeech < 1));

		// Determine Timer on/off text
		sprintf(text, "Replays: ");
		strcat(text, (option_replayOn ? "ON" : "OFF"));
		optMenu.NameItem(4, text);

		optMenu.SetLeftArrow(4,	(option_replayOn > 0));
		optMenu.SetRightArrow(4, (option_replayOn < 1));


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
		optMenu.NameItem(5, text);

		optMenu.SetLeftArrow(5, (option_cameraMode > 0));
		optMenu.SetRightArrow(5, (option_cameraMode < maxCameraMode));


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
				strcat(text, "PARALLAX");
				break;
			case 3:
				strcat(text, "SCROLLING");
				break;
		}
		optMenu.NameItem(6, text);

		optMenu.SetLeftArrow(6, (option_background > 0));
		optMenu.SetRightArrow(6, (option_background < maxBackground));


		

		
		/** Render *****************/
		if (inGame) {
			MoveCamera();
			Render(RENDER_BG);
		}
		else {
			DrawBackground();
		}
		optMenu.AutoArrange(static_cast<char>(1));
		optMenu.SpaceItems(7);
		optMenu.SpaceItems(8);
		optMenu.Display();
		// If the "replays" option is selected
		if (inGame && optMenu.GetSel() == 4) {
			note.Render();
		}
		UpdateScreen();
		
		
		/** Input ******************/
		action = optMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (optMenu.GetSel()) {
					case 0:
						ToggleSound();
						break;
					case 1:
						option_musicOn =
							(option_musicOn ?
							 false : true);
						break;
					case 2:
						option_timerOn =
							(option_timerOn ?
							 false : true);
						break;
					case 3:
						option_helpSpeech =
							(option_helpSpeech ?
							 false : true);
						break;
					case 4:
						option_replayOn =
							(option_replayOn ?
							 false : true);
						break;
					case 5:
						if (option_cameraMode <
						    maxCameraMode)
						{
							option_cameraMode++;
						}
						else {
							option_cameraMode = 0;
						}
						break;
					case 6:
						if (option_background <
						    maxBackground)
						{
							option_background++;
						}
						else {
							option_background = 0;
						}
						break;
					case 7:
						if (ControlSetupMenu(inGame) ==
						    -2)
							return -2;
						break;
					case 8:
						return -1;
						break;
				}
				break;
			case 3: // Left
				switch (optMenu.GetSel()) {
					case 0:
						if (option_soundOn)
							ToggleSound();
						break;
					case 1:
						option_musicOn = false;
						break;
					case 2:
						option_timerOn = false;
						break;
					case 3:
						option_helpSpeech = false;
						break;
					case 4:
						option_replayOn = false;
						break;
					case 5:
						if (option_cameraMode > 0)
							option_cameraMode--;
						break;
					case 6:
						if (option_background > 0)
							option_background--;
						break;
				}
				break;
			case 4: // Right
				switch (optMenu.GetSel()) {
					case 0:
						if (!option_soundOn) {
							ToggleSound();

							// play sound again
							// here, since the
							// sound in the menu
							// selection was muted.
							PlaySound(5);
						}
						break;
					case 1:
						option_musicOn = true;
						break;
					case 2:
						option_timerOn = true;
						break;
					case 3:
						option_helpSpeech = true;
						break;
					case 4:
						option_replayOn = true;
						break;
					case 5:
						if (option_cameraMode <
						    maxCameraMode)
							option_cameraMode++;
						break;
					case 6:
						if (option_background <
						    maxBackground)
							option_background++;
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
	}
}





int PauseMenu() {
	int numItems = 6;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Options");
	pauseMenu.NameItem(2, "Help");
	
	pauseMenu.NameItem(3, "Restart Level");
	pauseMenu.NameItem(4, "Change Level");
	pauseMenu.NameItem(5, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, (SCREEN_H / 2) - (FONT_H * (numItems - 1)));
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(3);

	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (pauseMenu.GetSel()) {
					case 1: // Options
						if (OptionsMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						else {
							// Sync the in-game player keymap with the preferences,
							// in case they changed.
							RefreshPlayerKeys();
						}
						break;
					case 2:
						if (HelpMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						break;
					default:
						return pauseMenu.GetSel();
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
		
		MoveCamera();
		Render(RENDER_BG);
		pauseMenu.Display();
		UpdateScreen();
	}
}




int EndOfLevelMenu() {
	int numItems = 4;
	
	menu theMenu(numItems);
	
	theMenu.SetTitle("NOW WHAT?");
	theMenu.NameItem(0, "Next Level");
	theMenu.NameItem(1, "View Replay");
	theMenu.NameItem(2, "Change Level");
	theMenu.NameItem(3, "Quit Game");
	
	theMenu.Move(SCREEN_W / 2, (SCREEN_H / 2) - (FONT_H * numItems));
	theMenu.AutoArrange(1);
	theMenu.SpaceItems(2);
	
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
		
		Render(RENDER_MOVECAMERA | RENDER_ANIMATE | RENDER_BG);
		theMenu.Display();
		UpdateScreen();
	}
}




int ReplayPauseMenu() {
	int numItems = 6;
	
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("REPLAY PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Take Control From Here");
	pauseMenu.NameItem(2, "Restart");
	pauseMenu.NameItem(3, "Options");
	pauseMenu.NameItem(4, "Next Level");
	pauseMenu.NameItem(5, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, (SCREEN_H / 2) - (FONT_H * numItems));
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(3);
	pauseMenu.SpaceItems(4);
	
	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (pauseMenu.GetSel()) {
					case 3: // Options
						if (OptionsMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						else {
							// Sync the in-game player keymap with the preferences,
							// in case they changed.
							RefreshPlayerKeys();
						}
						break;
					default:
						return pauseMenu.GetSel();
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
		
		MoveCamera();
		Render(RENDER_BG);
		pauseMenu.Display();
		UpdateScreen();
	}

}





int SelectLevelMenu() {
	uint i;
	int numItems = 2;
	menu lvlMenu(numItems); // Create the menu object
	char text[512]; // For holding complete level error message
	int bottomY = SCREEN_H - FONT_H - 4;
	
	FILE *testFile;
	bool refreshLevel = true;
	bool dummyLevel;
	uint numLevels = 1000;
	char notYetMsg = rand() % 3;
	char *levelError = NULL; // For pointing to the (actual) error message
	                         // returned by LoadLevel
	
	int action;
	
	lvlMenu.Move(SCREEN_W / 2, 4);
	lvlMenu.SetTitle("");
	lvlMenu.SetSel(1); // Select the level number menu item by default

	// Make the syntax error note
	txt errorNote(FONT_W * 3, FONT_H * 3, "");
	errorNote.Wrap((SCREEN_W * .9) - (FONT_W * 3));

	while (true) {
		/** Load Level ****/
		if (refreshLevel) {
			dummyLevel = false;
			while (true) {
				// Load Level
				delete [] levelError;
				// If currentLevel is higher than the max
				if ((option_levelSet == 0 &&
				     currentLevel > option_levelMax0) ||
				    (option_levelSet == 1 &&
				     currentLevel > option_levelMax1))
				{
					// Display a dummy level
					levelError = LoadLevel(-1);

					// Flag it as unplayable
					dummyLevel = true;

					// Make the player say a message
					char tmpMsg[37];
					switch (notYetMsg) {
						case 0:
							sprintf(tmpMsg,
							"You gotta do level %d "
							"first!",
							currentLevel - 1);
							break;
						case 1:
							sprintf(tmpMsg,
							"Did you beat level %d "
							"yet?",
							currentLevel - 1);
							break;
						case 2:
							sprintf(tmpMsg,
							"You haven't done "
							"level %d!",
							currentLevel - 1);
							break;
					}
					blocks[0].SetDir(2);
					Speak(0, tmpMsg);

				} else {
					levelError = LoadLevel(currentLevel);
				}

				// If there was some error
				if (levelError != NULL) {
					// If the level could not be loaded
					sprintf(text, "!");
					if (strcmp(levelError, text) == 0) {
						//numLevels = currentLevel;
						if (currentLevel <= 0) {
							sprintf(text,
							"The file for level %d "
							"doesn't exist!\n",
							currentLevel);

							numLevels = 1000;
						}
						else {
							// Try the previous
							// level
							currentLevel--;
							continue;
						}
					} else {
						// Update the errorNote's text
						sprintf(text,
						"Syntax errors exist in the "
						"level file:\n\n%s\n"
						"Please fix the errors and try "
						"selecting the level again.",
						levelError);
					}

					errorNote.SetText(text);
					errorNote.Wrap();
				}

				// Try to open the next level file, to see if
				// it exists, so we know whether to display a
				// right arrow
				testFile = OpenLevel(currentLevel + 1);
				if (testFile != NULL) {
					fclose(testFile);
				}
				else {
					numLevels = currentLevel + 1;
				}
				
				// Turn off all the status lights on the
				// telepads
				if (levelError == NULL) {
					for (i = 0; i < numTelepads; i++) {
						telepads[i].NeedsToTeleport();
					}
				}
				
				break;
			}
			
			refreshLevel = false;
		}


		/** Set dynamic menu items' text ****/
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
		lvlMenu.SetRightArrow(0,
			(option_levelSet < NUM_LEVEL_SETS - 1));
		
		// Determine Level text
		sprintf(text, "Level %d", currentLevel);
		lvlMenu.NameItem(1, text);
		lvlMenu.SetLeftArrow(1,	(currentLevel > 0));
		lvlMenu.SetRightArrow(1, (currentLevel < numLevels - 1));
		
		
		/** Render ****/
		if (levelError == NULL) {
			Render(RENDER_BG | RENDER_MOVECAMERA | RENDER_ANIMATE);
		}
		else {
			DrawBackground();
			
			errorNote.Render();
		}
		lvlMenu.AutoArrange(static_cast<char>(1));
		
		// Position the menu items to be stacked at the bottom of the
		// screen
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
				if (levelError == NULL && dummyLevel == false) {
					switch (lvlMenu.GetSel()) {
						case 0:
						case 1:
							delete [] levelError;
							return lvlMenu.GetSel();
							break;
					}
				}
				break;
			case 3: // Left
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet > 0)
							option_levelSet--;

						numLevels = 1000;
						refreshLevel = true;
						break;
					case 1:
						if (currentLevel > 0)
							currentLevel--;

						refreshLevel = true;
						break;
				}
				break;
			case 4: // Right
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet <
						    NUM_LEVEL_SETS - 1)
							option_levelSet++;

						numLevels = 1000;
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
		if (currentLevel > numLevels - 1) currentLevel = numLevels - 1;
	}
}
