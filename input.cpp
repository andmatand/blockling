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


// Keys that can be pushed at any time
void GlobalInput(SDL_Event event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				// Toggle music
				case SDLK_F2:
					ToggleMusic();
					break;
				
				// Toggle sound
				case SDLK_F3:
					ToggleSound();
					break;
					
				// Toggle fullscreen
				case SDLK_f:
					#ifdef WINDOWS
						screenSurface = SDL_SetVideoMode(surface->w,
								surface->h,
								surface->format->BitsPerPixel,
								SDL_HWSURFACE |
									(surface->flags & (SDL_FULLSCREEN ? 0 : SDL_FULLSCREEN)));
					#else
						printf("FullScreen toggle: %d\n", SDL_WM_ToggleFullScreen(screenSurface));
					#endif
					break;
				
				default:
					break;
			}
		break;
	}
}





void GameInput(bool inReplay) {
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

	/** Handle repeat rate of Game Keys ****/
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

	/** Handle repeat rate of Player Keys ****/
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
		GlobalInput(event);
		if (inReplay) ReplayInput(event);
		
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
				if (inReplay == false) {
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
					case SDLK_KP_PLUS:
					case SDLK_PLUS:
					case SDLK_KP_MINUS:
					case SDLK_PAGEUP:
						LoadTileset("default");
						break;
					case SDLK_PAGEDOWN:
						LoadTileset("scifi");
						break;
						
					#ifdef DEBUG
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
					case SDLK_2:
						blockXSpeed = TILE_W;
						blockYSpeed = TILE_H;
						blockYGravity = blockYSpeed;
						break;
					case SDLK_x:
						blocks[0].SetWon(3);
						wonLevel = 3;
						break;
					#endif
					
					default:
						break;
				}


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
	// Keep track of which key is currently held down
	static char keyDown = 0;
	bool keyRepeating;
	static uint keyTimer = 0;
	
	char oldKeyDown = keyDown;
	
	while (SDL_PollEvent(&event)) {
		GlobalInput(event);
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_KP_ENTER:
					case SDLK_RETURN:
						return 5;
					case SDLK_UP:
						keyDown = 1;
						break;
					case SDLK_DOWN:
						keyDown = 2;
						break;
					case SDLK_LEFT:
						keyDown = 3;
						break;
					case SDLK_RIGHT:
						keyDown = 4;
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
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_UP:
						if (keyDown == 1) keyDown = 0;
						break;
					case SDLK_DOWN:
						if (keyDown == 2) keyDown = 0;
						break;
					case SDLK_LEFT:
						if (keyDown == 3) keyDown = 0;
						break;
					case SDLK_RIGHT:
						if (keyDown == 4) keyDown = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				return 9;
				break;
		}
	}
	
	
	/** Handle repeat rate of arrow keys ****/
	
	// If this key was not pushed last time
	if (keyDown != oldKeyDown) {
		keyRepeating = false;
		keyTimer = SDL_GetTicks();
		return keyDown;
	}
	// If the key is being held down
	else {
		// Initial delay
		if (SDL_GetTicks() >= keyTimer + 200) {
			keyRepeating = true;
		}
	}

	if (keyRepeating && SDL_GetTicks() >= keyTimer + 50) {
		keyTimer = SDL_GetTicks();
		return keyDown;
	}
	
	return 0;
}



void RefreshPlayerKeys() {
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
	 	playerKeys[i].sym = option_playerKeys[i].sym;
	}
}



void ResetDefaultKeys() {
	// Default Player keyboard layout
	option_playerKeys[0].sym = SDLK_LEFT;		// Move player left
	option_playerKeys[1].sym = SDLK_RIGHT;		// Move player right
	option_playerKeys[2].sym = SDLK_UP;		// Pick up block
	option_playerKeys[3].sym = SDLK_DOWN;		// Set down block
	option_playerKeys[4].sym = SDLK_RETURN;		// Push block
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



const char* KeyName(SDLKey sym) {
	switch (sym) {
		case SDLK_LEFT:
			return "LEFT";
			break;
		case SDLK_RIGHT:
			return "RIGHT";
			break;
		case SDLK_UP:
			return "UP";
			break;
		case SDLK_DOWN:
			return "DOWN";
			break;
		case SDLK_RETURN:
			return "ENTER";
			break;
		case SDLK_SPACE:
			return "SPACE";
			break;
		case SDLK_HOME:
			return "HOME";
			break;
		case SDLK_END:
			return "END";
			break;

		case SDLK_LSHIFT:
			return "LEFT SHIFT";
			break;
		case SDLK_RSHIFT:
			return "RIGHT SHIFT";
			break;
		case SDLK_LCTRL:
			return "LEFT CTRL";
			break;
		case SDLK_RCTRL:
			return "RIGHT CTRL";
			break;
		case SDLK_LALT:
			return "LEFT ALT";
			break;
		case SDLK_RALT:
			return "RIGHT ALT";
			break;
		case SDLK_TAB:
			return "TAB";
			break;


		case SDLK_1:
			return "1";
			break;
		case SDLK_2:
			return "2";
			break;
		case SDLK_3:
			return "3";
			break;
		case SDLK_4:
			return "4";
			break;
		case SDLK_5:
			return "5";
			break;
		case SDLK_6:
			return "6";
			break;
		case SDLK_7:
			return "7";
			break;
		case SDLK_8:
			return "8";
			break;
		case SDLK_9:
			return "9";
			break;
		case SDLK_0:
			return "0";
			break;

		case SDLK_LEFTBRACKET:
			return "[";
			break;
		case SDLK_RIGHTBRACKET:
			return "]";
			break;
		case SDLK_SEMICOLON:
			return ";";
			break;
		case SDLK_QUOTE:
			return "'";
			break;
		case SDLK_COMMA:
			return ",";
			break;
		case SDLK_PERIOD:
			return ".";
			break;
		case SDLK_SLASH:
			return "/";
			break;

		case SDLK_a:
			return "a";
			break;
		case SDLK_b:
			return "b";
			break;
		case SDLK_c:
			return "c";
			break;
		case SDLK_d:
			return "d";
			break;
		case SDLK_e:
			return "e";
			break;
		case SDLK_f:
			return "f";
			break;
		case SDLK_g:
			return "g";
			break;
		case SDLK_h:
			return "h";
			break;
		case SDLK_i:
			return "i";
			break;
		case SDLK_j:
			return "j";
			break;
		case SDLK_k:
			return "k";
			break;
		case SDLK_l:
			return "l";
			break;
		case SDLK_m:
			return "m";
			break;
		case SDLK_n:
			return "n";
			break;
		case SDLK_o:
			return "o";
			break;
		case SDLK_p:
			return "p";
			break;
		case SDLK_q:
			return "q";
			break;
		case SDLK_r:
			return "r";
			break;
		case SDLK_s:
			return "s";
			break;
		case SDLK_t:
			return "t";
			break;
		case SDLK_u:
			return "u";
			break;
		case SDLK_v:
			return "v";
			break;
		case SDLK_w:
			return "w";
			break;
		case SDLK_x:
			return "x";
			break;
		case SDLK_y:
			return "y";
			break;
		case SDLK_z:
			return "z";
			break;
		default:
			break;
	}
	
	return "[name unknown]";
}






void ReplayInput(SDL_Event event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					if (option_replaySpeed > 0) option_replaySpeed--;
					break;
				case SDLK_RIGHT:
					if (option_replaySpeed < 3) option_replaySpeed++;
					break;
				default:
					break;
			}
	}
	
}
