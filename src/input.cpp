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


// Keys that can be pushed at any time
void GlobalInput(SDL_Event event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				// Change tilesets
				case SDLK_TAB:
					SelectTileset(0);
					break;
				case SDLK_BACKSPACE:
					SelectTileset(1);
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
					case SDLK_z:
						blocks[0].SetStrong(1);
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
	
	
	// If this is not a replay, do the input for Non-Playable Characters	
	if (mode == 0) {
		NPCInput();
	}
	
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
//	A negative value means that the key is being held down and this is a repetition
signed char MenuInput() {
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
					case SDLK_LCTRL:
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
					case SDLK_LALT:
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
		return -keyDown;
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
	gameKeys[0].sym = SDLK_a;	// Move camera left
	gameKeys[0].mod = KMOD_NONE;	// modifier

	gameKeys[1].sym = SDLK_d;	// Move camera right
	gameKeys[1].mod = KMOD_LSHIFT;	// modifier
	
	gameKeys[2].sym = SDLK_w;	// Move camera up
	gameKeys[2].mod = KMOD_LSHIFT;	// modifier
	
	gameKeys[3].sym = SDLK_s;	// Move camera down
	gameKeys[3].mod = KMOD_LSHIFT;	// modifier

	gameKeys[4].sym = SDLK_LSHIFT;	// Undo
	gameKeys[4].mod = KMOD_NONE;	// modifier

	gameKeys[5].sym = SDLK_F5;	// Restart Level
	gameKeys[5].mod = KMOD_NONE;	// modifier

	gameKeys[6].sym = SDLK_RETURN;	// Quit
	gameKeys[6].mod = KMOD_NONE;	// modifier
	
	gameKeys[7].sym = SDLK_F1;	// Help
	gameKeys[7].mod = KMOD_NONE;	// modifier



	/** Default Player keys ****/
	option_playerKeys[0].sym = SDLK_LEFT;		// Move player left
	option_playerKeys[1].sym = SDLK_RIGHT;		// Move player right
	option_playerKeys[2].sym = SDLK_UP;		// Pick up block
	option_playerKeys[3].sym = SDLK_DOWN;		// Set down block
	option_playerKeys[4].sym = SDLK_LCTRL;		// Push block
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
			return "START";
			break;
                case SDLK_ESCAPE:
			return "SELECT";
			break;

		case SDLK_LCTRL:
			return "A";
			break;
		case SDLK_LALT:
			return "B";
			break;
		case SDLK_LSHIFT:
			return "X";
			break;
		case SDLK_SPACE:
			return "Y";
			break;

		case SDLK_TAB:
			return "L";
			break;
		case SDLK_BACKSPACE:
			return "R";
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
					if (option_replaySpeed > 1) option_replaySpeed--;
					break;
				case SDLK_RIGHT:
					if (option_replaySpeed < 5) option_replaySpeed++;
					break;
				default:
					break;
			}
	}
	
}
