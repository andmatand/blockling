/*
 *   physics.cpp
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






int BlockNumber(int x, int y, int w, int h) {
	for (int i = 0; i < static_cast<int>(numBlocks); i++) {
		//std::cout << "i:" << i << "\n";
		if (BoxOverlap(x, y, w, h, blocks[i].GetX(), blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH())) {
			return i;
		}
	}
	return -1;
}


/** BoxContents returns:
***	-1 empty
***	-2 brick
***	-3 spike
***	-4 telepad
***	a block number (>= 0) if it contains a block
**/
int BoxContents(int x, int y, int w, int h) {
	int i;
	
	// Bricks
	for (i = 0; i < static_cast<int>(numBricks); i++) {
		if (BoxOverlap(x, y, w, h, bricks[i].GetX(), bricks[i].GetY(), bricks[i].GetW(), bricks[i].GetH())) {
			return -2;
		}
	}
	
	// Spikes
	for (i = 0; i < static_cast<int>(numSpikes); i++) {
		if (BoxOverlap(x, y, w, h, spikes[i].GetX(), spikes[i].GetY(), TILE_W, TILE_H)) {
			return -3;
		}
	}

	// Telepads
	for (i = 0; i < static_cast<int>(numTelepads); i++) {
		if (BoxOverlap(x, y, w, h, telepads[i].GetX1(), telepads[i].GetY1() + (TILE_H - 4), TILE_W, 2) ||
			BoxOverlap(x, y, w, h, telepads[i].GetX2(), telepads[i].GetY2() + (TILE_H - 4), TILE_W, 2))
		{
			return -4;
		}
	}

	// Blocks
	i = BlockNumber(x, y, w, h);
	if (i >= 0) return i;
	
	return -1;
}



bool BoxOverlap (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
	/*
	SDL_LockSurface(screenSurface);
	for (int b = y2; b < y2 + h2; b++) {
		for (int a = x2; a < x2 + w2; a++) {
			PutPixel(screenSurface, a, b, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0xff));
		}
	}
	for (int b = y1; b < y1 + h1; b++) {
		for (int a = x1; a < x1 + w1; a++) {
			PutPixel(screenSurface, a, b, SDL_MapRGB(screenSurface->format, 0xff, 0x00, 0x00));
		}
	}
	SDL_UnlockSurface(screenSurface);
	SDL_UpdateRect(screenSurface, 0, 0, SCREEN_W, SCREEN_H);
	SDL_Delay(1);
	*/

	if (x1 <= x2 + (w2 - 1) && x1 + (w1 - 1) >= x2 && y1 <= y2 + (h2 - 1) && y1 + (h1 - 1) >= y2) {
		return true;
	}
	else {
		return false;
	}
}



bool block::OnSolidGround() {
	int c = BoxContents(x, y + h, w, 1);
	
	// If the block is sitting on an unmovable object (a brick, a spike, or a telepad)
	if (c == -2 || c == -3 || c == -4) {
		return true;
	}
	
	// If the block is on another block, check that block
	if (c >= 0) {
		return blocks[c].OnSolidGround();
	}
	
	// If the block isn't on anything
	return false;
}




void block::Climb(char direction) {
	int b;
	char s[11];
	int x1, y1; // Destination x and y
	
	if (direction == 0) {
		x1 = x - TILE_W;
	}
	else {
		x1 = x + w;
	}
	
	b = BoxContents(x1, y, TILE_W, h);

	// If the tile to the left/right is empty, simply move.
	if (b == -1) {
		xMoving = x1 - x;
	}
	// If the space to the left/right is occupied
	else {
		// Check each horizontal line, descending
		for (int j = -(TILE_H - h); j < h; j++) {
			b = BoxContents(x1, y + j, w, 1);
			if (b != -1) {
				y1 = y - (h - j); // Destination y coordinate
				break;
			}
		}
		
		// If it's either not a block, or it's a block on solid ground
		if (b < 0 || blocks[b].OnSolidGround()) {
			// If the diagonal tile (up & to the left/right) is clear
			if (BoxContents(x1, y1, w, h) == -1) {
				//Climb up block/brick
				path = "";
				//sprintf(s, "-%dy%dx", (y - y1) + (TILE_H - h), x1 - x);
				sprintf(s, "-%dy%dx", (y - y1), x1 - x);
				path = s;
			}
		}
	}
}




void block::Physics() {
	int i, hit, oldX, oldY;
	bool doGravity;
	
	// Don't do gravity more than once per frame, if we are doing physics more than once.
	(didPhysics == false) ? doGravity = true : doGravity = false;
	didPhysics = true;
	
	
	/*** Process paths ***/
	if (path.length() > 0 && xMoving == 0 && yMoving == 0) {
		char s;
		//std::cout << "==Processing Path==\n";
		//std::cout << "path: " << path << "\n";
		do {
			for (i = 0; i < static_cast<int>(path.length()); i++) {
				s = path[i];
				if (s == 'x' || s == 'y') {
					std::stringstream ss(path.substr(0, i));
					int n; // Number of tiles to move/milliseconds to sleep
					ss >> n;
					
					switch (s) {
						case 'x':
							xMoving = n;
							doGravity = false; // Cancel out gravity for the first frame,
									   // since we haven't got a chance to
									   // actually start moving X yet
							break;
						case 'y':
							yMoving = n;
							break;
					}
					
					// Remove the part of the path string we just processed
					path = path.erase(0, i + 1);
					
					break;
				}
			}
		} while ( !(xMoving != 0 || yMoving != 0 || path.length() == 0) );
	}
	
	
	
	/*** Y-AXIS ***/
	int newY = y;
	
	if (doGravity) {
		if (yMoving == 0) newY += blockYGravity;
	}

	// Process path movement
	if (yMoving < 0) {
		newY -= blockYSpeed;
		if (newY < y + yMoving) newY = y + yMoving; // Protect against high block speed moving it too far
	}
	if (yMoving > 0) {
		newY += blockYSpeed;
		if (newY > y + yMoving) newY = y + yMoving; // Protect against high block speed moving it too far
	}


	// Move y up to newY, processing any collisions along the way
	if (newY < y) {
		// Check each pixel between y - 1 and newY
		for (i = y - 1; i >= newY; i--) {
			// Loop to process all collisons (it's rare that there's more than one, but it's theoretically possible)
			while (true) {
				hit = BoxContents(x, i, w, 1);  // Returns -1 if the box is clear
				
				// If this space is occupied by other object
				if (hit != -1) {
					
					// If we hit a block, try to get it out of the way
					if (hit >= 0) {
						// Do its physics/gravity if it hasn't done them this frame,
						// since the order of block numbers may not be the ideal order
						// for how they are arranged on the playing field.
						if (blocks[hit].GetDidPhysics() == false) {
							oldX = x;
							oldY = y;
							blocks[hit].Physics();
							
							// If THIS y was changed from some crazy chain reaction when
							// we called Physics, then get out of here.
							if (x != oldX || y != oldY) {
								return; // Exit the function
							}
							continue; // Check if we're still hitting anything
						}

						// If this block is strong or if it's a player pushing up (can push ONE block only, see other 'type' line below), and
						// If the block hasn't moved this frame, we may try to
						// push it out of the way (a block is only permitted to
						// move once per frame)
						if ((strong > 0 || type == 1) && blocks[hit].GetMoved() == false) {
							// Try pushing the block out of the way
							// if it's not already moving
							if (blocks[hit].GetPath().length() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0) {
								oldY = blocks[hit].GetY(); // Save old position of this block we're trying to push
								blocks[hit].SetYMoving(newY - y);
								if (type == 0) blocks[hit].SetStrong(2); // If this is a block pushing on this block, temporarily make this block strong
								
								blocks[hit].Physics();

								// If it worked, check if we're still hitting anything.
								if (blocks[hit].GetY() < oldY) continue;
							}
						}
					}
					
					// If we are still hitting something, stop here.
					if (hit != -1) {
						newY = i + 1; // This is as far as the block could go
						
						// Abort the path
						yMoving = 0;
						path = "";
						
						//std::cout << "y hit " << hit << " at " << newY << " (going up)\n";
						
						hit = -3; // Flag for that line down there to also break out
							 // of the for loop.
						break; // Breaks out of the while loop.
					}
				}
				if (hit == -1) break;
			}
			if (hit == -3) break;
		}
	}
	

	// Move y down to newY, processing any collisions along the way
	if (newY > y) {
		bool sunkPlayer = false;
		// Check each pixel between y + (h - 1) and newY + (h - 1)
		for (i = y + 1; i <= newY; i++) {
			// Loop to process all collisons (it's rare that there's more than one, but it's theoretically possible)
			while (true) {
				hit = BoxContents(x, i + (h - 1), w, 1); // Returns -1 if the box is clear
			
				// If this space is occupied by other object
				if (hit != -1) {
					
					// If we hit a block, try to get it out of the way
					if (hit >= 0) {
						// Do its physics if it hasn't done them this frame,
						if (blocks[hit].GetDidPhysics() == false) {
							oldX = x;
							oldY = y;
							blocks[hit].Physics();
							
							// If THIS block was moved from some crazy chain reaction when
							// we called Physics, then get out of here.
							if (x != oldX || y != oldY) {
								return; // Exit the function
							}
							continue; // Check if we're still hitting anything
						}

						// If this block is strong or if we're hitting a player, and
						// If the block hasn't moved this frame, we may try to
						// push it out of the way (a block is only permitted to
						// move once per frame)
						if ((strong > 0 || blocks[hit].GetType() == 1) && blocks[hit].GetMoved() == false) {
							
							// Try pushing the block out of the way
							// if it's not already moving.
							if (blocks[hit].GetPath().length() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0) {
								oldY = blocks[hit].GetY(); // Save old position of this block we're trying to push
								
								// If this is a player, decrease his height so he will sink
								// a little (2px total) under the weight of the block.  But
								// only do this once per frame (sunkPlayer keeps track of this)
								if (blocks[hit].GetType() == 1 && blocks[hit].GetH() > TILE_H - 2 && sunkPlayer == false && blocks[hit].GetX() == x) {
									blocks[hit].SetH(blocks[hit].GetH() - 2);
									sunkPlayer = true;
								}
								
								blocks[hit].SetYMoving(newY - y);
								blocks[hit].SetStrong(2); // Temporarily make this block strong
								
								blocks[hit].Physics();

								// If it worked, check if we're still hitting anything.
								if (blocks[hit].GetY() > oldY) continue;
							}
						}
					}
					
					// If we are still hitting something, stop here.
					if (hit != -1) {
						newY = i - 1; // This is as far as the block could go

						// Abort the path
						yMoving = 0;
						path = "";

						//std::cout << "y hit " << hit << " at " << newY << " (going down)\n";
						
						hit = -3; // Flag for that line down there to also break out
							 // of the for loop.
						break; // Breaks out of the while loop.
					}
				}
				if (hit == -1) break;
			}
			if (hit == -3) break;
		}
	}
	
	
	if (y != newY) moved = true;
	y = newY; // Set y to newY, which was as far as we got until hitting something
	


	


	/*** X-AXIS ***/
	int newX = x;
	
	if (doGravity) {
		if (xMoving == 0 ) newX += blockXGravity; // Maybe remove the requirement that xMoving be 0
	}

	// Process X path movement
	if (xMoving < 0) {
		newX -= blockXSpeed;
		if (newX < x + xMoving) newX = x + xMoving; // Protect against high block speed moving it too far
		
		// Decrease pixels remaining to move along path
		//xMoving += blockXSpeed;
		//if (xMoving > 0) xMoving = 0;
	}
	if (xMoving > 0) {
		newX += blockXSpeed;
		if (newX > x + xMoving) newX = x + xMoving; // Protect against high block speed moving it too far

		// Decrease pixels remaining to move along path
		//xMoving -= blockXSpeed;
		//if (xMoving < 0) xMoving = 0;
	}
	

	// Move x left to newX, processing any collisions along the way
	if (newX < x) {
		// Check each pixel between x - 1 and newX
		for (i = x - 1; i >= newX; i--) {
			// Loop to process all collisons (it's rare that there's more than one, but it's theoretically possible)
			while (true) {
				hit = BoxContents(i, y, 1, h); // Returns -1 if the box is clear
				
				// If this space is occupied by other object
				if (hit != -1) {
					
					// If we hit a block, try to get it out of the way
					if (hit >= 0) {
						// First, do its physics if it hasn't done them this frame,
						// just in case normal gravity or something will get it out
						// of the way.
						if (blocks[hit].GetDidPhysics() == false) {
							oldX = x;
							oldY = y;
							blocks[hit].Physics();
							
							// If THIS x was changed from some crazy chain reaction when
							// we called Physics, then get out of here.
							if (x != oldX || y != oldY) {
								return; // Exit the function
							}
							continue; // Check if we're still hitting anything
						}
						

						// If this block is strong, and
						// If the block hasn't moved this frame, we may try to
						// push it out of the way (a block is only permitted to
						// move once per frame)
						if (strong > 0 && blocks[hit].GetMoved() == false) {				
							// Try pushing the block out of the way
							// if it's not already moving
							if (blocks[hit].GetPath().length() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 
								// and if it's on solid ground
								&& blocks[hit].OnSolidGround())
							{
								oldX = blocks[hit].GetX(); // Save old position of this block we're trying to push
								blocks[hit].SetXMoving(newX - x);
								blocks[hit].SetStrong(2); // Temporarily make this block strong
								
								blocks[hit].Physics();
								
								// If it worked, check if we're still hitting anything
								if (blocks[hit].GetX() < oldX) continue;
							}
						}
					}
					
					// If we still are hitting something, stop here.
					if (hit != -1) {
						newX = i + 1; // This is as far as the block could go

						// Abort the path
						xMoving = 0;
						path = "";

						//std::cout << "x hit " << hit << " at " <<  newX << "(going left)\n";
						
						hit = -3; // Flag
						break; // Out of while loop
					}
				}
				if (hit == -1) break;
			}
			if (hit == -3) break;
		}
	}

	// Move x right to newX, processing any collisions along the way
	if (newX > x) {
		// Check each pixel between x + (h - 1) and newX + (h - 1)
		for (i = x + 1; i <= newX; i++) {
			// Loop to process all collisons (it's rare that there's more than one, but it's theoretically possible)
			while (true) {
				hit = BoxContents(i + (w - 1), y, 1, h); // Returns -1 if the box is clear
				
				// If this space is occupied by other object
				if (hit != -1) { 
					
					// If we hit a block, try to get it out of the way
					if (hit >= 0) {
						// First, do its physics if it hasn't done them this frame,
						// just in case normal gravity or something will get it out
						// of the way.
						if (blocks[hit].GetDidPhysics() == false) {
							oldX = x;
							oldY = y;
							blocks[hit].Physics();
							
							// If THIS x was changed from some crazy chain reaction when
							// we called Physics, then get out of here.
							if (x != oldX || y != oldY) {
								return;
							}
							continue; // Check if we're still hitting anything
						}
						

						// If this block is strong, and
						// If the block hasn't moved this frame, we may try to
						// push it out of the way (a block is only permitted to
						// move once per frame)
						if (strong > 0 && blocks[hit].GetMoved() == false) {							
							// Try pushing the block out of the way
							// if it's not already moving
							if (blocks[hit].GetPath().length() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 
								// and if it's on solid ground
								&& blocks[hit].OnSolidGround())
							{
								oldX = blocks[hit].GetX(); // Save old position of this block we're trying to push
								blocks[hit].SetXMoving(newX - x);
								blocks[hit].SetStrong(2); // Temporarily make this block strong
								
								blocks[hit].Physics();

								// If it worked, check if we're still hitting anything
								if (blocks[hit].GetX() > oldX) continue;
							}
						}
					}
					
					// If we still are hitting something, stop here.
					if (hit != -1) {
						newX = i - 1; // This is as far as the block could go

						// Abort the path
						xMoving = 0;
						path = "";

						//std::cout << "x hit " << hit << " at " <<  (w - 1) + newX << " (going right)\n";
						
						hit = -3; // Flag
						break; // Out of while loop
					}
				}
				if (hit == -1) break;
			}
			if (hit == -3) break;
		}
	}

	if (x != newX) moved = true;
	oldX = x;
	x = newX; // Set x to newX, which was as far as we got until hitting something
	
	
	/** If this block moved, move the block on top of it **/
	if (x !=oldX) {
		int b = BlockNumber(oldX, y - 1, w, 1);
		if (b >= 0 && blocks[b].GetX() == oldX && blocks[b].GetMoved() == false && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
		//if (b >= 0 && blocks[b].GetMoved() == false && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
			blocks[b].SetXMoving(x - oldX);
			blocks[b].Physics();
			// Jesus is Lord.  Thank you Jesus for sacrificing yourself for me!
		}
	}
}




// Decrease pixels remaining to move along path
void block::PostPhysics() {
	if (yMoving < 0) {
		yMoving += blockYSpeed;
		if (yMoving > 0) yMoving = 0;
	}
	if (yMoving > 0) {
		yMoving -= blockYSpeed;
		if (yMoving < 0) yMoving = 0;
	}
	

	if (xMoving < 0) {
		xMoving += blockXSpeed;
		if (xMoving > 0) xMoving = 0;
	}
	if (xMoving > 0) {
		xMoving -= blockXSpeed;
		if (xMoving < 0) xMoving = 0;
	}
	
	if (strong == 2) strong = 0;
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
		
		// Don't teleport if it was only a new passenger because of Undo-ing
		if (justUndid == true) {
			occupant1Teleported = true;
			doIt = false;
		}
		else {
			occupant1Teleported = false;
		}
	}

	// Check if occupant2 is new
	b = GetOccupant2();
	if (b != occupant2) {
		occupant2 = b;
		if (occupant2 >= 0) doIt = true; // Try to teleport

		// Don't teleport if it was only a new passenger because of Undo-ing
		if (justUndid == true) {
			occupant2Teleported = true;
			doIt = false;
		}
		else {
			occupant2Teleported = false;
		}
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

	// The "free" telepad must be clear of any blocks that may be in the way even
	// if they are not directly on the telepad (e.g. a falling block)
	if (doIt == true) {
		if (occupant1 == -1) {
			if (BlockNumber(x1, y1 - TELEPAD_H, TILE_W, TILE_H) != -1) {
				doIt = false;
			}
		}
		if (occupant2 == -1) {
			if (BlockNumber(x2, y2 - TELEPAD_H, TILE_W, TILE_H) != -1) {
				doIt = false;
			}
		}
	}

	if (doIt) {
		return true;
	}
	
	return false;
}



void telepad::Teleport() {
	state = 2; // Teleporting
	
	// Get bottom block number
	int b;
	b = (occupant1 >= 0 ? occupant1 : occupant2);

	/*** Determine destination info ***/
	int dX, dY;
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


	/*** Determine the height of the source load to teleport (i.e. find blocks on top of the block) ***/
	int sX, sY;
	int sH; // source height
	int ba[numBlocks]; // Array to keep track of which blocks are going to get teleported
	uint numToMove = 1; // How many blocks are going to get teleported

	ba[0] = b; // The first block in the array is the bottom block (usually the player)
	sX = blocks[b].GetX();
	sY = blocks[b].GetY();
	sH = blocks[b].GetH();
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
	


	/*** Animate the teleportation ***/
	// Create source surface
	SDL_Surface *sourceSurf = MakeSurface(sH);
	
	int yPos = sH; // Keeps track of current y Position (bottom to top)
	for (uint i = 0; i < numToMove; i++) {
		yPos -= blocks[ba[i]].GetH();
		
		// Blit images of the teleporting blocks to the source surface
		ApplySurface(0, yPos, blocks[ba[i]].GetSurface(), sourceSurf);
	
		// Make all teleporting blocks invisible so they wont be rendered by their GetSurface()
		blocks[ba[i]].SetType( -(blocks[ba[i]].GetType() + 1));
	}
	// Update the source surface
	//SDL_UpdateRect(sourceSurf, 0, 0, 0, 0);

	// Create destination surface
	SDL_Surface *destSurf = MakeSurface(sH);
	

	int pX, pY; // pixel offset coordinates	
	uint col; // pixel color
	uint squareSize = 4; // How many pixels will make up each square that is moved
	
	bool map[(sH * TILE_W) / squareSize]; // Keeps track of which squares have been teleported (false = hasn't been moved, true = has)
	
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
	uint numPixels; // Will count how many pixels are finished
	
	
	// "Teleport" each pixel from the source to the destination
	while (true) {
		// Lock source and dest surfaces
		if (LockSurface(sourceSurf) == false) continue;
		if (LockSurface(destSurf) == false) continue;
		
		// Move multiple squares at a time, and always proportionate to the
		// load size (so it will always appear to be the same speed)
		for (uint i = 0; i < (sH / squareSize); i++) {
			// Check if we've moved all the pixels over
			numPixels = 0;
			for (int j = 0; j < (sH * TILE_W) / squareSize; j++) {
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
		
			// Trasfer one squares at a time (whose dimensions are (squareSize / 2) x (squareSize / 2))
			for (int sqY = 0; sqY < squareSize / 2; sqY++) {
				for (int sqX = 0; sqX < squareSize / 2; sqX++) {
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
		
		// Draw background, other objects
		Render(0);
		
		// Blit source & dest surfaces to screenSurface
		ApplySurface(sX - cameraX, sY - cameraY, sourceSurf, screenSurface);
		ApplySurface(dX - cameraX, dY - cameraY, destSurf, screenSurface);
		
		// Update the whole screenSurface
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
		
		if (numPixels == (sH * TILE_W) / squareSize) break;
	}
	
	// Free the surfaces
	SDL_FreeSurface(sourceSurf);
	SDL_FreeSurface(destSurf);


	
	
	/*** Actually move all the blocks found earlier ***/
	yPos = dB; // Keeps track of current y Position (bottom to top)
	for (uint i = 0; i < numToMove; i++) {
		yPos -= blocks[ba[i]].GetH();
		blocks[ba[i]].SetX(dX);
		blocks[ba[i]].SetY(yPos);

		// Make all teleporting blocks visible again (convert -1 back to 0, -2 back to 1)
		blocks[ba[i]].SetType( -(blocks[ba[i]].GetType() + 1));
	}


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
	
	state = 0; // Off
}
