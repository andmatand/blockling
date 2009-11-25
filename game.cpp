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


void CollectLevelGarbage() {
	printf("Collecting Level Garbage...\n");
	
	delete [] bricks; bricks = NULL;
	delete [] blocks; blocks = NULL;
	delete [] telepads; telepads = NULL;
	delete [] torches; torches = NULL;
	delete [] playerKeys; playerKeys = NULL;
	delete [] playerBlock; playerBlock = NULL;

	if (undoBlocks != NULL) {
		for (uint i = 0; i < maxUndo; i++) {
			delete [] undoBlocks[i];
			undoBlocks[i] = NULL;
		}
		delete [] undoBlocks;
		undoBlocks = NULL;
	}
	
	if (undoTelepads != NULL) {
		for (uint i = 0; i < maxUndo; i++) {
			// Set old undoTelepad pointers to NULL before
			// destructor tries to free memory which is
			// already freed
			for (uint j = 0; j < numTelepads; j++) {
				undoTelepads[i][j].DeInitTeleport(false);
			}
			
			delete [] undoTelepads[i];
			undoTelepads[i] = NULL;
		}

		delete [] undoTelepads;
		undoTelepads = NULL;
	}
}





int Game() {

	int returnVal = 0;
	uint i, j;
	int x, b;
	std::string tempPath;
	char s[11];
	bool pushedKey;
	bool quitGame = false;
	bool restartLevel;
	float cameraXVel, cameraYVel;
	uint wonLevelTimer = 0;

	uint oldLevel = currentLevel;
	stickyPlayer = false;

	// Replay variables
	bool recordingReplay = false;
	showingReplay = false;
	int currentReplayKey;
	bool replayKeyWorked;
	menu *replayMenu = NULL;
	uint frameNumber; // counts frames for doing frameskipping for fast replays
	
	// Replay object pointer
	replay *neatoReplay = NULL;

	// Get full filename for the replay temp file
	char replayTempFile[256];
	sprintf(replayTempFile, "%sblockman-replay.tmp", TEMP_PATH);

	/*** Loop to advance to next level ***/
	while(quitGame == false) {
		wonLevel = 0;
		levelTime = 0;
		levelTimeRunning = false;
		physicsStarted = false;

		// Don't sync the real undo memory size with the
		// (possibly changed) option if we are doing a replay
		// which needs to keep the same conditions as the
		// original playthrough.
		if (showingReplay == false) {
			maxUndo = option_undoSize;
		}

		if (stickyPlayer || showingReplay || currentLevel != oldLevel || restartLevel) {
			// Load Level without a menu (e.g. from in-game "Next Level" or "Restart Level")
			while (true) {
				if (LoadLevel(currentLevel) != NULL) {
					fprintf(stderr, "Error: Loading level %d failed.\n", currentLevel);
	
					currentLevel = 0;
					continue;
				}
				break;
			}
		}
		else {
			// Load Level Menu
			selectingLevel = true;
			switch (SelectLevelMenu()) {
				case 0:
					break;
				case -1: // Esc
					return -1;
					break;
				case -2: // Close window
					return -2;
					break;
			}
		}
		selectingLevel = false;
		restartLevel = false;
		oldLevel = currentLevel;
		
		playerBlock = new int[numPlayers];
		
		// Initialize playerBlock
		for (i = 0; i < numPlayers; i++) {
			playerBlock[i] = -1;
		}
		
		
		// Reset manual camera movement
		manualCameraTimer = 0;
		cameraXVel = 0;
		cameraYVel = 0;
	
		// Reset keys
		for (i = 0; i < NUM_GAME_KEYS; i++) {
			gameKeys[i].on = 0;
		}
		for (i = 0; i < numPlayers; i++) {
			for (uint j = 0; j < NUM_PLAYER_KEYS; j++) {
				playerKeys[(i * NUM_PLAYER_KEYS) + j].on = 0;
			}
		}


		/*** Initialze replay ***/
		recordingReplay = (option_replayOn ? true : false);	
		replayKeyWorked = false;
	
		if (showingReplay) {
			// Initialize the replay for reading
			neatoReplay->InitRead();
			
			// Disable recording
			recordingReplay = false;
			
			// Create the speed selection menu
			replayMenu = new menu(1);
			replayMenu->SetTitle("");
			replayMenu->Move(SCREEN_W / 2, 0);
		}
		else if (recordingReplay) {
			// Create the replay object
			neatoReplay = new replay(replayTempFile, 100); // A buffer of 100 takes about 500 bytes of memory

			// Initialize the replay object for writing
			neatoReplay->InitWrite();
		}

		/******* GAME LOOP *******/
		while (quitGame == false && selectingLevel == false && restartLevel == false) {
			GameInput(showingReplay ? true : false);
						
			// Handle Restart Level key
			if (gameKeys[5].on == 1) {
				showingReplay = false;
				stickyPlayer = false;
				restartLevel = true;
				break;
			}

			/** Set speed of replay ****/
			if (showingReplay) {
				switch (option_replaySpeed) {
					case 0:
						// Slow physics
						blockXSpeed = 1;
						blockYSpeed = 1;
						blockYGravity = blockYSpeed;
						break;
					default:
						// Normal-speed physics
						blockXSpeed = TILE_W / 2;
						blockYSpeed = TILE_H / 2;
						blockYGravity = blockYSpeed;
						break;
				}
			}

			/*** Pause Menu (when Quit button [Esc] is pushed) ***/
			if (gameKeys[6].on > 0) {
				PlaySound(6); // Menu ENTER sound
				
				j = SDL_GetTicks() - levelTimeTick;
				
				if (showingReplay) {
					i = currentLevel;
					
					// Show special replay pause menu
					switch (ReplayPauseMenu()) {
						case -1: // Esc
						case 0: // Resume
							break;
						case 1: // Take Control
							// Turn off any keys that might still be on
							TurnOffAllKeys();
							
							// Reset physics to normal speed
							blockXSpeed = TILE_W / 2;
							blockYSpeed = TILE_H / 2;
							blockYGravity = blockYSpeed;							
							
							showingReplay = false;
							break;
						case 2: // Restart the replay
							stickyPlayer = false;
							showingReplay = true;
							restartLevel = true;
							break;
						case 4: // Next Level
							showingReplay = false;
							currentLevel += 1;
							break;
						case -2: // Close Window
							returnVal = -2;
						case 5: // Quit Game
							showingReplay = false;
							quitGame = true;
							break;
					}
					
					// If "Next Level" was chosen, break from main loop
					if (currentLevel != i) break;
				}
				else { // Normal pause menu
					b = 0;
					while (b == 0) {
						switch (PauseMenu()) {
							case 0: // Resume Game
								b = 1;
								break;
							case 1: // Options
								OptionsMenu(true);
								
								// Sync the in-game player keymap with the preferences,
								// in case they changed.
								RefreshPlayerKeys();
								break;
							case 2: // Help
								// Help();
								break;
							case 3: // Restart Level
								b = 1;
								showingReplay = false;
								stickyPlayer = false;
								restartLevel = true;
								break;
							case 4: // Switch Level
								b = 1;
								showingReplay = false;
								stickyPlayer = false;
								selectingLevel = true;
								break;

							case -1: // Esc
								b = 1;
								break; // Resume the game
							case -2: // Close window
								b = 1;
								returnVal = -2;
							case 5: // Quit Game
								b = 1;
								showingReplay = false;
								quitGame = true;
								break;
						}
					}
				}
				
				// No more Esc
				//gameKeys[0].on = 0;
				
				// Clear any keys that may have been held when Esc was pressed
				TurnOffAllKeys();
				
				// Make the time spent in the menu undetectable to the timer incrementer
				levelTimeTick = SDL_GetTicks() - j;
			}
			
			
			
			/** Manual Camera Movement **/
			if (physicsStarted) {
				cameraXVel *= .85f;
				cameraYVel *= .85f;
				
				// Move camera left
				if (gameKeys[0].on > 0) {
					cameraXVel -= 2;
					manualCameraTimer = SDL_GetTicks();
				}
				
				// Move camera right
				if (gameKeys[1].on > 0) {
					cameraXVel += 2;
					manualCameraTimer = SDL_GetTicks();
				}
				
				// Move camera up
				if (gameKeys[2].on > 0) {
					cameraYVel -= 2;
					manualCameraTimer = SDL_GetTicks();
				}

				// Move camera down
				if (gameKeys[3].on > 0) {
					cameraYVel += 2;
					manualCameraTimer = SDL_GetTicks();
				}
				
				// Enforce maximum camera velocity limitations
				if (cameraXVel > TILE_W) cameraXVel = TILE_W;
				if (cameraXVel < -TILE_W) cameraXVel = -TILE_W;
				if (cameraYVel > TILE_H) cameraYVel = TILE_H;
				if (cameraYVel < -TILE_H) cameraYVel = -TILE_H;
				
				cameraX += static_cast<int>(cameraXVel);
				cameraY += static_cast<int>(cameraYVel);
			}
			
			
			/*** Handle Player Movement ***/
			for (i = 0; i < numPlayers; i++) {
				pushedKey = false; // This will forbid multiple actions from happening in the same frame

				
				if (showingReplay) {
					// Turn undo key off
					gameKeys[4].on = 0;

					// If the next key in the replay is Undo, press that
					// here (not inside the normal keypressing scope which
					// requires that the player be on solid ground)
					if (neatoReplay->GetNextKey() == 5) {
						neatoReplay->PushNextKey();
					}
				}
				
				// Undo
				if (gameKeys[4].on > 0) {
					if (recordingReplay) neatoReplay->SaveKey(5); // Save the keypress in the replay
					if (showingReplay) replayKeyWorked = true;
					
					Undo(1);
					pushedKey = true;
				}

				// If the player is not already moving
				if (blocks[i].GetPathLength() == 0 &&blocks[i].GetXMoving() == 0 && blocks[i].GetYMoving() == 0 
					// and is on solid ground
					&& blocks[i].OnSolidGround()
					// and a key hasn't been pushed yet
					&& !pushedKey
					// and the block is not disabled
					&& blocks[i].GetType() >= 0
					// and the player is still playing (hasn't won)
					&& blocks[i].GetWon() == 0
					// and the player is either facing left or right (not toward camera)
					&& (blocks[i].GetDir() == 0 || blocks[i].GetDir() == 1))
				{
					
					/*** Perform next keypress from replay file ***/
					if (showingReplay) {
						// Turn all playerKeys off
						for (uint j = 0; j < NUM_PLAYER_KEYS; j++) {
							playerKeys[j].on = 0;
						}
						
						if (physicsStarted) {
							currentReplayKey = neatoReplay->GetNextKey();
							//printf("currentReplayKey = %d\n", currentReplayKey);
						
							// Push the next key if it's okay
							if (currentReplayKey != 100) {
								//if (replayKeyWorked == false) printf("Key didn't do anything.  Trying again.\n");
								//printf("pushing key %d\n", currentReplayKey);
								
								neatoReplay->PushKey(currentReplayKey);
								if (currentReplayKey == -1) {
									replayKeyWorked = true;
								}
								else {
									replayKeyWorked = false;
								}
							}
						}
					}
					
					
					// Enter (push a block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 4].on > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - TILE_W;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), TILE_W, 1); // First try to push from bottom of player
						if (b < 0) b = BlockNumber(x, blocks[i].GetY(), TILE_W, 1); // If that didn't work, push from top
						
						if (b >= 0 && blocks[b].OnSolidGround()) {
							PlaySound(2); // Play sound
							if (recordingReplay) neatoReplay->SaveKey(4); // Save the keypress in the replay
							if (showingReplay) replayKeyWorked = true;
							Undo(0); // Save Undo state
							
							if (blocks[i].GetDir() == 0) {
								blocks[i].SetXMoving(-TILE_W);
								blocks[b].SetXMoving(-TILE_W);
							}
							else {
								blocks[i].SetXMoving(TILE_W);
								blocks[b].SetXMoving(TILE_W);
							}
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1); // If player is strong, make this block strong for now
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					// Left
					if (pushedKey == false && playerKeys[(i * NUM_PLAYER_KEYS) + 0].on > 0) {
						if (blocks[i].GetDir() == 0) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								// Try to set block's path to climb/walk left
								blocks[i].Climb(0);
								
								// If the player is actually going to move
								if (blocks[i].GetPathLength() > 0 || blocks[i].GetXMoving() != 0) {
									Undo(0); // Save Undo state
									if (recordingReplay) neatoReplay->SaveKey(0); // Save the keypress in the replay
									if (showingReplay) replayKeyWorked = true;
									pushedKey = true; // Now player can't push other buttons this frame
								}
							}
						}
						else {
							if (recordingReplay) neatoReplay->SaveKey(0); // Save the keypress in the replay
							if (showingReplay) replayKeyWorked = true;
							
							blocks[i].SetDir(0);
							
							pushedKey = true;
						}
					}

					// Right
					if (pushedKey == false && playerKeys[(i * NUM_PLAYER_KEYS) + 1].on > 0) {
						if (blocks[i].GetDir() == 1) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {								
								// Try to set block's path to climb/walk right
								blocks[i].Climb(1);

								// If the player is actually going to move
								if (blocks[i].GetPathLength() > 0 || blocks[i].GetXMoving() != 0) {
									Undo(0); // Save Undo state
									if (recordingReplay) neatoReplay->SaveKey(1); // Save the keypress in the replay	
									if (showingReplay) replayKeyWorked = true;
									pushedKey = true; // Now player can't push other buttons this frame
								}
							}
						}
						else {
							if (recordingReplay) neatoReplay->SaveKey(1); // Save the keypress in the replay
							if (showingReplay) replayKeyWorked = true;
							
							blocks[i].SetDir(1);
							
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
		
					// Down (set down block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 3].on > 0) {
						// What's on top of player's head?
						b = BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, TILE_W, 1);

						// If it's a block, set its path to be set down.
						if (b >= 0 && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
							PlaySound(1); // Play sound
							if (recordingReplay) neatoReplay->SaveKey(3); // Save the keypress in the replay
							if (showingReplay) replayKeyWorked = true;
							Undo(0); // Save Undo state
							
							if (blocks[i].GetDir() == 0) { // player facing left
								sprintf(s, "-%dy-%dx", TILE_H - blocks[i].GetH(), TILE_W);
							}
							if (blocks[i].GetDir() == 1) { // player facing right
								sprintf(s, "-%dy%dx", TILE_H - blocks[i].GetH(), TILE_W);
							}
							blocks[b].SetPath(s);
							
							// If player is strong, make this block strong for now
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1);
							
							// If the player is carrying more than one block on his
							// head and is trying to set them down, make this
							// block temporarily strong.
							if (BlockNumber(blocks[b].GetX(), blocks[b].GetY() - 1, TILE_W, 1) >= 0) {
								blocks[b].SetStrong(1);
							}
							
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
								
					// Up (pick up block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 2].on > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - 1;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY(), 1, 1);
						
						// If it's a block, make it climb up onto the player =)
						if (b >= 0 && blocks[b].GetType() >= 0) {
							PlaySound(0); // Play sound
							if (recordingReplay) neatoReplay->SaveKey(2); // Save the keypress in the replay
							if (showingReplay) replayKeyWorked = true;
							Undo(0); // Save Undo state
							
							// If player is strong, make this block strong for now
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1);
							
							// Climb
							blocks[b].Climb(static_cast<char>(
										(blocks[i].GetDir() == 0)
										? 1
										: 0
									));
							
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					if (pushedKey == false && recordingReplay && levelTimeRunning && wonLevel < 2) {
						neatoReplay->SaveKey(-1); // Save the non-keypress (sleep) in the replay
					}
					
					// Prevent bug where player moves in very first frame before player's
					// face is set back to normal in Render() (from zing/stickyPlayer)
					if (physicsStarted == false && pushedKey == true) {
						blocks[0].SetFace(0); // normal face
					}
					
					// Only count a replay key as pushed if it did something
					//if (showingReplay && pushedKey && replayKeyWorked) {
					if (showingReplay && replayKeyWorked) {
						neatoReplay->DecrementKey();
					}
				}
				

				// Check if player is waiting for a block he's acted upon to finish moving
				if (playerBlock[i] != -1) {
					// If it's not moving (along a path) anymore
					if (blocks[playerBlock[i]].GetPathLength() == 0 && blocks[playerBlock[i]].GetXMoving() == 0 && blocks[playerBlock[i]].GetYMoving() == 0) {
						// Not strong anymore
						blocks[playerBlock[i]].SetStrong(0);
						
						// No longer wait for it.
						playerBlock[i] = -1;
					}
				}
			}

			

			
			/*** Do Block Physics ***/
			if (!stickyPlayer && physicsStarted) {
				// Do Pre-Physics
				for (i = 0; i < numBlocks; i++) {
					blocks[i].SetDidPhysics(false);
					blocks[i].SetMoved(false);
				}

				// Do Physics
				for (i = 0; i < numBlocks; i++) {
					if (blocks[i].GetDidPhysics() == false) {
						//printf("===Block %d Physics===\n", i);
						blocks[i].Physics();
						
						// Temporarily suspend all physics if a player reaches the exit
						if (blocks[i].GetWon() == 1) {
							wonLevel = 1;
							break;
						}
					}
				}

				// For each player:
				// If player is not floating and there is no block on his head, make him rise
				for (i = 0; i < numPlayers; i++) {
					if (blocks[i].GetType() >= 10 && blocks[i].GetH() < TILE_H && BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, blocks[i].GetW(), 1) < 0) {
						b = blocks[i].GetY(); 			// Save old y position
						blocks[i].SetYMoving(-blockYSpeed);	// Set the block to move up
						if (blocks[i].GetYMoving() < -2) blocks[i].SetYMoving(-2);
						
						// Temporarily disable xMoving
						x = blocks[i].GetXMoving(); // Save it
						blocks[i].SetXMoving(0); // Set it to 0
						
						blocks[i].Physics(); 			// Try to move the block up
						
						blocks[i].SetXMoving(x); // Restore xMoving
						
						if (blocks[i].GetY() < b) 		// If it moved up, increase the height by the amount moved
							blocks[i].SetH(blocks[i].GetH() + (b - blocks[i].GetY()));
					}
				}

				// Do Post-Physics (decrement xMoving and yMoving, revoke temporary strength privileges)
				if (wonLevel != 1) {
					for (i = 0; i < numBlocks; i++) {
						blocks[i].PostPhysics();
					}
				}
			}
			


			// Check if player is on/in certain tiles
			for (i = 0; i < numPlayers; i++) {
				// Is player on a spike?
				if (BoxContents(blocks[i].GetX(), blocks[i].GetY() + blocks[i].GetH(), blocks[i].GetW(),1) == -3) {
					blocks[i].SetFace(4); // scared mouth
					blocks[i].SetDir(2); // Facing the camer
				}
				
				// If the player just finished walking into the exit (with the door open)
				if (blocks[i].GetWon() == 2 && blocks[i].GetX() == exitX && blocks[i].GetY() - blocks[i].GetYOffset() == exitY) {
					blocks[i].SetWon(3);
					if (i == 0) {
						PlaySound(8);
						
						blocks[i].SetFace(3); // happy mouth
						blocks[i].SetDir(2); // Face the camera

						wonLevel = 3;
						wonLevelTimer = SDL_GetTicks();
					}
					else {
						// If this isn't player 0, make it disappear
						blocks[i].SetType( static_cast<char>(-blocks[i].GetType()) );
						wonLevel = 0;
					}
				}

			}
			
		

			/*** Center camera on player ***/
			// Position cameraY so that the level is lined up with the next level's Y position of the player
			if (stickyPlayer) {
				//cameraX = -blocks[0].GetX() - stickyPlayerX
				cameraTargetX = (
							blocks[0].GetX() - stickyPlayerX
						)
						+ (SCREEN_W / 2); // Nullify what the CenterCamera function will
								  // do to change this coordinate.
				
				cameraTargetY = (
							blocks[0].GetY() - stickyPlayerY
						)
						+ (SCREEN_H / 2);
			}
			else {
				SetCameraTargetBlock(0);
			}


			// Open the door for a player that won
			for (i = 0; i < numBlocks; i++) {
				if (blocks[i].GetWon() == 1) {
					blocks[i].SetFace(1); // open mouth
					while (wonLevel < 2) {
						Render(1);
					}
					
					blocks[i].SetWon(2); // player can now finish walking into the door
				}
			}

			/** Timer **/
			// Increment the timer
			if (levelTimeRunning && wonLevel == 0 && SDL_GetTicks() >= levelTimeTick + 1000) {
				if (showingReplay == false || option_replaySpeed == 1) {
					levelTime ++;
					levelTimeTick = SDL_GetTicks();
				}
			}
			// Start the timer when the first movement is made
			if (levelTimeRunning == false && pushedKey) {
				levelTimeRunning = true;
				levelTimeTick = SDL_GetTicks();
				physicsStarted = true; // Also start physics if they haven't already
			}


			/*** Draw the Background (underneath teleportation animation) ***/
			DrawBackground();

			/*** Do Telepads (including teleportation animation) ***/
			for (i = 0; i < numTelepads; i++) {
				// If the telpad is currently in the process of teleporting something
				// OR the telepad needs to teleport
				if (telepads[i].GetTeleporting() == true) {
					telepads[i].Teleport();
				}
				else if (telepads[i].NeedsToTeleport()) {
					telepads[i].Teleport();
				}
			}
			
			/** Render **/
			if (showingReplay) {
				
				// Skip frames depending on playback speed of replay
				// (and determine menu text)
				frameNumber++;
				replayMenu->SetLeftArrow(0, 1);
				replayMenu->SetRightArrow(0, 1);
				switch (option_replaySpeed) {
					case 0: // Slow motion
						sprintf(s, "SLOW");
						replayMenu->SetLeftArrow(0, 0);

						frameNumber = 0; // Show every frame
						break;
					case 1: // Normal speed
						sprintf(s, "REALTIME");

						frameNumber = 0; // Show every frame
						break;
					case 2: // 2x
						sprintf(s, "2x");

						// Skip every second frame
						if (frameNumber >= 2) {
							frameNumber = 0;
						}
						break;
					case 3: // 4x
						sprintf(s, "4x");

						if (frameNumber >= 4) {
							frameNumber = 0;
						}
						break;
					case 4: // 8x
						sprintf(s, "8x");
						replayMenu->SetRightArrow(0, 0);

						if (frameNumber >= 8) {
							frameNumber = 0;
						}
						break;
				}
				
				// Draw speed selection menu
				if (wonLevel == 0) {
					replayMenu->NameItem(0, s);
					replayMenu->AutoArrange(1);
					replayMenu->MoveItem(0, replayMenu->GetItemX(0), SCREEN_H - FONT_H - 4);
					replayMenu->Display();
				}
			}
			if (showingReplay == false || frameNumber == 0) {
				Render(4);
			}

			/*** Stuff for when the player reached the exit ***/
			if (wonLevel == 3 && SDL_GetTicks() > wonLevelTimer + 1000) {
				if ((recordingReplay && option_replayOn) || showingReplay) {
					// Show menu asking what to do next
					switch (EndOfLevelMenu()) {
						case -1: // Esc
						case 0: // Next Level
							showingReplay = false;
							break;
						case 1: // View Replay
							stickyPlayer = false;
							showingReplay = true;
							break;
						case -2: // Close window
							showingReplay = false;
							quitGame = true;
							returnVal = -2;
							break;
					}
				}

				wonLevel = 4;
				if (showingReplay == false) {
					currentLevel ++;
				}
				
				break;
			}
		} // End of game loop
		
			
		/** Zing level offscreen **/
		if (quitGame == false && wonLevel == 4 && showingReplay == false) {
			cameraTargetX = SCREEN_W * 4;
			cameraTargetY = cameraY;
			wonLevelTimer = SDL_GetTicks();
			while (cameraX < levelX + levelW) {
				// Make the player look scared shortly after he starts "moving"
				if (SDL_GetTicks() >= wonLevelTimer + 500)
					blocks[0].SetFace(4); // Scared
				
				CenterCamera(2);
				Render(2);
			}
		}
		
		// Collect garbage for global object pointers
		CollectLevelGarbage();

		// Collect garbage for replay
		if (recordingReplay) {
			neatoReplay->DeInitWrite();
		}

		if (showingReplay == false) {
			delete neatoReplay;
			neatoReplay = NULL;
			
			if (remove(replayTempFile) != 0) {
				fprintf(stderr, "File error: Could not delete %s\n", replayTempFile);
			}
			
			delete replayMenu;
			replayMenu = NULL;
		}

	} // Back to top of while loop to load next level
	
	return returnVal;
}



FILE * OpenLevel(uint level) {
	// Find the name of the levelset directory
	char levelSet[16];
	switch (option_levelSet) {
		case 0:
			sprintf(levelSet, "default");
			break;
		case 1:
			sprintf(levelSet, "bman1");
			break;
		case 2:
			sprintf(levelSet, "custom");
			break;
	}

	char levelFile[4];
	sprintf(levelFile, "%03d", level);
	char filename[256];
	sprintf(filename, "%s%s%s/%s", DATA_PATH, LEVEL_PATH, levelSet, levelFile);

	#ifdef DEBUG
	printf("\nLoading level %d...\n", level);
	printf("filename: \"%s\"\n", filename);
	#endif

	return fopen(filename, "r");
}




char * LoadLevel(uint level) {
	char *errorMsg = new char[256]; // For holding error messages
	errorMsg[0] = '\0';
	char temp[256]; // For assembling strings of error messages
	temp[0] = '\0';
	
	// Free memory used by previous level
	CollectLevelGarbage();
	
	// Zero all game object count variables
	numBlocks = 0;
	numPlayers = 0;
	numBricks = 0;
	numTelepads = 0;
	numTorches = 0;
	numSpikes = 0;
	


	bool syntaxError = false;
	
	FILE *f;
	f = OpenLevel(level);
	if (f == NULL) {
		// Signal that the file could not be opened
		sprintf(errorMsg, "!");
		return errorMsg;
	}

	/** Read the file for the first time, to get info about level (width, height, numBlocks, numBricks, etc.) ****/
	int c;
	int x = 0, y = 0, width = 0, height = 0;
	
	char telepadLetter[26];
	int telepadMates[26];
	for (uint i = 0; i < 26; i++) {
		telepadLetter[i] = 0;
		telepadMates[i] = 0;
	}
	bool addNewTelepad;
	
	bool charOnThisLine = false;
	bool restOfLineIsComment = false;
	int numExits = 0;
	while (!feof(f)) {
		c = fgetc(f);

		if (c > 32 && restOfLineIsComment == false) {
			switch (c) {
				case '#': // Comment
					restOfLineIsComment = true;
					break;
				case '@': // player
					numPlayers ++;
					numBlocks ++;
					break;
				case '*': // exit
					numExits ++;
					break;
				case '^':
					numSpikes ++;
					break;
				case 'T':
					numTorches ++;
				case '0':
				case '1':
				case '2':
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
			if (c >= 97 && c <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == static_cast<char>(c)) {
						telepadMates[i] ++;
						addNewTelepad = false;
						break;
					}
				}
				if (addNewTelepad) {
					telepadLetter[numTelepads] = static_cast<char>(c);
					numTelepads ++;
				}
			}
			
			// Only increment the level height if there is a character on this line
			if (charOnThisLine == false && restOfLineIsComment == false) {
				height += TILE_H;
				charOnThisLine = true;
			}

			x += TILE_W;
			if (x > width) width = x;
		}

		// New Line (LF)
		if (c == 10) {
			charOnThisLine = false;
			restOfLineIsComment = false;
			x = 0;
		}
	}
	
	
	/** Check Level for Errors ******/
	
	// Check for telepad errors
	for (uint i = 0; i < numTelepads; i++) {
		if (telepadMates[i] != 1) {
			sprintf(temp, "-Telepad '%c' should have 1 mate, but\nit has %d mates.\n", telepadLetter[i], telepadMates[i]);
			strcat(errorMsg, temp);
			syntaxError = true;
			break; // Only report one telepad error at a time, so the string doesn't overflow
		}
	}

	// Check for missing exit
	if (numExits != 1) {
		sprintf(temp, "-There must be exactly 1 level exit,\nbut there are %d.\n", numExits);
		strcat(errorMsg, temp);
		syntaxError = true;
	}
	
	// Check for missing player
	if (numPlayers == 0) {
		sprintf(temp, "-There must be at least one player.\n");
		strcat(errorMsg, temp);
		syntaxError = true;
	}
	
	// Don't load the level if there are syntax errors
	if (syntaxError == true) {
		fclose(f);
		return errorMsg;
	}
	



	/** Position level according to size, and initialize variables
	    according to number of objects **/

	x = -(width / 2);
	x += (abs(x) % TILE_W); // Align to grid
	
	y = -(height / 2);
	y += (abs(y) % TILE_H); // Align to grid
	
	// These global variables (*gasp*) are used by the camera
	levelX = x;
	levelY = y;
	levelW = width;
	levelH = height;
	
	#ifdef DEBUG
	printf("levelX = %d\n", levelX);
	printf("levelY = %d\n", levelY);
	printf("levelW = %d\n", levelW);
	printf("levelH = %d\n", levelH);
	#endif

	playerKeys = new keyBinding[NUM_PLAYER_KEYS * numPlayers];
	RefreshPlayerKeys(); // Assign option_playerKeys values to playerKeys
	
	
	bricks = new brick[numBricks];
	blocks = new block[numBlocks];
	telepads = new telepad[numTelepads];
	torches = new torch[numTorches];
	spikes = new spike[numSpikes];

	
	undoBlocks = new block*[maxUndo];
	for (uint i = 0; i < maxUndo; i++) {
 		undoBlocks[i] = new block[numBlocks];
	}
	
	undoTelepads = new telepad*[maxUndo];
	for (uint i = 0; i < maxUndo; i++) {
 		undoTelepads[i] = new telepad[numTelepads];
	}
	
	Undo(-1); // Reset undo

	
	// Default physics settings
	blockXSpeed = TILE_W / 2;
	blockYSpeed = TILE_H / 2;
	blockXGravity = 0;
	blockYGravity = blockYSpeed;

	// Reset brick types to "undetermined"
	for (uint i = 0; i < numBricks; i++) {
		bricks[i].SetType(-1);
	}


	/** Read the file again, this time storing object's positions ****/
	numBlocks = numPlayers; // We'll re-use these variables for array indexes
	numPlayers = 0;
	numBricks = 0;
	numTorches = 0;
	numSpikes = 0;

	// Reset telepad mate counts
	for (uint i = 0; i < 26; i++) {
		telepadMates[i] = -1;
	}

	rewind(f); // Go back to the beginning of the file
	while (!feof(f)) {
		c = fgetc(f);
		
		if (c > 32 && restOfLineIsComment == false) {
			// Only increment the y coordinate if there is a character on this line
			if (charOnThisLine == false && restOfLineIsComment == false) {
				y += TILE_H;
				charOnThisLine = true;
			}

			switch (c) {
				case '#': // Comment
					restOfLineIsComment = true;
					y -= TILE_H;
					break;
				case '@': // player
					blocks[numPlayers].SetX(x);
					blocks[numPlayers].SetY(y);
					blocks[numPlayers].SetType(10);
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
					bricks[numBricks].SetType(0); // Make sure the brick
					                              //we're about to make is a "wall" brick
					// Proceed down to next case (put a brick behind the torch)
				case '0': // normal brick (type automatically selected)
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					numBricks ++;
					break;
				case '1': // wall brick (manual override)
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					bricks[numBricks].SetType(0);
					numBricks ++;
					break;
				case '2': // grass brick (manual override)
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					bricks[numBricks].SetType(-2);
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
			if (c >= 97 && c <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == c) {
						// If this is the second telepad in the pair
						if (telepadMates[i] == 0) {
							telepads[i].SetX2(x);
							telepads[i].SetY2(y);

							#ifdef DEBUG
							std::cout << "Telepad #" << i << " " << telepadLetter[i] << "2" << " (" << x << ", " << y << ")\n";
							#endif
						}

						// If this is the first telepad in the pair
						if (telepadMates[i] == -1) {
							telepadMates[i] = 0;
							telepads[i].SetX1(x);
							telepads[i].SetY1(y);
							
							#ifdef DEBUG
							std::cout << "Telepad #" << i << " " << telepadLetter[i] << "1" << " (" << x << ", " << y << ")\n";
							#endif
						}
					
						break;
					}
				}

			}

			x += TILE_W;
		}


		// New Line (LF)
		if (c == 10) {
			charOnThisLine = false;
			restOfLineIsComment = false;
			
			x = -(width / 2);
			x += (abs(x) % TILE_W); // Align to grid
		}
	}
	fclose(f);
	
	/*** Change brick types based on their placement next to other bricks ***/
	int leftBrick, rightBrick, topBrick, bottomBrick;
	// Make vertical bricks into "wall" bricks
	for (uint i = 0; i < numBricks; i++) {
		// Only change bricks that have not yet been set
		if (bricks[i].GetType() != -1) continue;
		
		// Find surrounding bricks, and ignore them if they are of type 0 (wall)
		leftBrick = BrickNumber(bricks[i].GetX() - TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
		if (bricks[leftBrick].GetType() == 0) leftBrick = -1; // Pretend like this brick isn't here
		
		rightBrick = BrickNumber(bricks[i].GetX() + TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
		if (bricks[rightBrick].GetType() == 0) rightBrick = -1; // Pretend like this brick isn't here
		
		topBrick = BrickNumber(bricks[i].GetX(), bricks[i].GetY() - TILE_H, TILE_W, TILE_H);
		//if (bricks[topBrick].GetType() == 0) topBrick = -1; // Pretend like this brick isn't here
		
		bottomBrick = BrickNumber(bricks[i].GetX(), bricks[i].GetY() + TILE_H, TILE_W, TILE_H);
		//if (bricks[bottomBrick].GetType() == 0) bottomBrick = -1; // Pretend like this brick isn't here
		
		// If (there is a brick above this brick
		// OR there is a brick below this brick)
		// AND there is no brick to the left
		// AND there is no brick to the right
		if ((topBrick >= 0
			|| bottomBrick >= 0)
			&& leftBrick == -1
			&& rightBrick == -1)
		{
			bricks[i].SetType(0); // Wall
			continue;
		}
	}
	
	
	// Do "land" bricks
	for (uint i = 0; i < numBricks; i++) {
		if (bricks[i].GetType() < 0) {
			leftBrick = BrickNumber(bricks[i].GetX() - TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
			// Ignore wall bricks (because they aren't "connected" to land bricks)
			if (leftBrick >= 0) {
				if (bricks[leftBrick].GetType() == 0) {
					leftBrick = -1;
				}
			}

			rightBrick = BrickNumber(bricks[i].GetX() + TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
			// Ignore wall bricks (because they aren't "connected" to land bricks)
			if (rightBrick >= 0) {
				if (bricks[rightBrick].GetType() == 0) {
					rightBrick = -1;
				}
			}

			// If there is no brick to the left
			// AND there is a brick to the right
			if (leftBrick == -1
				&& rightBrick >= 0)
			{
				bricks[i].SetType(1); // Left-side piece of land
				continue;
			}

			// If there is a brick to the left
			// AND there is a brick to the right
			if (leftBrick >= 0
				&& rightBrick >= 0)
			{
				bricks[i].SetType(2); // Middle piece of land
				continue;
			}

			// If there is a brick to the left
			// AND there is no brick to the right
			if (leftBrick >= 0
				&& rightBrick == -1)
			{
				bricks[i].SetType(3); // Right-side piece of land
				continue;
			}

			// If there is no brick to the left
			// AND there is no brick to the right
			if (leftBrick == -1
				&& rightBrick == -1)
			{
				bricks[i].SetType(4); // Single piece of land
				continue;
			}
		}
	}


	// Make the player face the exit
	if (exitX > blocks[0].GetX()) {
		blocks[0].SetDir(1);
	}
	else {
		blocks[0].SetDir(0);
	}


	// Position cameraY so that the level is centered on the player
	SetCameraTargetBlock(0);
	CenterCamera(1);
	
	if (stickyPlayer) {
		// Position cameraX so that the level is just offscreen to the right
		cameraX = -SCREEN_W - (levelW / 2);

		// Make cameraY line up with where the sticky player needs to be
		cameraY = blocks[0].GetY() - stickyPlayerY;
		
		blocks[0].SetFace(4); // Scared
	}
	else {
		CenterCamera(-1); // Instantly move camera and zero camera speed
	}
	
	return NULL;
}







// -1 = reset, 0 = save, 1 = load
void Undo(char action) {
	static int undoSlot = 0; // The slot number in the ringbuffer that will be written to next time Undo is called
	static int undoStart = 0; // The slot number of the oldest state.  We cannot undo past this slot.
	static int undoEnd = 0; // The slot number of the newest state.  If Undo(1) is called, this is the state that will be loaded.

	if (maxUndo <= 0) return;
	
	if (action == -1) {
		undoSlot = 0;
		undoStart = 0;
		undoEnd = 0;
	}
	
	// Save state
	if (action == 0) {
		/*** Hide player's movement information ***/
		int player_xMoving;
		int player_yMoving;
		char *player_path = NULL;
		
		if (blocks[0].GetPathLength() > 0) {
			player_path = new char[blocks[0].GetPathLength() + 1];
	
			strcpy(player_path, blocks[0].GetPath());
			blocks[0].SetPath("");
		}
		
		player_xMoving = blocks[0].GetXMoving();
		blocks[0].SetXMoving(0);

		player_yMoving = blocks[0].GetYMoving();
		blocks[0].SetYMoving(0);
		/***/

		
		if (undoEnd != undoStart && undoSlot == undoStart) {
			undoStart ++;
		}
		// Wrap around undoStart position
		if  (undoStart == static_cast<int>(maxUndo)) undoStart = 0;

		#ifdef DEBUG_UNDO
		printf("\nSaving state to undoSlot %d\n", undoSlot);
		#endif
		
		// Save the state of all the blocks
		for (uint i = 0; i < numBlocks; i++) {
			undoBlocks[undoSlot][i] = blocks[i];
		}

		// Save the state of all the telepads
		for (uint i = 0; i < numTelepads; i++) {
			undoTelepads[undoSlot][i] = telepads[i];
		}

		// Make undoEnd point to the most recent state
		undoEnd = undoSlot;
		
		// Increment undoSlot to use the next slot next time
		undoSlot ++;

		// Wrap around undoSlot position
		if (undoSlot == static_cast<int>(maxUndo)) {
			undoSlot = 0;
		}

		
	
		#ifdef DEBUG_UNDO
		printf("UndoStart = %d\n", undoStart);
		printf("UndoEnd = %d\n", undoEnd);
		printf("UndoSlot = %d\n", undoSlot);
		#endif

		/*** Restore player's movement information ***/
		if (player_path != NULL) {
			blocks[0].SetPath(player_path);
			delete [] player_path;
			player_path = NULL;
		}
		blocks[0].SetXMoving(player_xMoving);
		blocks[0].SetYMoving(player_yMoving);
		/***/
	}
	
	// Load state
	if (action == 1) {
		if (!(undoStart == undoEnd && undoSlot == undoEnd) || maxUndo == 1) {
			PlaySound(4);
			
			#ifdef DEBUG2
			printf("\nUndoing from slot %d\n", undoEnd);
			#endif
			
			// Restore the state of all the blocks
			for (uint i = 0; i < numBlocks; i++) {
				blocks[i] = undoBlocks[undoEnd][i];
			}

			// Restore the state of all the telepads
			for (uint i = 0; i < numTelepads; i++) {
				// Free memory being dereferenced by *current*
				// telepads which were in the process of
				// teleporting when the Undo button was pushed.
				telepads[i].DeInitTeleport(true);

				// Restore old telepad
				telepads[i] = undoTelepads[undoEnd][i];
				
				// Set old telepad state to non-teleporting state
				telepads[i].DeInitTeleport(false);
			}

			// Restore temporarily disabled block types (for teleportation animation)
			// to normal (positive) values
			for (uint i = 0; i < numBlocks; i++) {
				if (blocks[i].GetType() >= -99 && blocks[i].GetType() <= -1) {
					blocks[i].SetType(static_cast<char>( -(blocks[i].GetType() + 1) ));
				}
			}


			if (undoEnd != undoStart) {
				// Move undoEnd back, so it points to the now most recent undo state
				undoEnd --;
				if (undoEnd == -1) undoEnd = maxUndo - 1;
			}
			
			// Move undoSlot back, so we can overwrite this state next time
			undoSlot --;
			if (undoSlot == -1) undoSlot = maxUndo - 1;

			
			// Instantly move camera back to player
			if (option_cameraMode == 0) { // If the camera is set to "auto"
				SetCameraTargetBlock(0);
				CenterCamera(-1);
			}
			
			
			#ifdef DEBUG_UNDO
			printf("UndoEnd = %d\n", undoEnd);
			printf("UndoSlot = %d\n", undoSlot);
			#endif
		}
	
	}
}
