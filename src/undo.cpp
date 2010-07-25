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
		/******/

		
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

			// Clear all active speech bubbles
			ClearBubbles();
			// Change all players' face back to normal, in case they were currently talking
			for (uint i = 0; i < numPlayers; i++) {
				blocks[i].SetFace(0);
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
