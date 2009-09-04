/*
 *   input.cpp
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


void GameInput() {
	
	// Reset all Players's keys to 0 and let SDL_EnableKeyRepeat handle repeat rate
	/*
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
		playerKeys[i].on = 0;
	}
	// And Game keys
	for (uint i = 0; i < NUM_GAME_KEYS; i++) {
		gameKeys[i].on = 0;
	}
	*/

	/** Manually handle repeat rate of keys already on **/
	
	// Game Keys
	for (uint i = 0; i < NUM_GAME_KEYS; i++) {
		if (gameKeys[i].on == 1) {
			gameKeys[i].on = -1; // will be seen by game as Off
		}
		else if (gameKeys[i].on == -1) {
			// Initial delay
			if (SDL_GetTicks() >= gameKeys[i].timer + ((i >= 2 && i <= 5) ? 0 : 200)) {
				gameKeys[i].on = 2; // will be seen by game as On
				gameKeys[i].timer = SDL_GetTicks();
			}
		}
		else if (abs(gameKeys[i].on) == 2) {
			// Repeat delay
			if (SDL_GetTicks() >= gameKeys[i].timer + 10) {
				gameKeys[i].on = 2; // will be seen by game as On
				gameKeys[i].timer = SDL_GetTicks();
			}
			else {
				gameKeys[i].on = -2; // will be seen by game as Off
			}
		}
	}

	// Player Keys
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
		if (playerKeys[i].on == 1) {
			playerKeys[i].on = -1; // will be seen by game as Off
		}
		else if (playerKeys[i].on == -1) {
			// Initial delay
			if (SDL_GetTicks() >= playerKeys[i].timer + 200) {
				playerKeys[i].on = 2; // will be seen by game as On
				playerKeys[i].timer = SDL_GetTicks();
			}
		}
		/*
		else if (abs(playerKeys[i].on) == 2) {
			// Repeat delay
			if (SDL_GetTicks() >= playerKeys[i].timer + 0) {
				playerKeys[i].on = 2; // will be seen by game as On
				playerKeys[i].timer = SDL_GetTicks();
			}
			else {
				playerKeys[i].on = -2; // will be seen by game as Off
			}
		}
		*/
	}




	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				/** Turn on Game Keys **/
				for (uint i = 0; i < NUM_GAME_KEYS; i++) {
					if (event.key.keysym.sym == gameKeys[i].sym && (gameKeys[i].mod == KMOD_NONE || event.key.keysym.mod & gameKeys[i].mod)) {
						if (gameKeys[i].on == 0) {
							gameKeys[i].on = 1;
							gameKeys[i].timer = SDL_GetTicks();
							
							/*
							// Make camera controls smoother by turning off LEFT when RIGHT is pushed, etc.
							if (i == 2) gameKeys[3].on = 0;
							if (i == 3) gameKeys[2].on = 0;
							if (i == 4) gameKeys[5].on = 0;
							if (i == 5) gameKeys[4].on = 0;
							*/
						}
					}
				}

				/** Turn on Player Keys **/
				for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
					if (event.key.keysym.sym == playerKeys[i].sym) {
						if (playerKeys[i].on == 0) {
							// If shift is held
							if (event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT) {
								playerKeys[i].on = 3;
							}
							else {
								playerKeys[i].on = 1;
								playerKeys[i].timer = SDL_GetTicks();
							}
						}
					}
				}
				

				
				break;

			case SDL_KEYUP:		
				/** Turn off Game Keys **/
				for (uint i = 0; i < NUM_GAME_KEYS; i++) {
					if (event.key.keysym.sym == gameKeys[i].sym) {
						gameKeys[i].on = 0;
					}
				}

				/** Turn off Player Keys **/
				for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
					if (event.key.keysym.sym == playerKeys[i].sym) {
						playerKeys[i].on = 0;
					}
				}

				switch (event.key.keysym.sym) {
					case SDLK_f:
						printf("FullScreen toggle: %d\n", SDL_WM_ToggleFullScreen(screenSurface));
						break;
					default:
						break;
				}


				
				#ifdef DEBUG
				switch (event.key.keysym.sym) {
					case SDLK_1:
						if (blockXSpeed == 8) {
							blockXSpeed = 1;
							blockYSpeed = 1;
							blockYGravity = blockYSpeed;
						}
						else {
							blockXSpeed = 8;
							blockYSpeed = 8;
							blockYGravity = blockYSpeed;
						}
						break;
					case SDLK_KP_PLUS:
					case SDLK_PLUS:
					case SDLK_EQUALS:
						currentLevel ++;
						break;
					case SDLK_KP_MINUS:
					case SDLK_MINUS:
						currentLevel --;
						break;
					case SDLK_x:
						blocks[0].SetWon(3);
						wonLevel = 3;
						break;
					case SDLK_PAGEUP:
						LoadTileset("default");
						break;
					case SDLK_PAGEDOWN:
						LoadTileset("scifi");
						break;
					default:
						break;
				}
				#endif



				break;

			case SDL_QUIT:
				gameKeys[0].on = 1;
				break;
		}
	}
	
	
	
	// Add NPC access to playerKeys here

}



// Return values:
//	0 No key
//	1 Up
//	2 Down
//	3 Left
//	4 Right
//	5 Enter
//	6 Home
//	7 End
//	8 Esc
//	9 Window close button
//	10 Page Up
//	11 Page Down
char MenuInput() {

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_KP_ENTER:
					case SDLK_RETURN:
						return 5;
					case SDLK_UP:
						return 1;
						break;
					case SDLK_DOWN:
						return 2;
						break;
					case SDLK_HOME:
						return 6;
						break;
					case SDLK_END:
						return 7;
						break;
					case SDLK_PAGEUP:
						return 10;
						break;
					case SDLK_PAGEDOWN:
						return 11;
						break;
					case SDLK_BACKSPACE:
					case SDLK_ESCAPE:
						return 8;
						break;
					default:
						break;
				}
				break;
			//case SDL_KEYUP:
				
			//	break;
			case SDL_QUIT:
				return 9;
				break;
		}
	}
	
	return 0;
}




void TurnOffAllKeys() {
	uint i;
	for (i = 0; i < NUM_PLAYER_KEYS; i++) {
		playerKeys[i].on = 0;
	}
	for (i = 0; i < NUM_GAME_KEYS; i++) {
		gameKeys[i].on = 0;
	}
}
