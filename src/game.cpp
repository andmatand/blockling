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


void CollectLevelGarbage() {
	#ifdef DEBUG
		printf("Collecting Level Garbage...\n");
	#endif

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

	// Clear all active speech bubbles
	ClearBubbles();
}





int Game() {

	int returnVal = 0;
	uint i, j;
	int x, b;
	//std::string tempPath;
	char s[11];
	char *buf1 = NULL;
	char *buf2 = NULL;
	bool pushedKey, triedKey;
	char previousKey;
	bool quitGame = false;
	bool restartLevel = false;
	bool showedPushHint = false;
	uint oldLevel = currentLevel;
	uint wonLevelTimer = 0;

	cameraXVel = 0;
	cameraYVel = 0;
	stickyPlayer = false;

	// Replay variables
	bool recordingReplay = false;
	showingReplay = false;
	//int currentReplayKey;
	menu *replayMenu = NULL;
	uint frameNumber; // counts frames for doing frameskipping for fast replays
	char replayTempFile[256]; // Will hold filenames for replay temp files
	
	// Pointer for array of pointers replay objects
	replay **theReplays = NULL;

	/*** Loop to advance to next level ***/
	while(quitGame == false) {
		wonLevel = 0;
		levelTime = 0;
		levelTimeRunning = false;
		physicsStarted = false;
		bool movementStarted = false;

		// Don't sync the real undo memory size with the
		// (possibly changed) option if we are doing a replay
		// which needs to keep the same conditions as the
		// original playthrough.
		if (showingReplay == false) {
			maxUndo = option_undoSize;
		}

		// Load Level without a menu (e.g. from in-game "Next Level" or "Restart Level")
		if (stickyPlayer || showingReplay || currentLevel != oldLevel || restartLevel) {
			// Store the error message returned by LoadLevel
			buf1 = LoadLevel(currentLevel);

			// If there was an error message
			if (buf1 != NULL) {
				// Free the heap memory taken up by the error message
				delete [] buf1;
				buf1 = NULL;

				// Go to the SelectLevelMenu
				selectingLevel = true;
			}
		}
		else {
			selectingLevel = true;
		}

		if (selectingLevel) {
			stickyPlayer = false;
			// Show the Level-Selection Menu
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
	
		// Reset keys
		for (i = 0; i < NUM_GAME_KEYS; i++) {
			gameKeys[i].on = 0;
		}
		for (i = 0; i < numPlayers; i++) {
			for (uint j = 0; j < NUM_PLAYER_KEYS; j++) {
				playerKeys[(i * NUM_PLAYER_KEYS) + j].on = 0;
			}
		}
		previousKey = -1;

		/*** Initialze replay ***/
		recordingReplay = (option_replayOn ? true : false);	
	
		if (showingReplay) {
			// Initialize the replay files for reading
			for (i = 0; i < numPlayers; i++) {
				theReplays[i]->InitRead();
			}
			
			// Disable recording if we're playing!
			recordingReplay = false;
			
			// Create the speed selection menu
			replayMenu = new menu(1);
			replayMenu->SetTitle("");
			replayMenu->Move(SCREEN_W / 2, 0);
			
			frameNumber = 0;
		}
		else if (recordingReplay) {
			// Create an array of replay object pointers (one replay for each player)
			theReplays = new replay*[numPlayers];
			for (i = 0; i < numPlayers; i++) {
				// Loop to try sequential numbers at the end of the filename
				for (uint j = i; j < (2 ^ sizeof(j)); j++) {
					// Get full filename for this replay temp file
					sprintf(replayTempFile, "%sblockling-replay-%d.tmp", TEMP_PATH, j);

					// If this filename doesn't exist, we can exit the loop
					FILE *f = NULL;
					f = fopen(replayTempFile, "rt");
					if (f == NULL) {
						break;
					}
					else {
						fclose(f);
					}	
				}

				// Create the replay object
				theReplays[i] = new replay(i, replayTempFile, 100);  // A buffer of 100 takes about 500 bytes of memory

				// Initialize the replay object for writing
				theReplays[i]->InitWrite();
			}
		}

		// Reset tutorial state if this is level 0
		if (currentLevel == 0 && option_levelSet == 0) {
			TutorialSpeech(true);
		}
		
		


		/******* GAME LOOP *******/
		while (quitGame == false && selectingLevel == false &&
			restartLevel == false)
		{
			// Clear Speech Triggers
			ClearSpeechTriggers();
			
			// Handle Window Close
			if (GameInput(showingReplay ? 1 : 0) == -2) {
				quitGame = true;
				returnVal = -2;
				break;
			}
						
			// Handle Restart Level key
			if (gameKeys[5].on == 1) {
				showingReplay = false;
				stickyPlayer = false;
				restartLevel = true;
				break;
			}
			
			// Handle Help key
			if (gameKeys[7].on == 1) {
				if (HelpMenu(true) == -2) {
					quitGame = true;
					returnVal = -2;
					break;
				}
			}

			// Show tutorial on level 0
			if (currentLevel == 0 && option_levelSet == 0 &&
				physicsStarted)
			{
				TutorialSpeech(false);
			}

			// Show PUSH hint on level 5
			if (showedPushHint == false && currentLevel == 5 &&
				option_levelSet == 0 && physicsStarted)
			{
				showedPushHint = true;
				if (option_helpSpeech) blocks[0].SetDir(2);
				HelpSpeak(0, "Oh!");
				HelpSpeak(0, "I just remembered something...",
					true);

				buf1 = new char[45 + strlen(KeyName(
					option_playerKeys[4].sym))];
				sprintf(buf1,
					"If you press %s, I can push blocks!",
					KeyName(option_playerKeys[4].sym));
				HelpSpeak(0, buf1, true, 1);
				delete [] buf1;
				buf1 = NULL;

				HelpSpeak(0, "", true);
				switch (rand() % 2) {
					case 0:
						HelpSpeak(0, "I don't know why "
							"I just thought of "
							"that.", true);
						break;
					case 1:
						HelpSpeak(0, "Anyway...", true);
						break;
				}
			}

			/** Set speed of replay ****/
			/*
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
			*/

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
							quitGame = true;
							break;
					}
					
					// If "Next Level" was chosen, break from main loop
					if (currentLevel != i) break;
				}
				else { // Normal pause menu
					switch (PauseMenu()) {
						case 0: // Resume Game
							break;
						case 3: // Restart Level
							showingReplay = false;
							stickyPlayer = false;
							restartLevel = true;
							break;
						case 4: // Change Level
							showingReplay = false;
							stickyPlayer = false;
							selectingLevel = true;
							break;
						case -1: // Esc
							break; // Resume the game
						case -2: // Close window
							returnVal = -2;
						case 5: // Quit Game
							showingReplay = false;
							quitGame = true;
							break;
					}
				}
				
				// No more Esc
				//gameKeys[0].on = 0;
				
				// Clear any keys that may have been held when Esc was pressed
				TurnOffAllKeys();
				
				// Make the time spent in the menu undetectable to the timer incrementer
				levelTimeTick = SDL_GetTicks() - j;
			}
			
			
			/*** Handle Player Movement ***/
			for (i = 0; i < numPlayers; i++) {
				// This will forbid multiple actions from happening in the same frame
				pushedKey = false;
				triedKey = false;
				
				if (showingReplay) {
					// Turn undo key off
					gameKeys[4].on = 0;

					// If the next key in the replay is Undo, press that
					// here (not inside the normal keypressing scope which
					// requires that the player be on solid ground)
					if (theReplays[i]->GetNextKey() == 5) {
						theReplays[i]->PushNextKey();
					}
				}
				
				// Undo
				if (i == 0 && gameKeys[4].on > 0) {
					if (recordingReplay) theReplays[i]->SaveKey(5); // Save the keypress in the replay
					
					Undo(1);
					pushedKey = true;
				}

				// If the player is not already moving
				if (blocks[i].GetPathLength() == 0 &&
					blocks[i].GetXMoving() == 0 &&
					blocks[i].GetYMoving() == 0 &&
					// and is on solid ground
					blocks[i].OnSolidGround() &&
					// and a key hasn't been pushed yet
					!pushedKey &&
					// and the block is not disabled
					blocks[i].GetType() >= 0 &&
					// and the player is still playing (hasn't won)
					blocks[i].GetWon() == 0 &&
					// and the player is not facing the camera with locked movement
					blocks[i].GetDir() < 3)
				{
					
					/*** Perform next keypress from replay file ***/
					if (showingReplay) {
						// Turn all playerKeys off (for current player)
						for (uint j = 0; j < NUM_PLAYER_KEYS; j++) {
							playerKeys[(i * NUM_PLAYER_KEYS) + j].on = 0;
						}
						
						if (physicsStarted) {
							//currentReplayKey = theReplays[i]->GetNextKey();
							//printf("pushing key %d\n", currentReplayKey);
							theReplays[i]->PushNextKey();
							//printf("currentReplayKey = %d\n", currentReplayKey);
						}
					}
					
					
					// Enter (push a block)
					if (
						pushedKey == false &&
						playerBlock[i] == -1 &&
						playerKeys[(i * NUM_PLAYER_KEYS) + 4].on > 0 &&
						option_levelSet != 1) // For retro BLOCKMAN-1 mode, disable pushing
					{
						
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - 1;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), 1, 1); // First try to push from bottom of player
						if (b < 0) b = BlockNumber(x, blocks[i].GetY(), 1, 1); // If that didn't work, push from top
						
						if (b >= 0 && blocks[b].OnSolidGround()) {
							/** Check if there is room for it to move ****/
							if (blocks[i].GetDir() == 0) {	// Facing left
								x = blocks[b].GetX() - 1;
							}
							else { 				// Facing right
								x = blocks[b].GetX() + blocks[b].GetW();
							}
							// (re-use the x variable)
							x = BoxContents(x, blocks[b].GetY() + (blocks[b].GetH() - 1), 1, 1);
							
							// If the space next to the block is empty or if this player is strong
							if (x == -1 || blocks[i].GetStrong() > 0) {
								PlaySound(2); // Play sound

								// Save the keypress in the replay
								if (recordingReplay) theReplays[i]->SaveKey(4);

								// If this is player 0
								if (i == 0) {
									Undo(0); // Save Undo state
									previousKey = 4;
								}

								if (blocks[i].GetDir() == 0) {
									blocks[i].SetXMoving(-TILE_W);
									blocks[b].SetXMoving(-TILE_W);
								}
								else {
									blocks[i].SetXMoving(TILE_W);
									blocks[b].SetXMoving(TILE_W);
								}

								// If player is strong, make this block strong for now
								if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1);

								playerBlock[i] = b; // Player can't move until this block stops moving
								pushedKey = true; // Now player can't push other buttons this frame
							}
							else {
								triedKey = true;
								
								// Possibly do speech if this is the first player
								if (i == 0 && previousKey != 4) {
									pushedKey = true;
									previousKey = 4;
									
									// If there's another block blocking it
									/*
									if (x >= 0) {
										switch (rand() % 2) {
											case 0:
												HelpSpeak(0, "I'm not strong enough to push more than one block at a time.");
												break;
											case 1:
												HelpSpeak(0, "They're too heavy for me to push.");
												break;
										}
									}
									else {
									*/
										switch (rand() % 3) {
											case 0:
												HelpSpeak(0, "I can't move it.");
												break;
											case 1:
												HelpSpeak(0, "It won't budge.");
												break;
											case 2:
												HelpSpeak(0, "It won't move.");
												break;
										}
									/*
									}
									*/
								}
							}
						}
					}
					
					// Left
					if (pushedKey == false && playerKeys[(i * NUM_PLAYER_KEYS) + 0].on > 0) {
						// For retro BLOCKMAN-1 mode, disable turning "in place"
						j = 1; // Autoclimb = on
						if (option_levelSet == 1) {
							if (blocks[i].GetDir() == 1 && playerBlock[i] == -1) {
								// Flag that the player should not climb
								j = 0;

								blocks[i].SetDir(0);
								pushedKey = true;
							}
						}

						if (blocks[i].GetDir() == 0) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								// Try to set block's path to climb/walk left
								blocks[i].Climb(0, j);
								
								// If the player is actually going to move
								if (blocks[i].GetPathLength() > 0 || blocks[i].GetXMoving() != 0) {
									// If this is player 0
									if (i == 0) {
										// Save Undo state
										Undo(0);
									}

									// Save the keypress in the replay
									if (recordingReplay) theReplays[i]->SaveKey(0);
									
									// Now player can't push other buttons this frame
									pushedKey = true;
								}
							}
						}
						else if (option_levelSet != 1) {
							// Save the keypress in the replay
							if (recordingReplay) theReplays[i]->SaveKey(0);
							
							blocks[i].SetDir(0);
							pushedKey = true;
						}
					}

					// Right
					if (pushedKey == false && playerKeys[(i * NUM_PLAYER_KEYS) + 1].on > 0) {
						// For retro BLOCKMAN-1 mode, disable turning "in place"
						j = 1; // Autoclimb = on
						if (option_levelSet == 1) {
							if (blocks[i].GetDir() == 0 && playerBlock[i] == -1) {
								// Flag that the player should not climb
								j = 0;

								blocks[i].SetDir(1);
								pushedKey = true;
							}
						}

						if (blocks[i].GetDir() == 1) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								// Try to set block's path to climb/walk right
								blocks[i].Climb(1, j);

								// If the player is actually going to move
								if (blocks[i].GetPathLength() > 0 || blocks[i].GetXMoving() != 0) {
									// If this is player 0
									if (i == 0) {
										Undo(0); // Save Undo state
									}
									
									if (recordingReplay) theReplays[i]->SaveKey(1); // Save the keypress in the replay	

									pushedKey = true; // Now player can't push other buttons this frame
								}
							}
						}
						else if (option_levelSet != 1) {
							// Save the keypress in the replay
							if (recordingReplay) theReplays[i]->SaveKey(1);

							blocks[i].SetDir(1);
							
							// Now player can't push other buttons this frame
							pushedKey = true;
						}
					}
		
					// Down (set down block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 3].on > 0) {
						// What's on top of player's head?
						b = BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, TILE_W, 1);

						// If it's a block, set its path to be set down.
						if (b >= 0 && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
							PlaySound(1); // Play sound
							if (recordingReplay) theReplays[i]->SaveKey(3); // Save the keypress in the replay
							
							// If this is player 0
							if (i == 0) {
								Undo(0); // Save Undo state
							}
							
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
					if (pushedKey == false &&
							playerBlock[i] == -1 &&
							playerKeys[(i * NUM_PLAYER_KEYS) + 2].on > 0)
					{
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {  // Facing left
							x = blocks[i].GetX() - 1;
						}
						else {                          // Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY(), 1, 1);
						
						// If it's a block, make it climb up onto the player =)
						if (b >= 0 && blocks[b].GetType() >= 0) {
							if (recordingReplay) theReplays[i]->SaveKey(2); // Save the keypress in the replay
							
							// If this is player 0
							if (i == 0) {
								Undo(0); // Save Undo state
							}
							
							// If player is strong, make this block strong for now
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1);
							
							// Try to make the block climb onto the player's head
							// (re-use the x variable for this)
							x = blocks[b].Climb(static_cast<char>((blocks[i].GetDir() == 0) ? 1 : 0), true);
							
							// If there was room for the block to climb
							if (x == -1) {
								PlaySound(0); // Play sound
								
								playerBlock[i] = b; // Player can't move until this block stops moving
							}
							// If there was no room for the block to climb
							else {
								// Possibly do speech if this is player 0
								if (i == 0) {
									switch (x) {
										case 0:
											switch (rand() % 2) {
												case 0:
													SpeechTrigger(0, "I can't pick it up.  There's a block on top of it.", 1, 2, 4);
													break;
												case 1:
													SpeechTrigger(0, "I'm not strong enough to pick up that block with another one on top of it.", 1, 2, 4);
													break;
											}
											break;
										case -2:
											buf1 = new char[14];
											sprintf(buf1, "piece of land");
											break;
										case -3:
											buf1 = new char[6];
											sprintf(buf1, "spike");
											break;
										case -4:
											buf1 = new char[8];
											sprintf(buf1, "telepad");
											break;
									}
									
									if (buf1 != NULL) {
										buf2 = new char[64];
										switch (rand() % 2) {
											case 0:
												sprintf(buf2, "I can't move it up.  There's a %s in the way.", buf1);
												break;
											case 1:
												sprintf(buf2, "I can't.  There's a %s blocking it.", buf1);
												break;
										}
										HelpSpeak(0, buf2);
										
										delete [] buf1; buf1 = NULL;
										delete [] buf2; buf2 = NULL;
									}
								}
							}
							
							pushedKey = true; // Now player can't push other buttons this frame
						}
						else {
							// Check if there is an ungrabbable block
							if (BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), 1, 1) >= 0) {
								HelpSpeak(0, "I can't get a grip on the bottom of it.");
								if (rand() % 2) HelpSpeak(0, "My arms aren't very long, after all.", 1);
							}
						}
					}

					// Mark the moment when any player
					// moves for the first time (to start
					// the other repays recording sleeps)
					if (pushedKey && movementStarted == false) movementStarted = true;
					
					if (pushedKey == false && recordingReplay && movementStarted && wonLevel < 2) {
						theReplays[i]->SaveKey(-1); // Save the non-keypress (sleep) in the replay
					}
					
					// Prevent bug where player moves in very first frame before player's
					// face is set back to normal in Render() (from zing/stickyPlayer)
					if (physicsStarted == false && pushedKey == true) {
						blocks[0].SetFace(0); // normal face
					}
					
					// Turn off previousKey
					if (i == 0 && playerBlock[i] == -1 && pushedKey == false && triedKey == false) {
						previousKey = -1;
					}
				}
				

				// Check if player is waiting for a block he's acted upon to finish moving
				if (playerBlock[i] != -1) {
					// If it's not moving (along a path) anymore
					if (blocks[playerBlock[i]].GetPathLength() == 0 &&
						blocks[playerBlock[i]].GetXMoving() == 0 &&
						blocks[playerBlock[i]].GetYMoving() == 0)
					{
						// Not strong anymore
						blocks[playerBlock[i]].SetStrong(0);
						
						// No longer wait for it.
						playerBlock[i] = -1;
					}
				}

				// Start the timer when the first movement is made by the human player
				if (levelTimeRunning == false && i == 0 && pushedKey) {
					levelTimeRunning = true;
					levelTimeTick = SDL_GetTicks();
					// Also start physics if they haven't already
					physicsStarted = true; 
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
				// If player is not floating and there is
				// no block on his head, and he is not on
				// a spike, make him rise
				for (i = 0; i < numPlayers; i++) {
					if (blocks[i].GetType() >= 10 &&
						blocks[i].GetH() < TILE_H &&
						BlockNumber(blocks[i].GetX(),
							blocks[i].GetY() - 1,
							blocks[i].GetW(),
							1) < 0 &&
						BoxContents(blocks[i].GetX(),
							blocks[i].GetY() + blocks[i].GetH(),
							blocks[i].GetW(),
							1) != -3)
					{
						// Save old y position
						b = blocks[i].GetY();

						// Set the block to move up
						blocks[i].SetYMoving(-blockYSpeed);

						if (blocks[i].GetYMoving() < -2) blocks[i].SetYMoving(-2);
						
						// Temporarily disable xMoving
						x = blocks[i].GetXMoving(); // Save it
						blocks[i].SetXMoving(0); // Set it to 0
						
						// Try to move the block up
						blocks[i].Physics();
						
						// Restore xMoving
						blocks[i].SetXMoving(x);
						
						// If it moved up, increase the height by the amount moved
						if (blocks[i].GetY() < b) {
							blocks[i].SetH(blocks[i].GetH() + (b - blocks[i].GetY()));
						}
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
					blocks[i].SetDir(3); // Face the camera and lock movement
					if (blocks[i].GetH() > TILE_H - 2) blocks[i].SetH(blocks[i].GetH() - 2);
					
					// If this is player 0
					if (i == 0) {
						// Give a helpful hint about undoing
						switch (rand() % 2) {
							case 0:
								SpeechTrigger(0, "Ow.", FPS, 0, 2);
								break;
							case 1:
								SpeechTrigger(0, "Ouch.", FPS, 0, 2);
								break;
						}
						if (maxUndo >= 1) {
							char temp[80];
							sprintf(temp, "Umm pressing %s to undo would be really helpful right about now...", KeyName(gameKeys[4].sym));
							SpeechTrigger(0, temp, FPS * 4, 1, 3);
						}
					}
				}
				
				// If the player just finished walking into the exit (with the door open)
				if (blocks[i].GetWon() == 2 && blocks[i].GetX() == exitX && blocks[i].GetY() - blocks[i].GetYOffset() == exitY) {
					blocks[i].SetWon(3);
					// If this is player 0
					if (i == 0) {
						PlaySound(8);
						
						blocks[i].SetFace(3); // happy mouth
						blocks[i].SetDir(3); // Face the camera & lock movement

						// Stop any active speech bubbles
						ClearBubbles();

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
						Render(RENDER_UPDATESCREEN | RENDER_MOVECAMERA | RENDER_ANIMATE | RENDER_BG);
					}
					
					blocks[i].SetWon(2); // player can now finish walking into the door
				}
			}

			/** Timer **/
			// Increment the timer
			if (levelTimeRunning && wonLevel == 0
				&& SDL_GetTicks() >= levelTimeTick + 1000)
			{
				if (showingReplay == false ||
					option_replaySpeed == 1)
				{
					levelTime ++;
					levelTimeTick = SDL_GetTicks();
				}
			}
	

			// Process camera movements only on real frames
			if (showingReplay == false || frameNumber == 0) {
				MoveCamera();
			}

			// Draw the Background (underneath teleportation animation)
			DrawBackground();

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
						replayMenu->SetLeftArrow(0, 0);
						
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

						if (frameNumber >= 8) {
							frameNumber = 0;
						}
						break;
					case 5: // 16x
						sprintf(s, "16x");
						replayMenu->SetRightArrow(0, 0);

						if (frameNumber >= 16) {
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

		
			// Do auto camera centering every frame
			CenterCamera(0);

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
				// If the telepad is waiting to teleport (flashing red)
				else if (telepads[i].GetState() == 1) {
					if (telepads[i].GetOccupant1() == 0 || telepads[i].GetOccupant2() == 0) {
						SpeechTrigger(0, "I think there's something blocking the other telepad.", FPS * 2, 1, 1);
					}
				}
			}
			
			// Render
			if (showingReplay == false || frameNumber == 0) {
				Render(RENDER_UPDATESCREEN | RENDER_ANIMATE);
			}
			else {
				// Do stuff that would normally be done
				// within render that still needs to be
				// done every loop
				DrawBubbles(true);
			}

			/*** Stuff for when the player reached the exit ***/
			if (wonLevel == 3 && SDL_GetTicks() > wonLevelTimer +
				1000)
			{
				// "Unlock" the next level if it isn't already
				if (option_levelSet == 0 &&
					currentLevel + 1 > option_levelMax0)
				{
					option_levelMax0 = currentLevel + 1;
				} else if (option_levelSet == 1 &&
					currentLevel + 1 > option_levelMax1)
				{
					option_levelMax1 = currentLevel + 1;
				}
			
				if ((recordingReplay && option_replayOn) ||
					showingReplay)
				{
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
						case 2: // Change Level
							showingReplay = false;
							stickyPlayer = false;
							selectingLevel = true;
							break;
						case 3: // Quit Game
							quitGame = true;
							break;
						case -2: // Close window
							quitGame = true;
							returnVal = -2;
							break;
					}
				}

				if (selectingLevel == false) {
					wonLevel = 4;
					if (showingReplay == false) {
						currentLevel ++;
					}
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
				Render(RENDER_UPDATESCREEN | RENDER_ANIMATE | RENDER_BG);
			}
		}
		
		// Collect garbage for global object pointers
		CollectLevelGarbage();

		// Finalize the replay file (post-record)
		if (recordingReplay) {
			for (i = 0; i < numPlayers; i++) {
				theReplays[i]->DeInitWrite();
			}
		}

		// Collect garbage for replay (post-play or quitting the game)
		if (theReplays != NULL && (showingReplay == false || quitGame)) {
			for (i = 0; i < numPlayers; i++) {
				if (remove(theReplays[i]->GetFilename()) != 0) {
					fprintf(stderr, "File error: Could not delete %s\n", replayTempFile);
				}
			}
			
			delete [] theReplays;
			theReplays = NULL;
			
			delete replayMenu;
			replayMenu = NULL;
		}

	} // Back to top of while loop to load next level
	
	return returnVal;
}



FILE *OpenLevel(int level) {
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

	char filename[
		strlen(DATA_PATH) +
		strlen(LEVEL_PATH) +
		strlen(levelSet) + 1 +
		strlen(levelFile) + 1];

	if (level < 0) {
		sprintf(filename, "%s%s000", DATA_PATH, LEVEL_PATH);
	} else {
		sprintf(filename,
			"%s%s%s/%s",
			DATA_PATH,
			LEVEL_PATH,
			levelSet,
			levelFile);
	}

	#ifdef DEBUG
	printf("\nOpening level %d...\n", level);
	printf("filename: \"%s\"\n", filename);
	#endif

	return fopen(filename, "rt");
}


char *LoadLevel(int level) {
	char *errorMsg = new char[256]; // For holding error messages
	errorMsg[0] = '\0';
	char temp[256]; // For assembling strings of error messages
	temp[0] = '\0';
	
	// Free memory used by previous level
	CollectLevelGarbage();
	
	// Clear all active speech triggers
	for (uint i = 0; i < MAX_TRIGGERS; i++) {
		triggers[i].SetID(-1);
	}
	
	// Zero all game object count variables
	numBlocks = 0;
	numPlayers = 0;
	numBricks = 0;
	numTelepads = 0;
	numTorches = 0;
	numSpikes = 0;
	
	int numFirstPlayers = 0;

	bool syntaxError = false;
	
	FILE *f;
	f = OpenLevel(level);
	if (f == NULL) {
		// Signal to calling function that the file could not be opened
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
	
	bool validChar;
	bool charOnThisLine = false; // begin by assuming there is not a valid character on the first line
	bool restOfLineIsComment = false;
	int numExits = 0;
	while (!feof(f)) {
		c = fgetc(f);

		validChar = false; // being by assuming the current char is not valid

		if (restOfLineIsComment == false) {
			switch (c) {
				case '#': // Comment
					restOfLineIsComment = true;
					break;
				case '.':
					validChar = true;
					break;
				case '@': // First Player
					validChar = true;
					numFirstPlayers ++;
					numPlayers ++;
					numBlocks ++;
					break;
				case 'A': // Unintelligent NPC
					validChar = true;
					numPlayers ++;
					numBlocks ++;
					break;
				case '*': // exit
					validChar = true;
					numExits ++;
					break;
				case '^':
					validChar = true;
					numSpikes ++;
					break;
				case 'T':
					validChar = true;
					numTorches ++;
				case '0':
				case '1':
				case '2':
					validChar = true;
					numBricks ++;
					break;
				case 'x':
				case 'X':
					validChar = true;
					numBlocks ++;
					break;
				default:
					break;
			}
		}

		if (restOfLineIsComment == false) {
			// Check for lowercase a - z (telepad pairs)
			if (c >= 97 && c <= 122) {
				validChar = true;
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
		}

		if (validChar) {
			charOnThisLine = true;
			x += TILE_W;
			if (x > width) width = x;
		}
	
		// New Line (LF)
		if (c == 10) {
			// If there was a valid character on the previous line, increment the height
			if (charOnThisLine) height += TILE_H;

			charOnThisLine = false; // begin by assuming there is not a valid character on the next line
			restOfLineIsComment = false;
			x = 0;
		}
	}
	
	
	/** Check Level for Errors ******/
	
	// TODO: Check for too many objects & things that would cause buffer overflow
	/*
	if (numBlocks > sizeof(int) * 8) {
		sprintf(temp, "There are too many blocks!");
		strcat(errorMsg, temp);
		syntaxError = true;
	}
	*/

	// Check for telepad errors
	for (uint i = 0; i < numTelepads; i++) {
		if (telepadMates[i] != 1) {
			sprintf(temp, "-Telepad '%c' should have 1 mate, but\n it has %d mates.\n", telepadLetter[i], telepadMates[i]);
			strcat(errorMsg, temp);
			syntaxError = true;
			break; // Only report one telepad error at a time, so the string doesn't overflow
		}
	}

	// Check for missing exit
	if (numExits != 1) {
		sprintf(temp, "-There must be exactly 1 level exit,\n but there are %d.\n", numExits);
		strcat(errorMsg, temp);
		syntaxError = true;
	}
	
	// Check for != 1 first player
	if (numFirstPlayers != 1) {
		sprintf(temp, "-There must be exactly one first\n player (@), but there are %d.\n", numFirstPlayers);
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

	charOnThisLine = false; // begin by assuming there is not a valid character on the first line
	rewind(f); // Go back to the beginning of the file
	while (!feof(f)) {
		c = fgetc(f);
		
		validChar = false; // being by assuming the current char is not valid

		if (restOfLineIsComment == false) {
			switch (c) {
				case '#': // Comment
					restOfLineIsComment = true;
					break;
				case '.':
					validChar = true;
					break;
				case '@': // first player
					validChar = true;
					blocks[0].SetX(x);
					blocks[0].SetY(y);
					blocks[0].SetType(10);
					break;
				case 'A': // Unintelligent NPC
					validChar = true;
					blocks[1 + numPlayers].SetX(x);
					blocks[1 + numPlayers].SetY(y);
					blocks[1 + numPlayers].SetType(11);
					numPlayers ++;
					break;
				case '*': // exit
					validChar = true;
					exitX = x;
					exitY = y;
					break;
				case '^':
					validChar = true;
					spikes[numSpikes].SetX(x);
					spikes[numSpikes].SetY(y);
					numSpikes ++;
					break;
				case 'T': // torch
					validChar = true;
					torches[numTorches].SetX(x);
					torches[numTorches].SetY(y);
					numTorches ++;
					bricks[numBricks].SetType(0); // Make sure the brick
					                              //we're about to make is a "wall" brick
					// Proceed down to next case (put a brick behind the torch)
				case '0': // normal brick (type automatically selected)
					validChar = true;
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					numBricks ++;
					break;
				case '1': // wall brick (manual override)
					validChar = true;
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					bricks[numBricks].SetType(0);
					numBricks ++;
					break;
				case '2': // grass brick (manual override)
					validChar = true;
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					bricks[numBricks].SetType(-2);
					numBricks ++;
					break;
				case 'X': // block
					validChar = true;
					blocks[numBlocks].SetX(x);
					blocks[numBlocks].SetY(y);
					numBlocks ++;
					break;
				default:
					break;
			}
		}

		if (restOfLineIsComment == false) {
			// Check for lowercase a - z (telepad pairs)
			if (c >= 97 && c <= 122) {
				validChar = true;
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
		}

		if (validChar) {
			charOnThisLine = true;
			x += TILE_W;
		}


		// New Line (LF)
		if (c == 10) {
			restOfLineIsComment = false;

			// If there was a character on the previous line, increment the y
			if (charOnThisLine) y += TILE_H;

			charOnThisLine = false; // begin by assuming there is not a valid character on the next line
			
			x = -(width / 2);
			x += (abs(x) % TILE_W); // Align to grid
		}
	}
	fclose(f);
	
	// Add the first player to numPlayers
	numPlayers ++;

	/** Change brick types based on their placement next to other bricks ****/
	int leftBrick, rightBrick, topBrick, bottomBrick;
	// Make vertical bricks into "wall" bricks
	for (uint i = 0; i < numBricks; i++) {
		// Only change bricks that have not yet been set
		if (bricks[i].GetType() != -1) continue;
		
		/** Find surrounding bricks, and ignore them if they are of type 0 (wall) ****/
		
		leftBrick = BrickNumber(bricks[i].GetX() - TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
		if (leftBrick > -1) {
			if (bricks[leftBrick].GetType() == 0) leftBrick = -1; // Pretend like this brick isn't here
		}
		
		rightBrick = BrickNumber(bricks[i].GetX() + TILE_W, bricks[i].GetY(), TILE_W, TILE_H);
		if (rightBrick > -1) {
			if (bricks[rightBrick].GetType() == 0) rightBrick = -1; // Pretend like this brick isn't here
		}
		
		topBrick = BrickNumber(bricks[i].GetX(), bricks[i].GetY() - TILE_H, TILE_W, TILE_H);
		//if (bricks[topBrick].GetType() == 0) topBrick = -1; // Pretend like this brick isn't here
		
		bottomBrick = BrickNumber(bricks[i].GetX(), bricks[i].GetY() + TILE_H, TILE_W, TILE_H);
		//if (bricks[bottomBrick].GetType() == 0) bottomBrick = -1; // Pretend like this brick isn't here
		
		/** If (there is a brick above this brick
		    OR there is a brick below this brick)
		    AND there is no brick to the left
		    AND there is no brick to the right **/
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
	
	// Seed the random number generator with the same number every
	// time, so that random NPC behavior will be consistent every
	// time the level is played, and the game will thus remain a
	// "puzzle" game, and not a game of chance.
	srand(47);
	
	delete [] errorMsg;
	return NULL;
}



void TutorialSpeech(bool reset) {
	// This function is called in the game loop only during level 0
	// It makes the player say something, wait for a certain game
	// state, then say the next thing.
	/*
	   If I come to a block, I'll climb it myself.
	   I can't jump any higher than that.
	   My legs *are* pretty short...
	 */
	if (option_helpSpeech == false) return;

	static uchar step = 0; // Remembers which step in the tutorial we are on
	char temp[64]; // for holding strings

	if (reset) step = 0;

	switch (step) {
		case 0:
			blocks[0].SetDir(2);
			Speak(0, "Um.");
			Speak(0, "", true);
			Speak(0, "Excuse me...", true);
			Speak(0, "Can you help me get to that door over there?",
				true, 0);

			sprintf(temp, "Push %s to make me go left!",
				KeyName(option_playerKeys[0].sym));
			Speak(0, temp, true);

			// Go to the next step
			step++;

			break;
		case 1:
			// If the player is standing to the right of the block
			if (blocks[0].GetX() == blocks[1].GetX() +
				blocks[1].GetW())
			{
				Speak(0, "Hey!");
				Speak(0, "A block!", true);

				sprintf(temp, "Push %s and I'll pick it up!",
					KeyName(option_playerKeys[2].sym));
				Speak(0, temp, true);

				step++;
			}
			break;
		case 2:
			// If the player is holding the block
			if (blocks[1].GetX() == blocks[0].GetX() && (blocks[1].GetY() + blocks[1].GetH()) == blocks[0].GetY()) {
				Speak(0, "Ugh!");

				step++;
			}

			break;
		case 3:
			// Wait a second before saying this:
			SpeechTrigger(0, "I think I should take it toward the "
			                 "door...", 45, 1, 100);

			if (blocks[0].GetX() == exitX + (TILE_W * 3)) {
				step++;
			}
			break;
		case 4:
			Speak(0, "Okay.");
			sprintf(temp, "Push %s to make me set it down.",
				KeyName(option_playerKeys[3].sym));
			Speak(0, temp, true);
			step++;

			break;
		case 5:
			// If the player set the block down in the right place
			if (blocks[1].GetX() == exitX + (TILE_W * 2) &&
				blocks[1].GetY() == exitY + (TILE_H * 2))
			{
				Speak(0, "If I walk toward a block, I'll "
				         "climb onto it.");
				step++;
			}

			break;
	}
}




