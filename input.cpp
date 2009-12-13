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
				case SDLK_RETURN:
					if (event.key.keysym.mod & KMOD_LALT || event.key.keysym.mod & KMOD_RALT) {
						#ifdef _WIN32
							printf("Windows fullscreen toggle\n");
							//SDL_FreeSurface(screenSurface);
							screenSurface = SDL_SetVideoMode(screenSurface->w,
									screenSurface->h,
									screenSurface->format->BitsPerPixel,
									SDL_HWSURFACE |
										((screenSurface->flags & SDL_FULLSCREEN) ? 0 : SDL_FULLSCREEN) |
										SDL_ANYFORMAT
										);
						#else
							printf("fullscreen toggle: %d\n", SDL_WM_ToggleFullScreen(screenSurface));
						#endif
					}
					break;
				
				default:
					break;
			}
		break;
	}
}





// Mode:
// 0 = normal game input
// 1 = replay input (only game keys & replay input)
int GameInput(char mode) {
	for (uint i = 4; i < NUM_GAME_KEYS; i++) {
		if (gameKeys[i].on == 1) {
			gameKeys[i].on = -1; // will be seen by game as Off
		}
		else if (gameKeys[i].on == -1) {
			// Initial delay
			if (SDL_GetTicks() >= gameKeys[i].timer + 200) {
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
		CameraInput(event);
		if (mode == 1) ReplayInput(event);
		
		switch (event.type) {
			case SDL_KEYDOWN:
				/** Turn on Game Keys (minus the camera keys) **/
				for (uint i = 4; i < NUM_GAME_KEYS; i++) {
					if (event.key.keysym.sym == gameKeys[i].sym && (gameKeys[i].mod == KMOD_NONE || event.key.keysym.mod & gameKeys[i].mod)) {
						if (gameKeys[i].on == 0) {
							gameKeys[i].on = 1;
							gameKeys[i].timer = SDL_GetTicks();
						}
					}
				}

				/** Turn on Player Keys **/
				if (mode == 0) {
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
				/** Turn off Game Keys (minus the camera keys) **/
				for (uint i = 4; i < NUM_GAME_KEYS; i++) {
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
				return -2;
				break;
		}
	}
	
	
	
	// Add NPC access to playerKeys here
	NPCInput();
	
	return 0;
}





// Move the Non-Playable Characters around
void NPCInput() {
	uint i;
	bool ok;
	int b;
	char key;
	
	// Turn off all NPC keys
	for (i = 1; i < numPlayers; i++) {
		for (uint j = i * NUM_PLAYER_KEYS; j < (i + 1) * NUM_PLAYER_KEYS; j++) {
			playerKeys[j].on = 0;
		}
	}
	

	for (i = 1; i < numPlayers; i++) {
		key = -1;
		switch (blocks[i].GetType()) {
			// Slow random movement
			case 11:
				if (rand() % 20 == 0) {
					if (rand() % 2 == 0) {
						// Left
						key = 0;
					}
					else {
						// Right
						key = 1;
					}
				}
				break;
		}
		
		/** Look before we step, to make sure it's safe ****/
		ok = true; // start with the assumption that it's safe
		// If this button will cause him to walk and not just change directions
		if (blocks[i].GetDir() == key) {
			#ifdef DEBUG
				printf("** NPC %d: Checking for dangerous step...\n", i);
			#endif
			
			int x;
			if (blocks[i].GetDir() == 0) {	// Facing left
				x = blocks[i].GetX() - TILE_W;
			}
			else { 				// Facing right
				x = blocks[i].GetX() + blocks[i].GetW();
			}
			
			// Get contents of tile immediately below NPC at the new X position
			b = BoxContents(x, blocks[i].GetY() + blocks[i].GetH(), TILE_W, TILE_H);
			// if the tile is empty or a spike
			if (b == -1 || b == -3) {
				ok = false; // It's not safe
				
				// If there is a floating telepad on the current Y that he can step on instead of ground
				if (BoxContents(x, blocks[i].GetY(), TILE_W, TILE_H) == -4) {
					ok = true; // It's safe
				}
	
				// If the tile one space over and two spaces down is safe to stand on
				b = BoxContents(x, blocks[i].GetY() + blocks[i].GetH() + TILE_H, TILE_W, 1);
				if (b != -1 && b != -3) { // (not empty or a spike)
					ok = true; // It's safe
				}
			}
		}
		
		#ifdef DEBUG
			if (ok == false) {
				printf("** NPC %d: Refrained from dangerous step.\n", i);
			}
		#endif
		
		// Push whatever key was determined
		if (key >= 0 && ok) {
			playerKeys[(i * NUM_PLAYER_KEYS) + key].on = 1;
		}
	}
}





void CameraInput(SDL_Event event) {
	if (!physicsStarted) return;
		
	switch (event.type) {
		case SDL_KEYDOWN:
			// Turn on camera keys (first four gamekeys)
			for (uint i = 0; i < 4; i++) {
				if (event.key.keysym.sym == gameKeys[i].sym && (gameKeys[i].mod == KMOD_NONE || event.key.keysym.mod & gameKeys[i].mod)) {
					if (gameKeys[i].on == 0) {
						gameKeys[i].on = 1;
						gameKeys[i].timer = SDL_GetTicks();
					}
				}
			}

			break;
		case SDL_KEYUP:
			// Turn off camera keys (first four gamekeys)
			for (uint i = 0; i < 4; i++) {
				if (event.key.keysym.sym == gameKeys[i].sym) {
					gameKeys[i].on = 0;
				}
			}

			break;
	}
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
	static bool keyRepeating;
	static uint keyTimer = 0;
	
	char oldKeyDown = keyDown;
	
	while (SDL_PollEvent(&event)) {
		GlobalInput(event);
		CameraInput(event);
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_KP_ENTER:
					case SDLK_RETURN:
						if (!(event.key.keysym.mod & KMOD_ALT)) {
							return 5;
						}
						break;
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
	else if (keyRepeating == false) {
		// Initial delay
		if (SDL_GetTicks() >= keyTimer + 250) {
			keyRepeating = true;
		}
	}
	
	// Repeat delay
	if (keyRepeating && SDL_GetTicks() >= keyTimer + 50) {
		keyTimer = SDL_GetTicks();
		return keyDown;
	}
	
	return 0;
}



void RefreshPlayerKeys() {
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
	 	playerKeys[i].sym = option_playerKeys[i].sym;
	 	playerKeys[i].timer = 0;
	}
}



void ResetDefaultKeys() {
	/** Default Game Keys ****/
	//gameKeys[0].sym = SDLK_LEFT;	// Move camera left
	//gameKeys[0].mod = KMOD_LSHIFT;	// modifier
	gameKeys[0].sym = SDLK_a;	// Move camera left
	gameKeys[0].mod = KMOD_NONE;	// modifier

	//gameKeys[1].sym = SDLK_RIGHT;	// Move camera right
	//gameKeys[1].mod = KMOD_LSHIFT;	// modifier
	gameKeys[1].sym = SDLK_d;	// Move camera right
	gameKeys[1].mod = KMOD_NONE;	// modifier
	
	//gameKeys[2].sym = SDLK_UP;	// Move camera up
	//gameKeys[2].mod = KMOD_LSHIFT;	// modifier
	gameKeys[2].sym = SDLK_w;	// Move camera up
	gameKeys[2].mod = KMOD_NONE;	// modifier
	
	//gameKeys[3].sym = SDLK_DOWN;	// Move camera down
	//gameKeys[3].mod = KMOD_LSHIFT;	// modifier
	gameKeys[3].sym = SDLK_s;	// Move camera down
	gameKeys[3].mod = KMOD_NONE;	// modifier

	gameKeys[4].sym = SDLK_u;	// Undo
	gameKeys[4].mod = KMOD_NONE;	// modifier

	gameKeys[5].sym = SDLK_F5;	// Restart Level
	gameKeys[5].mod = KMOD_NONE;	// modifier

	gameKeys[6].sym = SDLK_ESCAPE;	// Quit
	gameKeys[6].mod = KMOD_NONE;	// modifier
	
	gameKeys[7].sym = SDLK_F1;	// Help
	gameKeys[7].mod = KMOD_NONE;	// modifier



	/** Default Player keys ****/
	option_playerKeys[0].sym = SDLK_LEFT;		// Move player left
	option_playerKeys[1].sym = SDLK_RIGHT;		// Move player right
	option_playerKeys[2].sym = SDLK_UP;		// Pick up block
	option_playerKeys[3].sym = SDLK_DOWN;		// Set down block
	option_playerKeys[4].sym = SDLK_RETURN;		// Push block
}



void TurnOffAllKeys() {
	uint i;
	
	// Turns off only player 0's keys
	for (i = 0; i < NUM_PLAYER_KEYS; i++) {
		playerKeys[i].on = 0;
	}
	
	// Turns off all game keys
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
		case SDLK_INSERT:
			return "INSERT";
			break;
		case SDLK_DELETE:
			return "DELETE";
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
					if (option_replaySpeed < 5) option_replaySpeed++;
					break;
				default:
					break;
			}
	}
	
}
