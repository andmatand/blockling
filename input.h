/*
 *   input.h
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

/*** Function Prototypes ***/
void GameInput();
char MenuInput();
void TurnOffAllKeys();



/*** Varibale/Struct Declarations ***/
struct keyBinding {
	SDLKey sym;	// Key name (e.g. SDLK_LEFT)
	SDLMod mod;	// Modifier (e.g. KMOD_LSHIFT) - must be pushed along with 'sym' for key to be 'on'
	char on; 	// If the key is pushed
	uint timer;	// Used for repeat rate
};

keyBinding gameKeys[NUM_GAME_KEYS];	// Game's keymap (for Quit, Undo, etc.)
keyBinding *playerKeys; 		// Players' keymap (for moving players).  Initialized in LoadLevel()

SDL_Event event;
