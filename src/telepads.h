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


// Each telepad is actually a pair of two telepads
class telepad {
	public:
		// Constructor
		telepad():
			occupant1(-1),
			occupant2(-1),
			teleporting(false),
			ba(NULL),
			sourceSurf(NULL),
			destSurf(NULL),
			map(NULL)
			{};

		// Destructor
		~telepad()
			{
				DeInitTeleport(true);
			};

		/** Get **/
		int GetX1() const { return x1; };
		int GetY1() const { return y1; };
		int GetX2() const { return x2; };
		int GetY2() const { return y2; };
		char GetState() const { return state; };
		bool GetTeleporting() const { return teleporting; };

		int GetOccupant1(); // Returns which block is currently on the first telepad
		int GetOccupant2(); // Returns which block is currently on the second telepad

		SDL_Surface* GetSurface(bool animate); // Returns correct surface, based on state (in graphics.cpp)
	

		/** Set **/
		void SetX1(int xPos) { x1 = xPos; };
		void SetY1(int yPos) { y1 = yPos; };
		void SetX2(int xPos) { x2 = xPos; };
		void SetY2(int yPos) { y2 = yPos; };
		
		/** Other **/
		bool NeedsToTeleport();	// in physics.cpp
		void Teleport(); 	// in physics.cpp
		void DeInitTeleport(bool freePointers);	// in physics.cpp

	private:
		int x1, y1;	// First telepad
		int x2, y2;	// Second telepad
		char state;	// 0 = off, 1 = waiting to teleport, 2 = teleporting (for flashing light animation)
		int occupant1;	// Block on first telepad
		int occupant2;	// Block on second telepad
		bool occupant1Teleported; // Records if occupant1 teleported yet
		bool occupant2Teleported; // Records if occupant2 teleported yet
	
		/*** Variables used for teleportation animation ***/
		bool teleporting;	// If the telepad is currently in the process of teleporting something
		int *ba; // Array to keep track of which blocks are going to get teleported
		SDL_Surface *sourceSurf;
		SDL_Surface *destSurf;
		bool *map; // Keeps track of which squares have been teleported (false = hasn't been moved, true = has)
		int dX, dY; // Destination x and y
		int sX, sY; // Source x and y
		int sH; // source height
		uint numToMove; // How many blocks are going to get teleported
};



int telepad::GetOccupant1() {
	return BlockNumber(x1, y1 + 11, TILE_W, 1);
}

int telepad::GetOccupant2() {
	return BlockNumber(x2, y2 + 11, TILE_W, 1);
}



SDL_Surface* telepad::GetSurface(bool animate) {
	static uint t = 0;
	
	switch (state) {
		case 0:
			return telepadSurface[0];
			break;
		case 1:
			if (animate == false) return telepadSurface[1];
			
			if (SDL_GetTicks() < t + 500) {
				return telepadSurface[1];
			}
			else {
				if (SDL_GetTicks() >= t + 600) t = SDL_GetTicks();
				return telepadSurface[0];
			}
			
			break;
		case 2:
			if (animate == false) return telepadSurface[2];
			
			if (SDL_GetTicks() < t + 100) {
				return telepadSurface[2];
			}
			else {
				if (SDL_GetTicks() >= t + 200) t = SDL_GetTicks();
				return telepadSurface[0];
			}

			break;
		default:
			return NULL;
			break;
	}
}



bool telepad::NeedsToTeleport() {
	int b;
	bool doIt = false;

	state = 0;  // Default to off
	
	// Check if occupant1 is new
	b = GetOccupant1();
	if (b != occupant1) {
		occupant1 = b;
		if (occupant1 >= 0) doIt = true; // Try to teleport
	
		occupant1Teleported = false;
	}

	// Check if occupant2 is new
	b = GetOccupant2();
	if (b != occupant2) {
		occupant2 = b;
		if (occupant2 >= 0) doIt = true; // Try to teleport

		occupant2Teleported = false;
	}

	// Check if there is an occupant that hasn't teleported yet
	if (occupant1 >= 0 && occupant1Teleported == false) doIt = true;
	if (occupant2 >= 0 && occupant2Teleported == false) doIt = true;

	
	
	/** Contraints **/
	
	// The block must not be moving
	if (occupant1 >= 0) {
		if (blocks[occupant1].GetXMoving() != 0) doIt = false;
	}
	if (occupant2 >= 0) {
		if (blocks[occupant2].GetXMoving() != 0) doIt = false;
	}
	
	if (doIt) {
		state = 1;  // Waiting to teleport
	}

	// There must be one telepad free and one occupied
	if ( !((occupant1 == -1 && occupant2 >= 0) || (occupant1 >=0 && occupant2 == -1)) ) {
		doIt = false;
	}

	if (doIt == true) {
		// The "free" telepad must be clear of anything that may be in the way even
		// if they are not directly on the telepad (e.g. a falling
		// block or a piece of land directly above.
		if (occupant1 == -1) {
			if (BoxContents(x1, y1 - TELEPAD_H, TILE_W, TILE_H) != -1) {
				doIt = false;
			}
		}
		if (occupant2 == -1) {
			if (BoxContents(x2, y2 - TELEPAD_H, TILE_W, TILE_H) != -1) {
				doIt = false;
			}
		}
		
		// Don't let a player teleport if he's picking up a block
		if (occupant1 >= 0 && blocks[occupant1].GetType() >= 10) {
			if (playerBlock[occupant1] != -1) doIt = false;
		}
		if (occupant2 >= 0 && blocks[occupant2].GetType() >= 10) {
			if (playerBlock[occupant2] != -1) doIt = false;
		}
	}

	if (doIt) {
		return true;
	}
	
	return false;
}



// Fix telepads/blocks that were in the process of teleporting
// when their undo-state was saved; Restart the teleportation process.
void telepad::DeInitTeleport(bool freePointers) {
	teleporting = false;
	state = 0; // Off

	// Delete block array
	//if (ba != NULL) {
		if (freePointers) delete [] ba;
		ba = NULL;
	//}

	// Delete pixel map array
	//if (map != NULL) {
		if (freePointers) delete [] map;
		map = NULL;
	//}

	// Free the surfaces
	//if (sourceSurf != NULL) {
		if (freePointers) SDL_FreeSurface(sourceSurf);
		sourceSurf = NULL;
	//}
	
	//if (destSurf != NULL) {
		if (freePointers) SDL_FreeSurface(destSurf);
		destSurf = NULL;
	//}
}



void telepad::Teleport() {
	uint squareSize = 4; // How many pixels will make up each square that is moved

	/*** Initialization ***/
	if (teleporting == false) {
		teleporting = true;
		
		state = 2; // Teleporting
		
		PlaySound(3); // Play sound
		
		// Get bottom block number
		int b;
		b = (occupant1 >= 0 ? occupant1 : occupant2);

		/*** Determine destination info ***/
		
		int dB; // destination bottom
		if (occupant1 >= 0) {
			dX = x2;
			dB = (y2 + TILE_H - TELEPAD_H);
			dY = dB - blocks[b].GetH();
		}
		else {
			dX = x1;
			dB = (y1 + TILE_H - TELEPAD_H);
			dY = dB - blocks[b].GetH();
		}
		
		// Move camera to other telepad
		cameraTargetX = dX + (blocks[b].GetW() / 2);
		cameraTargetY = dY + (blocks[b].GetH() / 2);

		// Make block array (for teleportation animation)
		ba = new int[numBlocks];

		/*** Determine the height of the source load to teleport (i.e. find blocks on top of the block) ***/
		ba[0] = b; // The first block in the array is the bottom block (usually the player)
		sX = blocks[b].GetX();
		sY = blocks[b].GetY();
		sH = blocks[b].GetH();
		numToMove = 1;
		while (true) {
			// Find which block is on this block's head
			b = BlockNumber(blocks[b].GetX(), blocks[b].GetY() - 1, TILE_W, 1);
			
			// if it's a block, and if there's room for its height at the destination
			//if (b >= 0 && BoxContents(dX, dY - (sH + blocks[b].GetH()), TILE_W, blocks[b].GetH()) == -1) {
			if (b >= 0 && BoxContents(dX, dY - blocks[b].GetH(), TILE_W, blocks[b].GetH()) == -1) {
				sY = blocks[b].GetY();
				sH += blocks[b].GetH();
				dY -= blocks[b].GetH();
				ba[numToMove] = b;
				numToMove ++;
			}
			else {
				break;
			}
		}
		

		/*** Prepare to animate the teleportation ***/
		// Create source surface
		sourceSurf = MakeSurface(TILE_W, sH);
		
		int yPos = sH; // Keeps track of current y Position (bottom to top)
		int yDestPos = dB;
		for (uint i = 0; i < numToMove; i++) {
			yPos -= blocks[ba[i]].GetH();
			yDestPos -= blocks[ba[i]].GetH();

			// Blit images of the teleporting blocks to the source surface
			ApplySurface(0, yPos, blocks[ba[i]].GetSurface(), sourceSurf);
		
			// Make all teleporting blocks disabled so they wont be Rendered or have Physics applied
			blocks[ba[i]].SetType(static_cast<char>( -(blocks[ba[i]].GetType() + 1) ));

			// Move the blocks to their new positions
			blocks[ba[i]].SetX(dX);
			blocks[ba[i]].SetY(yDestPos);
		}

		// Update occupant information
		if (occupant1 >= 0) {
			occupant2 = occupant1;
			occupant2Teleported = true;
			occupant1 = -1;
		}
		else {
			occupant1 = occupant2;
			occupant1Teleported = true;
			occupant2 = -1;
		}

		// Create destination surface
		destSurf = MakeSurface(TILE_W, sH);
		
		map = new bool[(sH * TILE_W) / squareSize];
		uint pX, pY; // pixel offset coordinates
		uint col; // pixel color
		
		// Mark transparent pixels-squares as already moved, all others as not yet moved
		for (pY = 0; pY < sH / (squareSize / 2); pY++) {
			for (pX = 0; pX < TILE_W / (squareSize / 2); pX++) {
				
				// Note: Since this check skips pixels and checks only the top left
				// pixel of each square, this depends on block/player tiles being
				// drawn according to a grid with each "pixel" being squareSize pixels
				col = GetPixel(sourceSurf, pX * (squareSize / 2), pY * (squareSize / 2));
				
				if (col == SDL_MapRGB(sourceSurf->format, 0xff, 0x00, 0xff)) {
					map[(pY * (TILE_W / (squareSize / 2))) + pX] = true;
				}
				else {
					map[(pY * (TILE_W / (squareSize / 2))) + pX] = false;
				}
			}
		}
	}
	
	
	
	
	
	/*** Animation ***/
	if (teleporting == true) {
		uint pX, pY; // pixel offset coordinates	
		uint numPixels; // Will count how many pixels are finished
		uint col; // pixel color
		
		// Lock source and dest surfaces
		//if (LockSurface(sourceSurf) == false) return;
		//if (LockSurface(destSurf) == false) return;
		LockSurface(sourceSurf);
		LockSurface(destSurf);

		
		// Move multiple squares at a time, and always proportionate to the
		// load size (so it will always appear to be the same speed)
		for (uint i = 0; i < sH / squareSize; i++) {
			// Check if we've moved all the pixels over
			numPixels = 0;
			for (uint j = 0; j < (sH * TILE_W) / squareSize; j++) {
				if (map[j] == true) numPixels ++;
			}
			if (numPixels == (sH * TILE_W) / squareSize) break;
		
			while (true) {
				pX = (rand() % (TILE_W / (squareSize / 2)));
				pY = (rand() % (sH / (squareSize / 2)));
				
				// If this square has not been moved yet.
				if (map[(pY * (TILE_W / (squareSize / 2))) + pX] == false) {
					map[(pY * (TILE_W / (squareSize / 2))) + pX] = true;

					// Upscale the coordinates to fit onto the actual pixels (the top left pixel of the square we'll move)
					pX *= (squareSize / 2);
					pY *= (squareSize / 2);

					break;
				}
			}
		
			// Trasfer one square at a time (whose dimensions are (squareSize / 2) x (squareSize / 2))
			for (uint sqY = 0; sqY < squareSize / 2; sqY++) {
				for (uint sqX = 0; sqX < squareSize / 2; sqX++) {
					// Get pixel color from source
					col = GetPixel(sourceSurf, pX + sqX, pY + sqY);

					// Draw pixel to Destination
					PutPixel(destSurf, pX + sqX, pY + sqY, col);

					// Erase source pixel (with transparent color)
					PutPixel(sourceSurf, pX + sqX, pY + sqY, SDL_MapRGB(sourceSurf->format, 0xff, 0x00, 0xff));
				}
			}
		}
		
		// Unlock source and dest surfaces
		UnlockSurface(sourceSurf);
		UnlockSurface(destSurf);
		
		// Update source and dest surfaces
		SDL_UpdateRect(sourceSurf, 0, 0, 0, 0);
		SDL_UpdateRect(destSurf, 0, 0, 0, 0);
		
		// Blit source & dest surfaces to screenSurface
		ApplySurface(sX - cameraX, sY - cameraY, sourceSurf, screenSurface);
		ApplySurface(dX - cameraX, dY - cameraY, destSurf, screenSurface);
		
		// Check if the teleportation is done
		bool doneTeleporting = false;
		if (numPixels == (sH * TILE_W) / squareSize) doneTeleporting = true;
		
		/*** De-Initializtion ***/
		if (doneTeleporting) {
			// Make all teleporting blocks visible again (e.g. convert -1 back to 0, -2 back to 1)
			for (uint i = 0; i < numToMove; i++) {
				blocks[ba[i]].SetType(static_cast<char>( -(blocks[ba[i]].GetType() + 1) ));
			}

			DeInitTeleport(true);
		}
	}
}
