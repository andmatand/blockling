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
	int numItems = NUM_PLAYER_KEYS + 2;
	menu csMenu(numItems); // Create the menu object
	char text[256]; // For temporarily holding menu items' text as it is formed
	char tempString[14];
	
	uint i;
	int action;
	bool gettingKey = false;
	
	/** Set static menu items **/
	csMenu.Move(inGame ? SCREEN_W / 2 : 75, 100);
	csMenu.SetTitle("Control Setup");
	csMenu.NameItem(numItems - 2, "Reset To Defaults");
	csMenu.NameItem(numItems - 1, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/
		for (i = 0; i < NUM_PLAYER_KEYS; i++) {
			// Determine text
			switch (i) {
				case 0:
					sprintf(tempString, "Move Left");
					break;
				case 1:
					sprintf(tempString, "Move Right");
					break;
				case 2:
					sprintf(tempString, "Pick Up Block");
					break;
				case 3:
					sprintf(tempString, "Drop Block");
					break;
				case 4:
					sprintf(tempString, "Push Block");
					break;
			}
			sprintf(text, "%s: %s",
				tempString,
				(gettingKey && csMenu.GetSel() == static_cast<int>(i)) ? "Press a key..." : KeyName(option_playerKeys[i].sym)
				);

			// Set it
			csMenu.NameItem(i, text);
		}


		/** Render *****************/
		do {
			if (inGame) {
				Render(0);
			}
			else {
				DrawBackground();
			}
			csMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
			csMenu.MoveItem(numItems - 2, csMenu.GetItemX(numItems - 2), csMenu.GetItemY(numItems - 2) + FONT_H);
			csMenu.MoveItem(numItems - 1, csMenu.GetItemX(numItems - 1), csMenu.GetItemY(numItems - 1) + FONT_H);
			csMenu.Display();
			SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
			
			if (inGame == false) LimitFPS();

			if (gettingKey) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym != SDLK_ESCAPE) {
							option_playerKeys[csMenu.GetSel()].sym = event.key.keysym.sym;
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
	
	mainMenu.SetTitle("BLOCKMAN");
	mainMenu.NameItem(0, "Play");
	mainMenu.NameItem(1, "Options");
	mainMenu.NameItem(2, "Quit");
	
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



int OptionsMenu(bool inGame) {
	int numItems = 5;
	menu optMenu(numItems); // Create the menu object
	char text[256]; // For temporarily holding menu items' text as it is formed
	char tempString[11];
	
	uint maxUndoSize = 500;
	char change_undoSize;
	
	int action;
	
	/** Set static menu items **/
	optMenu.Move(inGame ? SCREEN_W / 2 : 75, 100);
	optMenu.SetTitle("OPTIONS");
	optMenu.NameItem(3, "Control Setup");
	optMenu.NameItem(4, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/
		// Determine Undo text
		sprintf(text, "Undo Memory: ");
		if (option_undoSize > 0) strcat(text, "<"); else strcat(text, " ");
		
		sprintf(tempString, " %d move", option_undoSize);
		if (option_undoSize != 1) strcat(tempString, "s");
		strcat(text, tempString);
		
		if (option_undoSize < maxUndoSize) strcat(text, " >");
		
		// Set it
		optMenu.NameItem(0, text);
		
		
		// Determine Sound text
		sprintf(text, "Sound: ");
		strcat(text, (option_soundOn ? "ON" : "OFF"));
		// Set it
		optMenu.NameItem(1, text);

		
		// Determine Sound text
		sprintf(text, "Music: ");
		strcat(text, (option_musicOn ? "ON" : "OFF"));
		// Set it
		optMenu.NameItem(2, text);

		/** Render *****************/
		if (inGame) {
			Render(0);
		}
		else {
			DrawBackground();
		}
		optMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
		optMenu.Display();
		// If the undo option is selected
		if (inGame && optMenu.GetSel() == 0) {
			sprintf(text, "This setting will not take effect\nuntil a new level is loaded");
			DrawText((SCREEN_W / 2) - (GetTextW(text) / 2), 300, text, 200, 200, 200);
		}
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		if (inGame == false) LimitFPS();

		
		
		/** Input ******************/
		action = optMenu.Input();
		change_undoSize = 0;
		
		switch (action) {
			case 1: // Enter
				switch (optMenu.GetSel()) {
					case 0:
						change_undoSize = 1;
						if (option_undoSize == maxUndoSize) option_undoSize = 0;
						break;
					case 1:
						(option_soundOn) ? option_soundOn = false : option_soundOn = true;
						break;
					case 2:
						(option_musicOn) ? option_musicOn = false : option_musicOn = true;
						break;
					case 3:
						if (ControlSetupMenu(inGame) == -2) return -2;
						break;
					case 4:
						return -1;
						break;
				}
				break;
			case 3: // Left
				switch (optMenu.GetSel()) {
					case 0:
						change_undoSize = -1;						
						break;
					case 1:
						(option_soundOn) ? option_soundOn = false : option_soundOn = true;
						break;
					case 2:
						(option_musicOn) ? option_musicOn = false : option_musicOn = true;
						break;
				}
				break;
			case 4: // Right
				switch (optMenu.GetSel()) {
					case 0:
						change_undoSize = 1;
						break;
					case 1:
						(option_soundOn) ? option_soundOn = false : option_soundOn = true;
						break;
					case 2:
						(option_musicOn) ? option_musicOn = false : option_musicOn = true;
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
	int numItems = 4;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Options");
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
		
		Render(3);
		theMenu.Display();
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
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
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		LimitFPS();
	}

}

