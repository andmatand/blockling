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
	int numItems = 3;
	menu optMenu(numItems);
	char undoText[23];
	char tempString[10];
	uint maxUndoSize = 500;
	
	optMenu.SetTitle("OPTIONS");
	optMenu.Move(inGame ? SCREEN_W / 2 : 75, 100);
	
	int action;
	
	while (true) {
		//sprintf(undoText, "Undo Memory: %d moves", option_undoSize);
		undoText[0] = '\0';
		strcat(undoText, "Undo Memory: ");
		if (option_undoSize > 0) strcat(undoText, "<"); else strcat(undoText, " ");
		
		sprintf(tempString, " %d moves", option_undoSize);
		strcat(undoText, tempString);
		
		if (option_undoSize < maxUndoSize) strcat(undoText, " >");
		
		
		optMenu.NameItem(0, undoText);
		optMenu.NameItem(1, "Control Setup");
		optMenu.NameItem(2, "Done");
		optMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));

		action = optMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (optMenu.GetSel()) {
					case 0:
						option_undoSize += 50;
						if (option_undoSize > maxUndoSize) option_undoSize = 0;
						break;
					case 1:
						break;
					case 2:
						return -1;
						break;
				}
				break;
			case 3: // Left
				if (option_undoSize >= 50) option_undoSize -= 50;
				break;
			case 4: // Right
				option_undoSize += 50;
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}

		if (option_undoSize > maxUndoSize) option_undoSize = maxUndoSize;
		
		if (inGame) {
			Render(0);
		}
		else {
			DrawBackground();
		}
		optMenu.Display();
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

