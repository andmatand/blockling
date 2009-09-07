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


void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination) {
	//Make a temporary rectangle to hold the offsets
	SDL_Rect offset;
	
	//Give the offsets to the rectangle
	offset.x = static_cast<Sint16>(x);
	offset.y = static_cast<Sint16>(y);
	
	//Blit the surface
	SDL_BlitSurface(source, NULL, destination, &offset);
}




SDL_Surface* FillSurface(const char *file, bool transparent) {
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



SDL_Surface* TileSurface(std::string path, const char *file, bool transparent) {
	std::string filename = file;
	const char *openPath = (path + filename).c_str();
	
	// If the file does not exist in this path, default to DEFAULT_TILESET_DIR
	FILE *fp = fopen(openPath, "r");
	if (!fp) {
		fprintf(stderr, "Error: %s does not exist in this tileset's directory (%s); using the default tile instead.\n", file, path.c_str());
		path = TILE_BASE_DIR + DEFAULT_TILESET_DIR + "/";
	}
	else {
		fclose(fp);
	}
	
	// Add filename to end of path
	path += filename;

	return FillSurface(path.c_str(), transparent);
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




void SetCameraTargetBlock(uint b) {
	cameraTargetX = blocks[b].GetX() + (blocks[b].GetW() / 2);
	cameraTargetY = blocks[b].GetY() + (blocks[b].GetH() / 2);
}




void CenterCamera(char override) {
	static int cameraXVel = 0, cameraYVel = 0;
	static char currentMovement = 0;
	int maxXVel;
	int maxYVel;
	
	// If camera has recently been manually moved
	if (manualCameraTimer > 0 && SDL_GetTicks() < manualCameraTimer + 2000) {
		// Don't allow camera to move too far away with the level completely offscreen
		if (cameraX + SCREEN_W < levelX)
			cameraX = levelX - SCREEN_W;
		if (cameraX > levelX + levelW)
			cameraX = levelX + levelW;

		if (cameraY + SCREEN_H < levelY)
			cameraY = levelY - SCREEN_H;
		if (cameraY > levelY + levelH)
			cameraY = levelY + levelH;
		
		cameraXVel = 0;
		cameraYVel = 0;
		//currentMovement = 2; // Re-center the camera quickly
		
		return; // Exit the function, avoiding any automatic camera movement
	}
	
	
	
	// If the camera is not currently in a movement
	if (currentMovement == 0) {
		// If the target is onscreen (e.g. the camera is just auto-tracking the player's movements)
		// make the camera move no quicker than the player's movements (to avoid jerky camera movements)
		if (
			!stickyPlayer
			&& cameraTargetX >= cameraX && cameraTargetX <= cameraX + (SCREEN_W - 1) 
			&& cameraTargetY >= cameraY && cameraTargetY <= cameraY + (SCREEN_H - 1))
		{
			currentMovement = 1; // Mark the beginning of a slow movement
		}
		else {
			currentMovement = 2; // Camera is moving to get the target onscreen
					     // and should thus move quickly for the entire movement.
					     // (This marks the beginning of a fast movement)
		}
	}

	// Slow movement's max velocity
	if (currentMovement == 1) {
		maxXVel = blockXSpeed;
		maxYVel = blockYSpeed;
	}
	// Default max velocity
	else {
		maxXVel = TILE_W * 3;
		maxYVel = TILE_H * 3;
	}
	
	// Velocity for "zinging" to next level!
	if (override == 2 || stickyPlayer) {
		maxXVel = TILE_W * 3;
		maxYVel = TILE_H * 3;	
	}
	

	
	// Width and height of tracking box.  The camera will only bother
	// moving if the target moves outside of this "box" in the middle
	// of the screen.
	int boxW = 10 * TILE_W;
	int boxH = 5 * TILE_H;
	
	// How much space to show beyond the edges of the level
	int xMargin = TILE_W * 1;
	int yMargin = TILE_H * 1;
	
	
	// Target X and Y are the final destination that the camera needs to reach,
	// which are the position that will make cameraTargetX and Y appear in the
	// middle of the screen.
	int targetX = cameraTargetX - (SCREEN_W / 2);
	int targetY = cameraTargetY - (SCREEN_H / 2);
	

	if (override != 2 && !stickyPlayer) {
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


		
		// For levels that are wider than the screen, keep the camera from going too far outside the level
		// and showing us useless empty space.
		if (levelW > SCREEN_W - (xMargin * 2)) {
			// If the camera is showing too much space to the right side of the level
			if (targetX + (SCREEN_W - 1) > levelX + (levelW - 1) + xMargin) {
				// Move the level as far right as it *should* go.
				targetX = levelX + (levelW - 1) + xMargin - (SCREEN_W - 1);
			}
			
			// If the camera is too far left of the left side of the level
			if (targetX < levelX - xMargin) {
				// Move the level as far left as it *should* go
				targetX = levelX - xMargin;
			}
		}
		// If the level's width will fit onscreen, keep the level centered
		else {
			targetX = -((SCREEN_W / 2) - (TILE_W / 2));
		}
	}
	
	if (!stickyPlayer) {
		// For levels that are taller than the screen, keep the camera from going too far outside the level
		// and showing us useless empty space.	
		if (levelH > SCREEN_H - (yMargin * 2) - TILE_H) {
			// If the camera is showing too much space to below the level (and one extra tile for text)
			if (targetY + (SCREEN_H - 1) > levelY + (levelH - 1) + yMargin + TILE_H) {
				// Move the level as far down as it *should* go.
				targetY = levelY + (levelH - 1) + yMargin + TILE_H - (SCREEN_H - 1);
			}
			
			// If the camera is too far above the level
			if (targetY < levelY - yMargin) {
				// Move the level as far up as it *should* go
				targetY = levelY - yMargin;
			}
		}
		else {
			targetY = -((SCREEN_H / 2) - (TILE_H / 2) - (TILE_H * 2));
		}
	}	
	
	
	// Move the camera instantly
	if (override == 1) {
		//cameraXVel = 0;
		//cameraYVel = 0;
		
		cameraX = targetX;
		cameraY = targetY;
		
		return;
	}
	
	// Adjust camera X and Y velocities to move towards target X and Y
	if (cameraX > targetX) cameraXVel --;
	if (cameraX < targetX) cameraXVel ++;
	
	if (cameraY > targetY) cameraYVel --;
	if (cameraY < targetY) cameraYVel ++;


	// Enforce maximum velocity limitations, slowing down smoothly
	if (cameraXVel > maxXVel) {
		cameraXVel = maxXVel;
		//cameraXVel =- 2;
		//if (cameraXVel < 0) cameraXVel = 0;
	}
	if (cameraXVel < -maxXVel) {
		cameraXVel = -maxXVel;
		//cameraXVel += 2;
		//if (cameraXVel > 0) cameraXVel = 0;	
	}

	if (cameraYVel > maxYVel) {
		cameraYVel = maxYVel;
		//cameraYVel =- 2;
		//if (cameraYVel < 0) cameraYVel = 0;
	}
	if (cameraYVel < -maxYVel) {
		cameraYVel = -maxYVel;
		//cameraYVel += 2;
		//if (cameraYVel > 0) cameraYVel = 0;	
	}

	
	// Slow down to zero velocity by the time we reach the target X
	if (cameraXVel != 0) {
		int xStoppingDistance = 0; // How much distance it will take for the camera to come to
		                          // a complete stop if it starts slowing down now.
		
		for (uint i = abs(cameraXVel); i > 0; i--) {
			xStoppingDistance += i;
		}
		
		if (xStoppingDistance > abs(targetX - cameraX)) {
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
		int yStoppingDistance = 0; // How much distance it will take for the camera to come to
		                          // a complete stop if it starts slowing down now.
		
		for (uint i = abs(cameraYVel); i > 0; i--) {
			yStoppingDistance += i;
		}
		
		if (yStoppingDistance > abs(targetY - cameraY)) {
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

	// Detect when the currentMovement has ended
	if (currentMovement > 0) {
		if (cameraXVel == 0 && cameraYVel == 0) {
			currentMovement = 0;
			physicsStarted = true;
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
		case 10:
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

	return 0;
}




// Returns correct surface for the block's/player's current face & direction.
SDL_Surface* brick::GetSurface() {
	return brickSurface[static_cast<int>(type)];
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



// Loads the tiles of the currently selected tileset, defaulting back
// to the default tileset for (and only for) any tiles not found in
// the tilesetDir.
void LoadTileset(std::string tilesetDir) {
	
	/*** Free all old surfaces ***/
	UnloadTileset();

	/*** Load new tile bmps into surfaces ***/
	// Set the path
	std::string path;
	path = TILE_BASE_DIR + tilesetDir + "/";
	
	bgSurface = TileSurface(path, "bg.bmp", 0);
	bgW = bgSurface->w;
	bgH = bgSurface->h;

	blockSurface = TileSurface(path, "block.bmp", 1);
	spikeSurface = TileSurface(path, "spike.bmp", 1);

	int a = 0, b = 0;
	char fn[32]; // filename
	int n;
	
	// Player surfaces
	for (uint i = 0; i < NUM_PLAYER_SURFACES; i++) {
		n = sprintf(fn, "player%d_%d.bmp", a, b);
		playerSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
		
		b++;
		if (b > (NUM_PLAYER_SURFACES / 3) - 1) {
			b = 0;
			a++;
		}
	}

	// Brick surfaces
	for (uint i = 0; i < NUM_BRICK_SURFACES; i++) {
		n = sprintf(fn, "brick%d.bmp", i);
		brickSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}


	// Torch surfaces
	for (uint i = 0; i < NUM_TORCH_FLAMES; i++) {
		n = sprintf(fn, "torch%d.bmp", i);
		torchSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}

	// Telepad surfaces
	for (uint i = 0; i < NUM_TELEPAD_STATES; i++) {
		n = sprintf(fn, "telepad%d.bmp", i);
		telepadSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}

	// Exit surfaces
	for (uint i = 0; i < NUM_EXIT_FRAMES; i++) {
		n = sprintf(fn, "exit%d.bmp", i);
		exitSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}

	// Item surfaces
	for (uint i = 0; i < NUM_ITEM_TYPES; i++) {
		n = sprintf(fn, "item%d.bmp", i);
		itemSurface[i] = TileSurface(path, fn, 1);
		
		#ifdef DEBUG
		std::cout << "Loading " << fn << "\n";
		#endif
	}


}




bool LockSurface(SDL_Surface *surf) {
	// Lock surface if needed
	if (SDL_MUSTLOCK(surf)) {
		if (SDL_LockSurface(surf) < 0) return false;
	}
	
	return true;
}


void UnloadTileset() {
	SDL_FreeSurface(bgSurface);
	SDL_FreeSurface(blockSurface);
	SDL_FreeSurface(spikeSurface);
	for (uint i = 0; i < NUM_PLAYER_SURFACES; i++) {
		SDL_FreeSurface(playerSurface[i]);
	}
	for (uint i = 0; i < NUM_BRICK_SURFACES; i++) {
		SDL_FreeSurface(brickSurface[i]);
	}	
	for (uint i = 0; i < NUM_TORCH_FLAMES; i++) {
		SDL_FreeSurface(torchSurface[i]);
	}
	for (uint i = 0; i < NUM_TELEPAD_STATES; i++) {
		SDL_FreeSurface(telepadSurface[i]);
	}
	for (uint i = 0; i < NUM_EXIT_FRAMES; i++) {
		SDL_FreeSurface(exitSurface[i]);
	}
	for (uint i = 0; i < NUM_ITEM_TYPES; i++) {
		SDL_FreeSurface(itemSurface[i]);
	}
}




// Makes a surface with certain characteristics (used for teleportation animation)
SDL_Surface* MakeSurface(int width, int height) {
	//return FillSurface("data/bmp/block0.bmp", true);

	SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, SCREEN_BPP, screenSurface->format->Rmask, screenSurface->format->Gmask, screenSurface->format->Bmask, 0); //screenSurface->format->Amask
	SDL_Surface *surface = NULL;
	
	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	
	// Make bright pink #ff00ff transparent
	uint colorKey = SDL_MapRGB(surface->format, 0xff, 0x00, 0xff);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorKey);

	// Fill surface with transparent color
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
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



void DrawBackground() {
	static int bgX, bgY;
	static uint bgTimer = 0;
	
	// Move background offset X and Y (to scroll it)
	if (SDL_GetTicks() > bgTimer + 60) {
		bgX += 1;
		if (bgX >= bgW) bgX = 0;
		
		bgY += 1;
		if (bgY >= bgH) bgY = 0;
		
		bgTimer = SDL_GetTicks();
	}
	for (int b = -bgH; b < -bgH + (((SCREEN_H / bgH) + 2) * bgH); b+= bgH) {
		for (int a = -bgW; a < -bgW + (((SCREEN_W /bgW) + 2) * bgW); a+= bgW) {
			ApplySurface(a + bgX, b + bgY, bgSurface, screenSurface);
		}
	}
}


// flag	0 = No screen update (drawing only)
//      1 = normal
//      2 = no CenterCamera
//	3 = combination of 0 and 2
//	4 = no background
void Render (char flag) {
	static uint torchTimer = 0;
	static uint doorFrame, doorFramePause;
	static uint timerPos;
	uint i;
	

	//if (LockSurface(screenSurface) == false) return;

	if (flag != 2 && flag != 3)
		CenterCamera(0);
	
	
	/*** Background ***/
	if (flag != 4) {
		DrawBackground();
	}
	
	
	/*** BRICKS ***/
	for (i = 0; i < numBricks; i++) {
		ApplySurface(bricks[i].GetX() - cameraX, bricks[i].GetY() - cameraY, bricks[i].GetSurface(), screenSurface);
	}
	
	/*** SPIKES ***/
	for (i = 0; i < numSpikes; i++) {
		ApplySurface(spikes[i].GetX() - cameraX, spikes[i].GetY() - cameraY, spikeSurface, screenSurface);
	}

	
	/*** EXIT ***/
	if (wonLevel == 0) {
		doorFrame = 0;
		doorFramePause = 0;
	}
	if (wonLevel == 1) {
		// Start the animation at frame 1 (not 0)
		if (doorFrame == 0) doorFrame = 1;
		
		// Make each doorFrame display for multiple frames
		doorFramePause ++;
		if (doorFramePause == 2) {
			doorFrame ++;
			doorFramePause = 0;
		}
	}
	if (wonLevel == 1 && doorFrame == NUM_EXIT_FRAMES - 1)
		wonLevel = 2;
	ApplySurface(exitX - cameraX, exitY - cameraY, exitSurface[doorFrame], screenSurface);

	
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
	// Deactivate stickyPlayer when he's lined up in the new level's position.
	if (flag != 3 && blocks[0].GetX() - cameraX <= stickyPlayerX && blocks[0].GetY() - cameraY == stickyPlayerY) {
		stickyPlayer = false;
		blocks[0].SetFace(0); // normal
	}
	
	// Activate stickyPlayer
	if (wonLevel == 4 && blocks[0].GetWon() > 0 && stickyPlayer == false) {
		stickyPlayer = true;
		stickyPlayerOrigX = blocks[0].GetX();
		stickyPlayerOrigY = blocks[0].GetY();
		stickyPlayerX = blocks[0].GetX() - cameraX;
		stickyPlayerY = blocks[0].GetY() - cameraY;
	}

	for (i = 0; i < numBlocks; i++) {
		if (blocks[i].GetType() >= 0) {
			blocks[i].Animate();
			
			if (i != 0 || !stickyPlayer) {
				ApplySurface(blocks[i].GetX() - cameraX, blocks[i].GetY() - cameraY, blocks[i].GetSurface(), screenSurface);
			}
		}
	}


	/*** TELEPADS ***/
	for (i = 0; i < numTelepads; i++) {
		ApplySurface(telepads[i].GetX1() - cameraX, telepads[i].GetY1() - cameraY, telepads[i].GetSurface(), screenSurface);
		ApplySurface(telepads[i].GetX2() - cameraX, telepads[i].GetY2() - cameraY, telepads[i].GetSurface(), screenSurface);
	}


	// Draw player on top of everything when sticky
	if (stickyPlayer) {
		ApplySurface(stickyPlayerX, stickyPlayerY, blocks[0].GetSurface(), screenSurface);
	}


	//UnlockSurface(screenSurface);
	//PutPixel(screenSurface, cameraX + 10, cameraY + 10, SDL_MapRGB(screenSurface->format, 0x00, 0xff, 0x00));


	/*** Text ***/
	char message[128];
	// Level #
	sprintf(message, "Level %d", currentLevel);
	DrawText(10, 380, message, 220, 220, 220);
	
	// Timer
	if (levelTimeRunning) {
		int min = static_cast<int>(levelTime / 60);
		int sec = levelTime % 60;
		sprintf(message, "%02d:%02d", min, sec);
		if (timerPos < 76) {
			timerPos += FPS;
			if (timerPos > 76) timerPos = 76;
		}
		DrawText(SCREEN_W - timerPos, 380, message, 220, 220, 220);
	}
	else {
		timerPos = 0;
	}


	if (flag != 0 && flag != 3) {
		// Tell SDL to update the whole screenSurface
		SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
	}
	
	LimitFPS();
	
}



void LimitFPS() {
	static uint lastTick;
	
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

