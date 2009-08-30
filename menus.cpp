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
	
	mainMenu.SetTitle("B L O C K M A N");
	mainMenu.NameItem(0, "Start Game");
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



int PauseMenu() {
	int numItems = 3;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("P A U S E D");
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
