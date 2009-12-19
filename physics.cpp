/*
 *   Copyright 2009 Andrew Anderson <www.billamonster.com>
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
		if (BoxOverlap(x, y, w, h, blocks[i].GetX(), blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH())) {
			return i;
		}
	}
	return -1;
}

int BrickNumber(int x, int y, int w, int h) {
	for (int i = 0; i < static_cast<int>(numBricks); i++) {
		if (BoxOverlap(x, y, w, h, bricks[i].GetX(), bricks[i].GetY(), bricks[i].GetW(), bricks[i].GetH())) {
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
***	-5 exit
***	a block number (>= 0) if it contains a block
**/
int BoxContents(int x, int y, int w, int h) {
	int i;
	
	// Bricks
	i = BrickNumber(x, y, w, h);
	if (i >= 0)
		return -2;
	
	// Spikes
	for (i = 0; i < static_cast<int>(numSpikes); i++) {
		if (BoxOverlap(x, y, w, h, spikes[i].GetX(), spikes[i].GetY(), TILE_W, TILE_H)) {
			return -3;
		}
	}

	// Telepads
	for (i = 0; i < static_cast<int>(numTelepads); i++) {
		if (BoxOverlap(x, y, w, h, telepads[i].GetX1(), telepads[i].GetY1() + (TILE_H - 4), TILE_W, 4) ||
			BoxOverlap(x, y, w, h, telepads[i].GetX2(), telepads[i].GetY2() + (TILE_H - 4), TILE_W, 4))
		{
			return -4;
		}
	}

	// Blocks
	i = BlockNumber(x, y, w, h);
	if (i >= 0)
		return i;
	
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




// returns BoxContents return value of thing in the way (directly above)
// with the exception that 0 = a block and there is no value > 0
// (-1 = there's nothing in the way)
int block::Climb(char direction) {
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
			
			// If this is not a player
			if (type < 10) {
				b = BoxContents(x, y - 1, w, 1);
				// If the space above is not empty
				if (b != -1) {
					// If it's not the case that the thing above is
					// a block and this block is strong
					if ( !(b >= 0 && strong > 0) )
						if (b > 0) b = 0;
						return b; // There was no room for the block to go up
				}
			}
			
			// If the tile diagonal tile (up & to the left/right) is clear
			if (BoxContents(x1, y1, w, h) == -1) {
				//Climb up block/brick
				sprintf(s, "-%dy%dx", (y - y1), x1 - x);
				SetPath(s);
			}
		}
	}
	
	return -1;
}




void block::Physics() {
	int i, hit, oldX, oldY;
	bool doGravity;
	
	// Don't do gravity more than once per frame, if we are doing physics more than once.
	(didPhysics == false) ? doGravity = true : doGravity = false;
	didPhysics = true;

	// Don't do any physics on disabled blocks
	if (type < 0) return;
	
	/*** Process paths ***/
	if (GetPathLength() > 0 && xMoving == 0 && yMoving == 0) {
		char s[4];
		s[0] = '\0'; // Temporary string to hold numbers
		int k, n;
		while (true) {
			for (i = 0; i < static_cast<int>(strlen(path)); i++) {
				if (path[i] == 'x' || path[i] == 'y') {
					n = static_cast<int>(strtol(s, NULL, 0));
					
					switch (path[i]) {
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
					if (i + 1 < static_cast<int>(strlen(path))) {
						k = 0;
						for (uint j = i + 1; j < strlen(path); j++) {
							path[k++] = path[j];
						}
						path[k] = '\0';
					}
					else {
						SetPath("");
					}
					
					break;
				}
				else {
					// Append this character to the temporary string
					s[i] = path[i];
					s[i + 1] = 0;

				}
			}
			
			if ((xMoving != 0 || yMoving != 0 || GetPathLength() == 0)) break;
		}
	}
	
	
	
	/*** Y-AXIS ***/
	int newY = y;
	
	if (doGravity) {
		if (yMoving == 0 && (won == 0 || y < exitY)) newY += blockYGravity;
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
						if ((strong > 0 || type >= 10) && blocks[hit].GetMoved() == false) {
							// Try pushing the block out of the way
							// if it's not already moving
							// and if it's not a player exiting the level
							if (blocks[hit].GetPathLength() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 && blocks[hit].GetWon() == 0) {
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
						SetPath("");
						
						//std::cout << "y hit " << hit << " at " << newY << " (going up)\n";
						
						hit = -3; // Flag for that line down there to also break out
							 // of the for loop.
						break; // Breaks out of the while loop.
					}
				}
				if (hit == -1) {
					// If this is a player who hasn't won yet, and he has reached the exit
					if (type >= 10 && won == 0 && BoxOverlap(x, i, w, 1, exitX, exitY, TILE_W, TILE_H)) {
						won = 1;
						return; // Delay doing the physics for now (open the door first)
					}

					break;
				}
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
						if ((strong > 0 || blocks[hit].GetType() >= 10) && blocks[hit].GetMoved() == false) {
							
							// Try pushing the block out of the way
							// if it's not already moving,
							// and if it's not a player exiting the level
							if (blocks[hit].GetPathLength() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 && blocks[hit].GetWon() == 0) {
								oldY = blocks[hit].GetY(); // Save old position of this block we're trying to push
								
								// If this is a player, decrease his height so he will sink
								// a little (2px total) under the weight of the block.  But
								// only do this once per frame (sunkPlayer keeps track of this)
								if (blocks[hit].GetType() >= 10 && blocks[hit].GetH() > TILE_H - 2 && sunkPlayer == false && blocks[hit].GetX() == x) {
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
						SetPath("");

						//std::cout << "y hit " << hit << " at " << newY << " (going down)\n";
						
						hit = -3; // Flag for that line down there to also break out
							 // of the for loop.
						break; // Breaks out of the while loop.
					}
				}
				if (hit == -1) {
					// If this is a player who hasn't won yet, and he has reached the exit
					if (type >= 10 && won == 0 && BoxOverlap(x, i + (h - 1), w, 1, exitX, exitY, TILE_W, TILE_H)) {
						won = 1;
						return; // Delay doing the physics for now (open the door first)
					}

					break;
				}
			}
			if (hit == -3) break;
		}
	}
	
	
	if (y != newY) moved = true;
	y = newY; // Set y to newY, which was as far as we got until hitting something
	


	


	/*** X-AXIS ***/
	int newX = x;
	
	if (doGravity) {
		if (xMoving == 0 && (won == 0 || y < exitY)) newX += blockXGravity;
	}

	// Process X path movement
	if (xMoving < 0) {
		newX -= blockXSpeed;
		if (newX < x + xMoving) newX = x + xMoving; // Protect against high block speed moving it too far
	}
	if (xMoving > 0) {
		newX += blockXSpeed;
		if (newX > x + xMoving) newX = x + xMoving; // Protect against high block speed moving it too far
	}
	
	if (won > 0 && x == exitX) {
		newX = x;
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
							if (blocks[hit].GetPathLength() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 
								// and if it's on solid ground
								&& blocks[hit].OnSolidGround()
								// and if it's not a player exiting the level
								&& blocks[hit].GetWon() == 0)
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
						SetPath("");

						//std::cout << "x hit " << hit << " at " <<  newX << "(going left)\n";
						
						hit = -3; // Flag
						break; // Out of while loop
					}
				}
				if (hit == -1) {
					// If this is a player who hasn't won yet, and he has reached the exit
					if (type >= 10 && won == 0 && BoxOverlap(i, y, 1, h, exitX, exitY, TILE_W, TILE_H)) {
						won = 1;
						return; // Delay doing the physics for now (open the door first)
					}

					break;
				}
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
							if (blocks[hit].GetPathLength() == 0 && blocks[hit].GetXMoving() == 0 && blocks[hit].GetYMoving() == 0 
								// and if it's on solid ground
								&& blocks[hit].OnSolidGround()
								// and if it's not a player exiting the level
								&& blocks[hit].GetWon() == 0)
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
						SetPath("");

						//std::cout << "x hit " << hit << " at " <<  (w - 1) + newX << " (going right)\n";
						
						hit = -3; // Flag
						break; // Out of while loop
					}
				}

				if (hit == -1) {
					// If this is a player who hasn't won yet, and he has reached the exit
					if (type >= 10 && won == 0 && BoxOverlap(i + (w - 1), y, 1, h, exitX, exitY, TILE_W, TILE_H)) {
						won = 1;
						return; // Delay doing the physics for now (open the door first)
					}

					break;
				}
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
			blocks[b].SetXMoving(x - oldX);
			blocks[b].SetDidPhysics(true); // Pretend like we did physics on this block already, so gravity won't be applied to it.
			blocks[b].Physics();
			// Jesus is Lord.  Thank you Jesus for sacrificing yourself for me!
		}
	}
	
	// If the block has fallen offscreen, disable it
	if (y > levelY + levelH + (SCREEN_H * 2)) {
		type = -100;
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







