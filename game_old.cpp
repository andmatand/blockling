/*
 *   game.cpp
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

bool LoadLevel(uint level);


void Game() {

	uint i;
	int x, b;
	std::string tempPath;
	char s[11];
	bool pushedKey;
	bool quitGame = false;
	char wonLevel; // 0 no, 1 yes
	
	currentLevel = 6;

	/*** Loop to advance to next level ***/
	while(quitGame == false) {
		// Load Level
		if (LoadLevel(currentLevel) == false) {
			fprintf(stderr, "Error: Loading level %d failed.", currentLevel);

			currentLevel = 0;
			//continue;
			escKey = 1;
			break;
		}
		
		keyOn = new char[numPlayers * NUM_KEYS]; // Will hold players's current key states
		for (i = 0; i < numPlayers * NUM_KEYS; i++) {
			keyOn[i] = 0;
		}
		keyTimer = new uint[numPlayers * NUM_KEYS]; // Will hold players's current key timers
		
		int playerBlock[numPlayers]; 	// Stores which block the player is currently picking
						// up and waiting on until he can move again.
		// Initialize playerBlock
		for (i = 0; i < numPlayers; i++) {
			playerBlock[i] = -1;
		}
		
		wonLevel = 0;
		

		/******* GAME LOOP *******/
		while (quitGame == false) {
			
			Input();
			if (escKey) {
				quitGame = true;
				std::cout << "quitGame = true\n";
			}
			
			
			/*** Handle Player Movement ***/
			for (i = 0; i < numPlayers; i++) {
				pushedKey = false; // This will forbid certain actions from both happening in the same frame
				

				// If the player is not already moving
				if (blocks[i].GetXMoving() == 0 && blocks[i].GetYMoving() == 0 
					// and is on solid ground
					&& OnSolidGround(i))
				{
					
					// Enter (push up block)
					if (pushedKey == false && playerBlock[i] == -1 && keyOn[(i * NUM_KEYS) + 4] > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - TILE_W;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), TILE_W, 1);
						
						if (b >= 0 && OnSolidGround(b)) {
							if (blocks[i].GetDir() == 0) {
								blocks[b].SetXMoving(-TILE_W);
							}
							else {
								blocks[b].SetXMoving(TILE_W);
							}
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					// Left
					if (keyOn[(i * NUM_KEYS) + 0] > 0) {
						if (blocks[i].GetDir() == 0) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								// If the space to the left is occupied...
								b = BoxContents(blocks[i].GetX() - TILE_W, blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH());
								if (b != -1) {
									// ...and the block (if it is a block) is on solid ground...
									if (b < 0 || OnSolidGround(b)) {
										// Find how much we need to subtract from y - TILE_H to get
										// the actual grid y-value, based on how much he has sunk
										// down from carrying something.
										if (blocks[i].GetY() < 0) {
											// Re-using variable x since it already exists
											x = (abs(blocks[i].GetY()) % TILE_H);
											if (x > 0) x = TILE_H - x;
										}
										else {
											x = blocks[i].GetY() % TILE_H;
										}

										// ...and the space up & left is free...
										if (BoxContents(blocks[i].GetX() - TILE_W, blocks[i].GetY() - TILE_H - x, TILE_W, TILE_H) == -1) {
											//Climb up block/brick
											tempPath = "";
											sprintf(s, "-%dy-%dx", TILE_H, TILE_W);
											tempPath = s;
											blocks[i].SetPath(tempPath);
										}
									}
								}
								else {
									blocks[i].SetXMoving(-TILE_W);
								}
								pushedKey = true; // Now player can't push other buttons this frame
							}
						}
						else {
							blocks[i].SetDir(0);
						}
					}

					// Right
					if (pushedKey == false && keyOn[(i * NUM_KEYS) + 1] > 0) {
						if (blocks[i].GetDir() == 1) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								// If the space to the right is occupied...
								b = BoxContents(blocks[i].GetX() + TILE_W, blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH());
								if (b != -1) {
									// ...and the block (if it is a block) is on solid ground...
									if (b < 0 || OnSolidGround(b)) {
										// Find how much we need to subtract from y - TILE_H to get
										// the actual grid y-value, based on how much he has sunk
										// down from carrying something.
										if (blocks[i].GetY() < 0) {
											// Re-using variable x since it already exists
											x = (abs(blocks[i].GetY()) % TILE_H);
											if (x > 0) x = TILE_H - x;
										}
										else {
											x = blocks[i].GetY() % TILE_H;
										}

										// ...and the space up and right is free
										if (BoxContents(blocks[i].GetX() + TILE_W, blocks[i].GetY() - TILE_H - x, TILE_W, TILE_H) == -1) {
											//Climb up block/brick
											tempPath = "";
											sprintf(s, "-%dy%dx", TILE_H, TILE_W);
											tempPath = s;
											blocks[i].SetPath(tempPath);
										}
									}
								}
								else {
									blocks[i].SetXMoving(TILE_W);
								}
								pushedKey = true; // Now player can't push other buttons this frame
							}
						}
						else {
							blocks[i].SetDir(1);
						}
					}
					
					// Up (pick up block)
					if (pushedKey == false && playerBlock[i] == -1 && keyOn[(i * NUM_KEYS) + 2] > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - TILE_W;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), TILE_W, 1);
						
						// If it's a block, set its path to be picked up.
						if (b >= 0) {
							// Find how much the player is sunk, so we don't have
							// to move the block up all the way to TILE_H
							if (blocks[i].GetY() < 0) {
								// Re-using variable x since it already exists
								x = (abs(blocks[i].GetY()) % TILE_H);
								if (x > 0) x = TILE_H - x;
							}
							else {
								x = blocks[i].GetY() % TILE_H;
							}

							tempPath = "";
							if (blocks[i].GetDir() == 0) {
								sprintf(s, "-%dy%dx", TILE_H - x, TILE_W);
								tempPath = s;
							}
							else {
								sprintf(s, "-%dy-%dx", TILE_H - x, TILE_W);
								tempPath = s;
							}
							
							blocks[b].SetPath(tempPath);
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					
					// Down (set down block)
					if (pushedKey == false && playerBlock[i] == -1 && keyOn[(i * NUM_KEYS) + 3] > 0) {
						// What's on top of player's head?
						b = BlockNumber(blocks[i].GetX(), blocks[i].GetY() - TILE_H, TILE_W, TILE_H);

						// If it's a block, set its path to be set down.
						if (b >= 0 && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
							// The mod TILE_H is to first move the block up
							// by how ever many pixels the player has sunk
							// down from carying the weight.
							if (blocks[i].GetY() < 0) {
								// Re-using variable x since it already exists
								x = (abs(blocks[i].GetY()) % TILE_H);
								if (x > 0) x = TILE_H - x;
							}
							else {
								x = blocks[i].GetY() % TILE_H;
							}

							tempPath = "";
							if (blocks[i].GetDir() == 0) { // player facing left
								sprintf(s, "-%dy-%dx", x, TILE_W);
								tempPath = s;
							}
							if (blocks[i].GetDir() == 1) { // player facing right
								sprintf(s, "-%dy%dx", x, TILE_W);
								tempPath = s;
							}
							
							blocks[b].SetPath(tempPath);
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					

				}

				// Check if player is waiting for a block he's acted upon to finish moving
				if (playerBlock[i] != -1) {
					// If it's not moving (along a path) anymore
					if (blocks[playerBlock[i]].GetPath().length() == 0 && blocks[playerBlock[i]].GetXMoving() == 0 && blocks[playerBlock[i]].GetYMoving() == 0) {
						// No longer wait for it.
						playerBlock[i] = -1;
					}
				}
			}
			
			
			/*** Do Block Physics ***/
			// Reset all DidPhysics flags
			for (i = 0; i < numBlocks; i++) {
				blocks[i].SetDidPhysics(false);
				blocks[i].SetMoved(false);
			}
			// Do Physics
			for (i = 0; i < numBlocks; i++) {
				if (blocks[i].GetDidPhysics() == false) {
					//printf("===Block %d Physics===\n", i);
					blocks[i].Physics();
				}
			}
			// Do Post-Physics (decrement xMoving and yMoving)
			for (i = 0; i < numBlocks; i++) {
				blocks[i].PostPhysics();
			}
			
			
			// For each player:
			// If player is not floating and there is no block on his head, make him rise
			for (i = 0; i < numPlayers; i++) {
				if (blocks[i].GetType() == 1 && blocks[i].GetH() < TILE_H && BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, blocks[i].GetW(), 1) < 0) {
					b = blocks[i].GetY(); 		// Save old y position
					blocks[i].SetYMoving(-2);	// Set the block to move up
					blocks[i].Physics(); 		// Try to move the block up
					if (blocks[i].GetY() < b) 	// If it moved up, increase the height by the amount moved
						blocks[i].SetH(blocks[i].GetH() + (b - blocks[i].GetY()));
				}
			}

			// Check if player is in certain locations
			for (i = 0; i < numPlayers; i++) {
				// Is player on a spike?
				if (BoxContents(blocks[i].GetX(), blocks[i].GetY() + blocks[i].GetH(), blocks[i].GetW(),1) == -3) {
					blocks[i].SetDir(3); // dead
				}
				
				// Is player at exit?
				if (blocks[i].GetDir() != 3 && blocks[i].GetXMoving() == 0) {
					if (BoxOverlap(exitX, exitY, TILE_W, TILE_H, blocks[i].GetX(), blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH())) {
						blocks[i].SetDir(2);
						wonLevel = 1;
					}
				}
			}


			// Center camera on player
			//cameraX = (blocks[0].GetX() + (blocks[0].GetW() / 2)) - (SCREEN_W / 2);
			//cameraY = (blocks[0].GetY() + (blocks[0].GetH() / 2)) - (SCREEN_H / 2);

			/** Render **/
			Render();
			
			if (wonLevel == 1) {
				SDL_Delay(1000);
				currentLevel ++;
				break;
			}
			
		} // End of game loop
		
		/** Collect garbage **/
		delete [] bricks; bricks = NULL;
		delete [] blocks; blocks = NULL;
		delete [] telepads; telepads = NULL;
		delete [] torches; torches = NULL;
		delete [] keyOn; keyOn = NULL;
		delete [] keyTimer; keyTimer = NULL;
	} // Back to top of while loop to load next level
}






bool LoadLevel(uint level) {
	char filename[32];
	FILE * f;

	sprintf(filename, "data/levels/%03d.txt", level);
	f = fopen(filename, "rb");
	if (f == NULL) {
		return false;
	}

	// Get info about level (width, height, numBlocks, numBricks, etc.)
	int c;
	int x = 0, y = 0, width = 0, height = 0;
	
	int telepadLetter[26];
	for (uint i = 0; i < 26; i++) {
		telepadLetter[i] = 0;
	}
	bool addNewTelepad;
	
	numBlocks = 0;
	numPlayers = 0;
	numBricks = 0;
	numTelepads = 0;
	numTorches = 0;
	numSpikes = 0;
	while (!feof(f)) {
		c = fgetc(f);

		if (c > 32) {
			switch (c) {
				case '@': // player
					numPlayers ++;
					numBlocks ++;
					break;
				case '*': // exit
					break;
				case '^':
					numSpikes ++;
					break;
				case 'T':
					numTorches ++;
				case '0':
					numBricks ++;
					break;
				case 'x':
				case 'X':
					numBlocks ++;
					break;
				default:
					break;
			}
			// Check for lowercase a - z (telepad pairs)
			if (static_cast<int>(c) >= 97 && static_cast<int>(c) <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == c) {
						addNewTelepad = false;
						break;
					}
				}
				if (addNewTelepad) {
					telepadLetter[numTelepads] = static_cast<int>(c);
					numTelepads ++;
				}
			}
			
			x += TILE_W;
			if (x > width) width = x;
		}

		// New Line (LF)
		if (c == 10) {
			x = 0;
			height += TILE_H;			
		}
	}
	

	// Position level according to size, and initialize variables
	// according to number of objects

	x = -(width / 2);
	x += (abs(x) % TILE_W); // Align to grid
	
	y = -(height / 2);
	y += (abs(y) % TILE_H); // Align to grid

	cameraX = -(SCREEN_W / 2);
	cameraY = -(SCREEN_H / 2);

	bricks = new brick[numBricks];
	blocks = new block[numBlocks];
	telepads = new telepad[numTelepads];
	torches = new torch[numTorches];
	spikes = new spike[numSpikes];
	
	// Default physics settings
	blockXSpeed = 8;
	blockYSpeed = 8;
	blockXGravity = 0;
	blockYGravity = blockYSpeed;



	// Now read the level again, getting object coordinates
	numBlocks = numPlayers; // We'll re-use these variables for array indexes
	numPlayers = 0;
	numBricks = 0;
	numTorches = 0;
	numSpikes = 0;

	// Reset telepad paring array
	for (uint i = 0; i < 26; i++) {
		telepadLetter[i] = 0;
	}

	rewind(f); // Go back to the beginning of the file
	while (!feof(f)) {
		c = fgetc(f);
		
		if (c > 32) {
			switch (c) {
				case '@': // player
					blocks[numPlayers].SetX(x);
					blocks[numPlayers].SetY(y);
					blocks[numPlayers].SetType(1);
					numPlayers ++;
					break;
				case '*': // exit
					exitX = x;
					exitY = y;
					break;
				case '^':
					spikes[numSpikes].SetX(x);
					spikes[numSpikes].SetY(y);
					numSpikes ++;
					break;
				case 'T': // torch
					torches[numTorches].SetX(x);
					torches[numTorches].SetY(y);
					numTorches ++;
				case '0': // brick
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					numBricks ++;
					break;
				case 'X': // block
					blocks[numBlocks].SetX(x);
					blocks[numBlocks].SetY(y);
					numBlocks ++;
					break;
				default:
					break;
			}

			// Check for lowercase a - z (telepad pairs)
			if (static_cast<int>(c) >= 97 && static_cast<int>(c) <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == c) {
						addNewTelepad = false;
						break;
					}
				}
				if (addNewTelepad) { // This is the first telepad in the pair
					telepadLetter[static_cast<int>(c) - 97] = static_cast<int>(c);
					telepads[static_cast<int>(c) - 97].SetX1(x);
					telepads[static_cast<int>(c) - 97].SetY1(y);
					std::cout << "Telepad " << static_cast<int>(c) - 97 << " " << static_cast<char>(c) << "1" << " (" << x << ", " << y << ")\n";
				}
				else { // This is the second telepad in the pair
					telepads[static_cast<int>(c) - 97].SetX2(x);
					telepads[static_cast<int>(c) - 97].SetY2(y);
					std::cout << "Telepad " << static_cast<int>(c) - 97 << " " << static_cast<char>(c) << "2" << " (" << x << ", " << y << ")\n";
				}
			}

			x += TILE_W;
		}


		// New Line (LF)
		if (c == 10) {
			x = -(width / 2);
			x += (abs(x) % TILE_W); // Align to grid
			
			y += TILE_H;
		}
	}
	fclose(f);

	return true;
}
