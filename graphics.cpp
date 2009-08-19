/*
 *   graphics.h
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


void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
SDL_Surface* MakeSurface(int height);


void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination) {
	//Make a temporary rectangle to hold the offsets
	SDL_Rect offset;
	
	//Give the offsets to the rectangle
	offset.x = static_cast<Sint16>(x);
	offset.y = static_cast<Sint16>(y);
	
	//Blit the surface
	SDL_BlitSurface(source, NULL, destination, &offset);
}



SDL_Surface* FillSurface(const char* file, bool transparent) {
	SDL_Surface *temp = SDL_LoadBMP(file);
	SDL_Surface *surface = NULL;
	
	if (temp == NULL) {
		printf("Failed to load image %s\n", file);
		return NULL;
	}

	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (transparent) {
		//Make bright pink #ff00ff transparent
		uint colorKey = SDL_MapRGB(screenSurface->format, 0xff, 0x00, 0xff);
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorKey);
	}
	
	return surface;
}


// Surface must be locked before calling this:
Uint32 GetPixel(SDL_Surface *surface, int x, int y) {
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x;

	return *p;
}





// Change block/player faces around
void block::Animate() {

	// Blinking
	if (face == 0 || face == 2) { // If the player isn't doing any other animation
		face = 0;
		static uint blinkTimer = SDL_GetTicks() + 1000;
		if (SDL_GetTicks() >= blinkTimer) {
			blinkTimer = SDL_GetTicks() + ((rand() % 10000) + 100);
			face = 2;
		}
	}
	
	// Talking
}	




void CenterCamera(char instant) {
	static int cameraXVel = 0, cameraYVel = 0;

	// Manual override
	if (instant == 1) {
		cameraXVel = 0;
		cameraYVel = 0;
		
		cameraX = cameraTargetX;
		cameraY = cameraTargetY;
		
		return;
	}

	// Width and height of tracking box.  The camera will only bother
	// moving if the target moves outside of this "box" in the middle
	// of the screen.
	int boxW = 10 * TILE_W;
	int boxH = 5 * TILE_H;
	
	// How much space to show beyond the edges of the level
	int xMargin = TILE_W * 1;
	int yMargin = TILE_H * 1;
	
	// Target X and Y are the final destination that the camera needs to reach.
	int targetX = cameraTargetX - (SCREEN_W / 2);
	int targetY = cameraTargetY - (SCREEN_H / 2);
	

	// Adjust target coordinates so that the camera doesn't
	// bother moving if the player is in the box.
	if (cameraX < targetX) {
		if (cameraX + boxW >= targetX) targetX = cameraX;
	}
	if (cameraX > targetX) {
		if (cameraX - boxW <= targetX) targetX = cameraX;
	}

	if (cameraY < targetY) {
		if (cameraY + boxH >= targetY) targetY = cameraY;
	}
	if (cameraY > targetY) {
		if (cameraY - boxH <= targetY) targetY = cameraY;
	}


	// Keep the camera from going too far outside the level
	// and showing us useless empty space.
	//if (targetX > cameraX) {
//		if (targetX + SCREEN_W > levelX + (levelW - 1) + xMargin && targetX > levelX - xMargin) {
		if (targetX + SCREEN_W > levelX + (levelW - 1) + xMargin) {
			//printf("CAMERA target too far RIGHT\n");
//			targetX = levelX + (levelW - 1) + xMargin - SCREEN_W;
		}
	//}
	
	//if (targetX < cameraX) {
		//if (targetX < levelX - xMargin && targetX + SCREEN_W < levelX + (levelW - 1) + xMargin) {
		if (targetX < levelX - xMargin) {
			//printf("CAMERA target too far LEFT\n");
			//targetX = levelX - xMargin;
		}
	//}

	
	// Adjust camera X and Y velocities to move towards newX/newY
	if (cameraX > targetX) cameraXVel --;
	if (cameraX < targetX) cameraXVel ++;
	
	if (cameraY > targetY) cameraYVel --;
	if (cameraY < targetY) cameraYVel ++;

	// Enforce maximum velocity limitations
	if (cameraXVel > TILE_W) cameraXVel = TILE_W;
	if (cameraXVel < -TILE_W) cameraXVel = -TILE_W;
	if (cameraYVel > TILE_H) cameraYVel = TILE_H;
	if (cameraYVel < -TILE_H) cameraYVel = -TILE_H;
	
	// Slow down to zero velocity by the time we reach the target X
	if (cameraXVel != 0) {
		if (abs(abs(targetX - cameraX) / cameraXVel) < abs(cameraXVel)) {
			if (cameraXVel < 0) {
				cameraXVel += 2;
				if (cameraXVel > 0) cameraXVel = 0;
			}
			else {
				cameraXVel -= 2;
				if (cameraXVel < 0) cameraXVel = 0;
			}
		}
	}
	if (cameraYVel != 0) {
		if (abs(abs(targetY - cameraY) / cameraYVel) < abs(cameraYVel)) {
			if (cameraYVel < 0) {
				cameraYVel += 2;
				if (cameraYVel > 0) cameraYVel = 0;
			}
			else {
				cameraYVel -= 2;
				if (cameraYVel < 0) cameraYVel = 0;
			}
		}
	}

	// Actually move the camera
	cameraX += cameraXVel;
	cameraY += cameraYVel;
}




// Returns correct surface for the block's/player's current face & direction.
SDL_Surface* block::GetSurface() {
	switch (type) {
		case 0:
			return blockSurface;
			break;
		case 1:
			return playerSurface[GetSurfaceIndex()];
			break;
		default:
			return NULL;
			break;
	}
}



// Returns actual surface array index number based on player's direction and facial expression.
int block::GetSurfaceIndex() {
	switch (dir) {
		case 0: // Left
			switch (face) {
				case 0: // normal
					return 0;
					break;
				case 1: // mouth open
					return 1;
					break;
				case 2: // blinking
					return 2;
					break;
				case 3: // happy mouth
					return 3;
					break;
				case 4: // scared mouth
					return 4;
					break;
			}
			break;
		case 1: // Right
			switch (face) {
				case 0: // normal
					return 5;
					break;
				case 1: // mouth open
					return 6;
					break;
				case 2: // blinking
					return 7;
					break;
				case 3: // happy mouth
					return 8;
					break;
				case 4: // scared mouth
					return 9;
					break;
			}
			break;
		case 2: // Facing camera
			switch (face) {
				case 0: // normal
					return 10;
					break;
				case 1: // mouth open
					return 11;
					break;
				case 2: // blinking
					return 12;
					break;
				case 3: // happy mouth
					return 13;
					break;
				case 4: // scared mouth
					return 14;
					break;
			}
			break;
	}
}





SDL_Surface* telepad::GetSurface() {
	static uint t = 0;
	
	switch (state) {
		case 0:
			return telepadSurface[0];
			break;
		case 1:
			if (SDL_GetTicks() < t + 500) {
				return telepadSurface[1];
			}
			else {
				if (SDL_GetTicks() >= t + 600) t = SDL_GetTicks();
				return telepadSurface[0];
			}
			
			break;
		case 2:
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




bool LockSurface(SDL_Surface *surf) {
	// Lock surface if needed
	if (SDL_MUSTLOCK(surf)) {
		if (SDL_LockSurface(surf) < 0) return false;
	}
	
	return true;
}




// Makes a surface with certain characteristics (used for teleportation animation)
SDL_Surface* MakeSurface(int height) {
	//return FillSurface("data/bmp/block0.bmp", true);

	SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, TILE_W, height, SCREEN_BPP, screenSurface->format->Rmask, screenSurface->format->Gmask, screenSurface->format->Bmask, 0); //screenSurface->format->Amask
	SDL_Surface *surface = NULL;
	
	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	
	// Make bright pink #ff00ff transparent
	uint colorKey = SDL_MapRGB(surface->format, 0xff, 0x00, 0xff);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorKey);

	// Fill surface with transparent color
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < TILE_W; i++) {
			PutPixel(surface, i, j, colorKey);
		}
	}

	return surface;
}



// Surface must be locked before calling this:
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x;

	*p = static_cast<unsigned char>(pixel);
}




void Render(bool update) {
	static uint bgTimer = 0;
	static uint torchTimer = 0;
	uint i;
	

	//if (LockSurface(screenSurface) == false) return;


	CenterCamera(0);
	
	
	/*** Background ***/
	if (SDL_GetTicks() > bgTimer + 60) {
		bgX += 1;
		if (bgX >= bgW) bgX = 0;
		
		bgY += 1;
		if (bgY >= bgH) bgY = 0;
		
		bgTimer = SDL_GetTicks();
	}
	for (int b = -bgH + cameraY; b < (SCREEN_H / bgH) + bgH - cameraY; b+= bgH) {
		for (int a = -bgW + cameraX; a < (SCREEN_W /bgW) + bgW - cameraX; a+= bgW) {
			//std::cout << "bg (" << a + bgX << ", " << b + bgY << ")\n";
			ApplySurface(a + bgX, b + bgY, bgSurface, screenSurface);
		}
	}	
	
	
	/*** BRICKS ***/
	for (i = 0; i < numBricks; i++) {
		ApplySurface(bricks[i].GetX() - cameraX, bricks[i].GetY() - cameraY, brickSurface, screenSurface);
	}
	
	/*** SPIKES ***/
	for (i = 0; i < numSpikes; i++) {
		ApplySurface(spikes[i].GetX() - cameraX, spikes[i].GetY() - cameraY, spikeSurface, screenSurface);
	}

	
	/*** EXIT ***/
	ApplySurface(exitX - cameraX, exitY - cameraY, exitSurface, screenSurface);

	
	/*** TORCHES ***/
	// Change flames
	if (SDL_GetTicks() > torchTimer + 5) {
		for (i = 0; i < numTorches; i++) {
			torches[i].FlickerFlame();
	    	}
		torchTimer = SDL_GetTicks();
	}
	// Draw torches
	for (i = 0; i < numTorches; i++) {
		
		ApplySurface(torches[i].GetX() + 1 - cameraX, torches[i].GetY() - cameraY, torchSurface[torches[i].GetFlame()], screenSurface);
	}
	
	
	
	/*** BLOCKS ***/
	for (i = 0; i < numBlocks; i++) {
		if (blocks[i].GetType() >= 0) {
			blocks[i].Animate();
			ApplySurface(blocks[i].GetX() - cameraX, blocks[i].GetY() - cameraY, blocks[i].GetSurface(), screenSurface);
		}
	}
	
	/*** TELEPADS ***/
	for (i = 0; i < numTelepads; i++) {
		ApplySurface(telepads[i].GetX1() - cameraX, telepads[i].GetY1() - cameraY, telepads[i].GetSurface(), screenSurface);
		ApplySurface(telepads[i].GetX2() - cameraX, telepads[i].GetY2() - cameraY, telepads[i].GetSurface(), screenSurface);
	}


	//UnlockSurface(screenSurface);

	if (update) {
		// Tell SDL to update the whole screenSurface
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
	}
	
	// Limit FPS
	while (SDL_GetTicks() <= lastTick + (1000 / FPS)) {
		SDL_Delay(1);
	}

	lastTick = SDL_GetTicks();
}





void UnlockSurface(SDL_Surface *surf) {
	// Unlock if needed
	if (SDL_MUSTLOCK(surf))	SDL_UnlockSurface(surf);
}

