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





void ApplySurface(int x, int y, SDL_Surface *source, SDL_Surface *destination) {
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
		fprintf(stderr, "Error: Failed to load image \"%s\"\n", file);
		return NULL;
	}

	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	temp = NULL;

	if (transparent) {
		//Make bright pink #ff00ff transparent
		uint colorKey = SDL_MapRGB(screenSurface->format, 0xff, 0x00, 0xff);
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorKey);
	}
	
	return surface;
}



void ToggleFullscreen() {
	option_fullscreen = (option_fullscreen ? false : true);
	
	#ifdef _WIN32
		#ifdef DEBUG
			printf("Windows fullscreen toggle\n");
		#endif
		//SDL_FreeSurface(screenSurface);
		screenSurface = SDL_SetVideoMode(screenSurface->w,
				screenSurface->h,
				screenSurface->format->BitsPerPixel,
				SDL_HWSURFACE |
					(option_fullscreen ? SDL_FULLSCREEN : 0) |
					SDL_ANYFORMAT
					);
	#else
		#ifdef DEBUG
			printf("X11 fullscreen toggle\n");
		#endif
		SDL_WM_ToggleFullScreen(screenSurface);
	#endif
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
}	




void SetCameraTargetBlock(uint b) {
	cameraTargetX = blocks[b].GetX() + (blocks[b].GetW() / 2);
	cameraTargetY = blocks[b].GetY() + (blocks[b].GetH() / 2);
}







void MoveCamera() {
	// If there is a joystick
	if (joystick != NULL) {
		SDL_JoystickUpdate();
		int x = SDL_JoystickGetAxis(joystick, 0);
		int y = SDL_JoystickGetAxis(joystick, 1);
		int threshold = 800;
		int divisor = 8000;

		if (x > threshold || x < -threshold) {
			cameraXVel += x / divisor;
			manualCameraTimer = SDL_GetTicks();
		}
		if (y > threshold || y < -threshold) {
			cameraYVel += y / divisor;
			manualCameraTimer = SDL_GetTicks();
		}
	}

	// If we aren't currently moving the camera manually
	if (SDL_GetTicks() > manualCameraTimer + 2000) {
		// Exit the function
		return;
	}

	// Enforce maximum camera velocity limitations
	//if (cameraXVel > TILE_W) cameraXVel = TILE_W;/
	//if (cameraXVel < -TILE_W) cameraXVel = -TILE_W;
	//if (cameraYVel > TILE_H) cameraYVel = TILE_H;
	//if (cameraYVel < -TILE_H) cameraYVel = -TILE_H;
	
	//if (cameraXVel > -.5 && cameraXVel < .5) cameraXVel = 0;
	//if (cameraYVel > -.5 && cameraYVel < .5) cameraYVel = 0;
	
	cameraX += static_cast<int>(cameraXVel);
	cameraY += static_cast<int>(cameraYVel);

	// Add friction
	cameraXVel *= .85f;
	cameraYVel *= .85f;
}








void CenterCamera(char override) {
	static char currentMovement = 0;
	int maxXVel;
	int maxYVel;
	
	// If camera has recently been manually moved
	if (override == 0 && manualCameraTimer > 0 && SDL_GetTicks() < manualCameraTimer + 2000) {
		// Don't allow camera to move too far away with the level completely offscreen
		if (cameraX + SCREEN_W < levelX)
			cameraX = levelX - SCREEN_W;
		if (cameraX > levelX + levelW)
			cameraX = levelX + levelW;

		if (cameraY + SCREEN_H < levelY)
			cameraY = levelY - SCREEN_H;
		if (cameraY > levelY + levelH + (FONT_H + (FONT_H / 2)))
			cameraY = levelY + levelH + (FONT_H + (FONT_H / 2));
		
		//cameraXVel = 0;
		//cameraYVel = 0;
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
	int boxW = SCREEN_W / 5;
	int boxH = SCREEN_H / 5;
	
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
			if (targetY + (SCREEN_H - 1) > levelY + (levelH - 1) + yMargin + (FONT_H + (FONT_H / 2))) {
				// Move the level as far down as it *should* go.
				targetY = levelY + (levelH - 1) + yMargin + (FONT_H + (FONT_H / 2)) - (SCREEN_H - 1);
			}
			
			// If the camera is too far above the level
			if (targetY < levelY - yMargin) {
				// Move the level as far up as it *should* go
				targetY = levelY - yMargin;
			}
		}
		else {
			targetY = -((SCREEN_H / 2) - (TILE_H / 2));
		}
	}	
	
	
	// Move the camera instantly
	if (override == 1 || override == -1) {
		if (override == -1) {
			cameraXVel = 0;
			cameraYVel = 0;
		}
	
		cameraX = targetX;
		cameraY = targetY;
		
		return;
	}
	
	// If camera mode is "manual"
	if (option_cameraMode == 1) {
		if (override == 0 && stickyPlayer == false) {
			// Don't move the camera unless this is an override (e.g. loading a new level)
			targetX = cameraX;
			targetY = cameraY;
		}
	}
	
	// Adjust camera X and Y velocities to move towards target X and Y
	if (cameraX > targetX) cameraXVel --;
	if (cameraX < targetX) cameraXVel ++;
	
	if (cameraY > targetY) cameraYVel --;
	if (cameraY < targetY) cameraYVel ++;


	// Enforce maximum velocity limitations, slowing down smoothly
	if (cameraXVel > maxXVel) {
		cameraXVel = static_cast<float>(maxXVel);
		//cameraXVel =- 2;
		//if (cameraXVel < 0) cameraXVel = 0;
	}
	if (cameraXVel < -maxXVel) {
		cameraXVel = static_cast<float>(-maxXVel);
		//cameraXVel += 2;
		//if (cameraXVel > 0) cameraXVel = 0;	
	}

	if (cameraYVel > maxYVel) {
		cameraYVel = static_cast<float>(maxYVel);
		//cameraYVel =- 2;
		//if (cameraYVel < 0) cameraYVel = 0;
	}
	if (cameraYVel < -maxYVel) {
		cameraYVel = static_cast<float>(-maxYVel);
		//cameraYVel += 2;
		//if (cameraYVel > 0) cameraYVel = 0;	
	}

	
	// Slow down to zero velocity by the time we reach the target X
	if (cameraXVel != 0) {
		int xStoppingDistance = 0; // How much distance it will take for the camera to come to
		                          // a complete stop if it starts slowing down now.
		
		for (uint i = abs(static_cast<uint>(cameraXVel)); i > 0; i--) {
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
		
		for (uint i = abs(static_cast<uint>(cameraYVel)); i > 0; i--) {
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
	cameraX += static_cast<int>(cameraXVel);
	cameraY += static_cast<int>(cameraYVel);
	
}











// Returns correct surface for the block's/player's current face & direction.
SDL_Surface* block::GetSurface() {
	switch (type) {
		// Regular block
		case 0:
			return blockSurface;
			break;
		// First Player
		case 10:
			return playerSurface[GetSurfaceIndex()];
			break;
		// NPC
		case 11:
			return player2Surface[GetSurfaceIndex()];
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
		case 3:
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



void Notify(char *text) {
	delete [] notifyText;
	notifyText = new char[strlen(text) + 1];
	sprintf(notifyText, "%s", text);
	notifyFrames = FPS * 1.5;
}



// Reads the list of tileset directories to load the next/previous
// tileset directory
//     dir:
//       0 = previous
//       1 = next
void SelectTileset(bool dir) {
	// Save the old tileset
	char oldTileset[strlen(option_tileset)];
	strcpy(oldTileset, option_tileset);

	char msg[43]; // For holding the notification message
	char *line = NULL; // For holding the current line
	char prevLine[sizeof(option_tileset)]; // For holding the previous line
	prevLine[0] = '\0'; // Make it start with 0 length
	bool foundCurrentTileset = false;
	char tilesetName[16];

	// Determine the path to the list file
	char *path = new char[strlen(DATA_PATH) + strlen(TILE_PATH) + 5];
	sprintf(path, "%s%slist", DATA_PATH, TILE_PATH);

	FILE *file;
	file = fopen(path, "rt");

	if (file == NULL) {
		strncpy(msg, "ERROR: The tileset list file doesn't exist", sizeof(msg));
	}
	else {
		// Search through the list file until the current tileset's name is
		// found, at which point we look at the previous line if dir == 0, or
		// the next line if dir == 1
		while ((line = ReadLine(file, sizeof(option_tileset))) != NULL) {
			#ifdef DEBUG
				printf("line = '%s'\n", line);
			#endif

			// If the current tileset was found on the previous line (on
			// the previous loop)
			if (foundCurrentTileset) {
				// Store this line in option_tileset and exit the loop
				strcpy(option_tileset, line);
				break;
			}

			// If this line matches the currently displayed tileset
			if (strcmp(line, option_tileset) == 0) {
				#ifdef DEBUG
					printf("Line matches!");
				#endif
				foundCurrentTileset = true;

				if (dir == 0) {
					if (strlen(prevLine) > 0) {
						// Store the previous line in option_tileset
						strcpy(option_tileset, prevLine);
					}
					// exit the loop
					break;
				}
			}

			// Store this line as the "previous line"
			strcpy(prevLine, line);

			// Free heap memory
			delete [] line;
			line = NULL;
		}
		delete [] line;
		line = NULL;

		if (foundCurrentTileset == false) {
			strncpy(option_tileset, DEFAULT_TILESET, sizeof(option_tileset));
		}
		fclose(file);

		// Determine the path to the tileset's name file
		char *path = new char[strlen(DATA_PATH) + strlen(TILE_PATH) + strlen(option_tileset) + 6];
		sprintf(path, "%s%s%s/name", DATA_PATH, TILE_PATH, option_tileset);

		file = fopen(path, "rt");
		if (file == NULL) {
			// If there is no 'name' file in the tileset's directory
			strncpy(tilesetName, "[untitled]", sizeof(tilesetName));
		}
		else {
			// Read the first line from the name file
			char *temp = ReadLine(file, sizeof(tilesetName));
			strncpy(tilesetName, temp, sizeof(tilesetName));
			delete [] temp;
			fclose(file);
		}

		
		// Load the new tileset, and if it fails, go back to the old
		// one
		if (LoadTileset(option_tileset) == 0) {
			sprintf(msg, "TILESET: %s", tilesetName);
		}
		else {
			sprintf(msg, "Error loading tileset \"%s\"", option_tileset);
			strcpy(option_tileset, oldTileset);
			LoadTileset(option_tileset);
		}
	}

	Notify(msg);
}


// Loads all the tiles from the tiles.bmp in the specified tilesetDir
char LoadTileset(char *tilesetDir) {
	// Y position where the tiles start
	int tilesY = 256;

	// Free all old surfaces
	UnloadTileset();

	// Set the path
	char path[strlen(DATA_PATH) + strlen(TILE_PATH) + strlen(tilesetDir) + 11];
	sprintf(path, "%s%s%s/tiles.bmp", DATA_PATH, TILE_PATH, tilesetDir);

	// The big surface holding all the tiles
	SDL_Surface *bigSurf = FillSurface(path, 0);
	if (bigSurf == NULL) {
		return 1;
	}
		
	// Lock the surface (for subsequent GetPixel calls)
	SDL_LockSurface(bigSurf);
	uint transColor = SDL_MapRGB(bigSurf->format, 0xff, 0x00, 0xff);

	// Detect the background height
	for (int y = tilesY - 1; y > 0; y--) {
		if (GetPixel(bigSurf, 0, y) != transColor) {
			bgH = y + 1;
			break;
		}
	}

	// Detect the background width
	for (int x = bigSurf->w - 1; x > 0; x--) {
		if (GetPixel(bigSurf, x, 0) != transColor) {
			bgW = x + 1;
			break;
		}
	}

	// Unlock the surface
	SDL_UnlockSurface(bigSurf);

	// Blit the background part to a new background surface
	bgSurface = MakeSurface(bgW, bgH);
	ApplySurface(0, 0, bigSurf, bgSurface);

	SDL_Surface *tempSurf = NULL;
	uint tileNum = 0;

	// Move the x,y offset of the bigSurf to put each tile in the correct position
	for (int y = -tilesY; y >= -(bigSurf->h - TILE_H); y -= TILE_H) {
		for (int x = 0; x >= -(bigSurf->w - TILE_W); x -= TILE_W) {
			// Prepare a surface for this tile
			tempSurf = MakeSurface(TILE_W, TILE_H);
			
			// Blit the current section of bigSurf onto this tile's
			// surface
			ApplySurface(x, y, bigSurf, tempSurf);

			// Point the correct surface to this temporary surface
			if (tileNum == 0) {
				blockSurface = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum == 1) {
				spikeSurface = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 2 && tileNum <= 2 + (NUM_BRICK_SURFACES - 1)) {
				brickSurface[tileNum - 2] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 7 && tileNum <= 7 + (NUM_EXIT_FRAMES - 1)) {
				exitSurface[tileNum - 7] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum == 10) {
				itemSurface[tileNum - 10] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 11 && tileNum <= 11 + (NUM_TORCH_FLAMES - 1)) {
				torchSurface[tileNum - 11] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 19 &&
			         tileNum <= 19 + (NUM_TELEPAD_STATES - 1))
			{
				telepadSurface[tileNum - 19] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 22 &&
			         tileNum <= 22 + (NUM_PLAYER_SURFACES - 1))
			{
				playerSurface[tileNum - 22] = tempSurf;
				tempSurf = NULL;
			}
			else if (tileNum >= 37 && tileNum <= 37 + (NUM_PLAYER_SURFACES - 1)) {
				player2Surface[tileNum - 37] = tempSurf;
				tempSurf = NULL;
			}

			if (tempSurf != NULL) SDL_FreeSurface(tempSurf);
			tileNum ++;
		}
	}

	SDL_FreeSurface(bigSurf);

	return 0;
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
	bgSurface = NULL;

	SDL_FreeSurface(blockSurface);
	blockSurface = NULL;

	SDL_FreeSurface(spikeSurface);
	spikeSurface = NULL;

	for (uint i = 0; i < NUM_PLAYER_SURFACES; i++) {
		SDL_FreeSurface(playerSurface[i]);
		playerSurface[i] = NULL;

		SDL_FreeSurface(player2Surface[i]);
		player2Surface[i] = NULL;
		
	}
	for (uint i = 0; i < NUM_BRICK_SURFACES; i++) {
		SDL_FreeSurface(brickSurface[i]);
		brickSurface[i] = NULL;
	}	
	for (uint i = 0; i < NUM_TORCH_FLAMES; i++) {
		SDL_FreeSurface(torchSurface[i]);
		torchSurface[i] = NULL;
	}
	for (uint i = 0; i < NUM_TELEPAD_STATES; i++) {
		SDL_FreeSurface(telepadSurface[i]);
		telepadSurface[i] = NULL;
	}
	for (uint i = 0; i < NUM_EXIT_FRAMES; i++) {
		SDL_FreeSurface(exitSurface[i]);
		exitSurface[i] = NULL;
	}
	for (uint i = 0; i < NUM_ITEM_TYPES; i++) {
		SDL_FreeSurface(itemSurface[i]);
		itemSurface[i] = NULL;
	}
}




// Makes a surface with the correct BPP, tranparent color, etc. for this game
SDL_Surface* MakeSurface(int width, int height) {
	SDL_Surface *temp = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			width,
			height,
			SCREEN_BPP,
			screenSurface->format->Rmask,
			screenSurface->format->Gmask,
			screenSurface->format->Bmask,
			//screenSurface->format->Amask
			0); 
	
	SDL_Surface *surface = NULL;
	
	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	temp = NULL;
	
	// Make bright pink #ff00ff transparent
	uint colorKey = SDL_MapRGB(surface->format, 0xff, 0x00, 0xff);
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorKey);

	// Fill surface with transparent color
	SDL_FillRect(surface, NULL, colorKey);
	/*
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			PutPixel(surface, i, j, colorKey);
		}
	}
	*/

	return surface;
}



// Surface must be locked before calling this:
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x;

	*p = static_cast<unsigned char>(pixel);
}



void DrawBackground() {
	const float parallaxConst = .5f;
	static int bgX = 0, bgY = 0;
	static uint bgTimer = 0;

	switch (option_background) {
		// Draw blackness and exit if background is disabled
		case 0:
			SDL_FillRect(screenSurface, NULL, 0x000000);
			return;
			break;

		// Position the parallax background
		case 2:
			bgX = (-cameraX * parallaxConst);
			if (bgX >= bgW) bgX %= bgW;
			if (bgX < 0) bgX = bgW - (abs(bgX) % bgW);

			bgY = (-cameraY * parallaxConst);
			if (bgY >= bgH) bgY %= bgH;
			if (bgY < 0) bgY = bgH - (abs(bgY) % bgH);

			break;
	
		// Scroll the background
		case 3:
			if (SDL_GetTicks() > bgTimer + 60) {
				bgX += 1;
				if (bgX >= bgW) bgX = 0;

				bgY += 1;
				if (bgY >= bgH) bgY = 0;

				bgTimer = SDL_GetTicks();
			}
			break;
	}

	// Draw the background
	for (int b = -bgH; b < -bgH + (((SCREEN_H / bgH) + 2) * bgH); b += bgH) {
		for (int a = -bgW; a < -bgW + (((SCREEN_W /bgW) + 2) * bgW); a += bgW) {
			ApplySurface(a + bgX, b + bgY, bgSurface, screenSurface);
		}
	}
}


void Render (const char flags) {
	static uint torchTimer = 0;
	static uint doorFrame, doorFramePause;
	static uint timerPos;
	static int levelTextX = 10;
	static signed char notifyY = -FONT_H;
	uint i;
	

	//if (LockSurface(screenSurface) == false) return;

	if (flags & RENDER_MOVECAMERA) {
		MoveCamera();
		CenterCamera(0);
	}
		
	/*** Background ***/
	if (flags & RENDER_BG) {
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
	if ((flags & RENDER_ANIMATE) && SDL_GetTicks() > torchTimer + 5) {
		for (i = 0; i < numTorches; i++) {
			torches[i].FlickerFlame();
	    	}
		torchTimer = SDL_GetTicks();
	}
	// Draw torches
	for (i = 0; i < numTorches; i++) {
		ApplySurface(torches[i].GetX() - cameraX, torches[i].GetY() - cameraY, torchSurface[torches[i].GetFlame()], screenSurface);
	}
	
	
	
	
	/*** BLOCKS ***/
	// Deactivate stickyPlayer when he's lined up in the new level's position.
	if (stickyPlayer && (flags & RENDER_ANIMATE) && blocks[0].GetX() - cameraX <= stickyPlayerX && blocks[0].GetY() - cameraY == stickyPlayerY) {
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

	// Move mouths of speaking players
	if (flags & RENDER_ANIMATE) AnimateSpeech();
	
	for (i = 0; i < numBlocks; i++) {
		if (blocks[i].GetType() >= 0) {
			if (flags & RENDER_ANIMATE) blocks[i].Animate();
			
			if (i != 0 || !stickyPlayer) {
				ApplySurface(blocks[i].GetX() - cameraX, blocks[i].GetY() - cameraY, blocks[i].GetSurface(), screenSurface);
			}
		}
	}


	/*** TELEPADS ***/
	for (i = 0; i < numTelepads; i++) {
		telepads[i].Render(flags & RENDER_ANIMATE ? true : false);
	}


	// Draw player on top of everything when sticky
	if (stickyPlayer) {
		ApplySurface(stickyPlayerX,
		             stickyPlayerY,
			     blocks[0].GetSurface(),
			     screenSurface);
	}


	//UnlockSurface(screenSurface);
	//PutPixel(screenSurface, cameraX + 10, cameraY + 10, SDL_MapRGB(screenSurface->format, 0x00, 0xff, 0x00));

	/** Draw any Speech "Bubbles" above players' heads *****/
	DrawBubbles((flags & RENDER_ANIMATE) ? true : false);

	char message[128];
	
	/** Notification Message at top of screen **/
	if (notifyText != NULL) {
		notifyFrames--;
		if (notifyFrames <= FONT_H) {
			if (notifyFrames <= FONT_H) notifyY -= (FPS / 4);
		}
		else if (notifyY < 4) {
			notifyY += (FPS / 4);
			if (notifyY > 4) notifyY = 4;
		}
		if (notifyY <= -FONT_H) {
			delete [] notifyText;
			notifyText = NULL;
		}
		else {
			txt note(SCREEN_W / 2, notifyY, notifyText);
			note.Center();
			note.Wrap();
			note.Render();
		}
	}


	/** Draw Level # **/
	sprintf(message, "Level %d", currentLevel);

	// If we are selecting a level, center the level text
	if (selectingLevel) {
		levelTextX = (SCREEN_W / 2) - (GetTextW(message, 0) / 2);
	}
	else {
		// Slide the text to the left
		levelTextX -= FPS * 2;
		if (levelTextX < 10) levelTextX = 10;

		DrawText(levelTextX, SCREEN_H - FONT_H - 4, message);
		
		// Determine position of "Replay"
		i = levelTextX + GetTextW(message, 0) + FONT_W;
	}

	
	/** Draw Replay Notification ****/
	if (showingReplay) {
		sprintf(message, "Replay");
		DrawText(i, SCREEN_H - FONT_H - 4, message, 1);
	}


	/** Draw Timer ****/
	if (option_timerOn) {
		if (levelTimeRunning) {
			int min = static_cast<int>(levelTime / 60);
			int sec = levelTime % 60;
			sprintf(message, "%02d:%02d", min, sec);
			if (timerPos < 76) {
				timerPos += FPS;
				if (timerPos > 76) timerPos = 76;
			}
			DrawText(SCREEN_W - timerPos, SCREEN_H - FONT_H - 4, message, 1);
		}
		else {
			timerPos = 0;
		}
	}

	/** Screen Update ****/
	if ((flags & RENDER_UPDATESCREEN)) {
		UpdateScreen();
	}
}



void UpdateScreen() {
	// Tell SDL to update the whole screenSurface
	SDL_UpdateRect(screenSurface, 0, 0, 0, 0);
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

