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


typedef unsigned int uint;
typedef unsigned char uchar;

/******* EXTERNAL INCLUDES *******/
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>



/******* GLOBAL CONSTANTS *******/
const unsigned char TILE_W = 16, TILE_H = 16;
const unsigned short SCREEN_W = 640, SCREEN_H = 400, SCREEN_BPP = 8;
const char FPS = 30;

const unsigned char NUM_PLAYER_SURFACES = 15;
const unsigned char NUM_BRICK_SURFACES = 5;
const unsigned char NUM_TORCH_FLAMES = 8;
const unsigned char NUM_TELEPAD_STATES = 3;
const unsigned char NUM_EXIT_FRAMES = 3;
const unsigned char NUM_ITEM_TYPES = 1;
const unsigned char NUM_LEVEL_SETS = 3;
const unsigned char TELEPAD_H = 4;
const char GAME_TITLE[] = "BLOCKLING";
//const char DATA_PATH[] = "../data/"; // defined in Makefile now
const char FONT_PATH[] = "font/";
const char MUSIC_PATH[] = "music/";
const char SOUND_PATH[] = "sound/";
const char LEVEL_PATH[] = "levels/";
const char TILE_PATH[] = "tiles/";
const char DEFAULT_TILESET[] = "default";
const char DEFAULT_LEVELSET[] = "default";
const char SETTINGS_PATH[] = "./";
#ifdef _WIN32
	const char TEMP_PATH[] = "./";
	const char SETTINGS_FILE[] = "blockling.cfg";
#else
	const char TEMP_PATH[] = "/tmp/";
	const char SETTINGS_FILE[] = ".blocklingrc";
#endif



/******* GLOBAL VARIABLES (part 1) ********/
SDL_Surface *screenSurface = NULL;
SDL_Surface *brickSurface[NUM_BRICK_SURFACES] = {NULL};
SDL_Surface *blockSurface = NULL;
SDL_Surface *torchSurface[NUM_TORCH_FLAMES] = {NULL};
SDL_Surface *spikeSurface = NULL;
SDL_Surface *itemSurface[NUM_ITEM_TYPES] = {NULL};
SDL_Surface *telepadSurface[NUM_TELEPAD_STATES] = {NULL};
SDL_Surface *exitSurface[NUM_EXIT_FRAMES] = {NULL};
SDL_Surface *bgSurface = NULL;
SDL_Surface *playerSurface[NUM_PLAYER_SURFACES] = {NULL};
SDL_Surface *player2Surface[NUM_PLAYER_SURFACES] = {NULL};

int blockXSpeed;
int blockYSpeed;
int blockXGravity;
int blockYGravity;

uint numBlocks;
uint numBricks;
uint numPlayers;
uint numTorches;
uint numSpikes;
uint numTelepads;
uint numItems;

uint currentLevel;
bool selectingLevel; // If we are currently in the menu for selecting a level
bool levelTimeRunning; // If the level timer is running
uint levelTime;	//How long the player has taken on this level (in milliseconds)
uint levelTimeTick; // For tracking how long each frame takes
char wonLevel;	// 0 = No player has won level yet
		// 1 = A player is opening the door (flag for Render to animate the door)
		// 2 = The first player has won (the level is over) and is walking into the door
		// 3 = The first player has finished walking into the door
		// 4 = The player now becomes sticky and the level scrolls into place under him

int exitX, exitY;

int cameraX, cameraY;
float cameraXVel, cameraYVel;
int cameraTargetX, cameraTargetY; // Camera will follow this position
uint manualCameraTimer; // For making sure camera will not automatically move if it has been manually moved recently
int levelX, levelY, levelW, levelH; // Used to provide info for camera to know how far to move

// For making the player stay put when camera is moving to the next level
bool stickyPlayer;
int stickyPlayerX, stickyPlayerY; // The screen coordinates
int stickyPlayerOrigX, stickyPlayerOrigY; // The player's game coordinates



/******* HEADER INCLUDES *******/
/* main.cpp */
char Init();
void DeInit();

#include "block.h"
#include "graphics.h"
#include "input.h"
#include "font.h"
#include "sound.h"
#include "menu.h"	// Needs sound.h
#include "physics.h"
#include "settings.h"
#include "replay.h"
#include "speech.h"




/******* SMALL CLASSES *******/
// Bricks are stationary "land"
class brick {
	public:
		// Constructor
		brick():
			w(TILE_W),
			h(TILE_H)
			{};
	
		// Get
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetW() const { return w; };
		int GetH() const { return h; };
		
		int GetType() const { return type; };
	
		// Set
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void SetType(char t) { type = t; };
		
		// Other
		SDL_Surface* GetSurface(); // in graphics.cpp
	private:
		int x, y;
		int w, h;
		char type; // 0 wall (vertical sections & behind torches)
		           // 1 left side
		           // 2 middle
		           // 3 right side
		           // 4 singe piece of land
};



class torch {
	public:
		/** Constructor **/
		torch():flame(static_cast<char>(rand() % NUM_TORCH_FLAMES)) {};
	
		/** Get **/
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetFlame() const { return flame; };
	
		/** Set **/
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void SetFlame(char f) { flame = f; };
		
		/** Other **/
		void FlickerFlame();
	private:
		int x, y;
		int flame;	// Current frame of animated flame
};

void torch::FlickerFlame() {
	// Default
	//if (option_tileset == 0) {
		if (rand() % 2 == 0) {
			if (rand() % 2 == 0) {
				flame -= (rand() % 2) + 1;
			}
			else {
				flame += (rand() % 2) + 1;
			}
		}

		if (flame < 0) flame += 2;
		if (flame > NUM_TORCH_FLAMES - 1) flame -= 2;
	//}
	// Sci-Fi
	//else {
		//if (rand() % 4 == 0) {
			//flame = rand() % NUM_TORCH_FLAMES;
		//}
	//}
}




class spike {
	public:
		/** Get **/
		int GetX() const { return x; };
		int GetY() const { return y; };
	
		/** Set **/
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
	private:
		int x, y;
};




// Items are "power-ups" that have no physics.  They disappear when
// a player touches them, and they cause something to happen.
class item : public spike {
	private:
		char type;	// Types
				// 0 spinach
};




/******* MORE GLOBAL VARIABLES (objects of the classes we just had declared) *******/
int bgW, bgH;  // For scrolling background

brick *bricks = NULL;
block *blocks = NULL;
torch *torches = NULL;
spike *spikes = NULL;

bool physicsStarted; // Whether the physics have started running for the current level




/*** MORE INCLUDES ***/
#include "telepad.h"	// Needs *blocks
telepad *telepads = NULL;

#include "game.h"


/*** GLOBAL VARIABLES THAT REFERENCE STUFF ABOVE ***/
// Undo stuff
uint maxUndo;
block **undoBlocks = NULL;
telepad **undoTelepads = NULL;	// Needs telepad class



/*** CPP INCLUDES ***/
#include "font.cpp"
#include "game.cpp"
#include "graphics.cpp"
#include "input.cpp"
#include "menu.cpp"
#include "physics.cpp"
#include "sound.cpp"
#include "settings.cpp"
#include "speech.cpp"
