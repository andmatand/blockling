/*
 *   main.h
 *      
 *   Copyright 2009 Andrew Anderson <billamonster.com>
 *      
 *   This file is part of Blockman
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

#define DEBUG

/******* PRE-INCLUDES *******/
#if defined(_MSC_VER)
	#include "SDL.h"
#else
	#include "SDL/SDL.h"
#endif
#include <iostream>
#include <string>
#include <sstream>




/******* CONSTANTS *******/
const unsigned char TILE_W = 16, TILE_H = 16;
const unsigned short SCREEN_W = 640, SCREEN_H = 400, SCREEN_BPP = 8;
const char FPS = 30;

const unsigned char NUM_PLAYER_SURFACES = 15;
const unsigned char NUM_TORCH_FLAMES = 8;
const unsigned char NUM_TELEPAD_STATES = 3;
const unsigned char TELEPAD_H = 4;
const uint NUM_GAME_KEYS = 6;
const uint NUM_PLAYER_KEYS = 5;
const std::string GAME_TITLE = "Blockman";




/******* GLOBAL VARIABLES (part 1) ********/
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

uint currentLevel;

int exitX, exitY;
int cameraX, cameraY;
int cameraTargetX, cameraTargetY; // Camera will follow this position
int levelX, levelY, levelW, levelH; // Used to provide info for camera to know how far to move


/******* FUNCTION PROTOTYPES *******/
/* main.cpp */
void Init();

/* game.cpp */
void Game();
void Undo(char action);

/* graphics.cpp */
void CenterCamera(char instant);
SDL_Surface* FillSurface(const char* file, bool transparent);
void Render(bool update);

/* input.cpp */
void Input();

/* physics.cpp */
int BlockNumber(int x, int y, int w, int h);
int BoxContents(int x, int y, int w, int h);
bool BoxOverlap (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool OnSolidGround(int b);




/******* CLASSES *******/
/* Blocks move.  Players are just a special type of block */
class block {
	public:
		/// Constructor ///
		block():
			xMoving(0),
			yMoving(0),
			w(TILE_W),
			h(TILE_H),
			dir(0),
			type(0),
			face(0),
			strong(0),
			path("")
			{};
	
		/// Get ////
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetXSpeed() const { return xSpeed; };
		int GetYSpeed() const { return ySpeed; };
		
		int GetXMoving() const { return xMoving; };
		int GetYMoving() const { return yMoving; };
		
		int GetW() const { return w; };
		int GetH() const { return h; };
		
		int GetDir() const { return dir; };
		char GetType() const { return type; };
		char GetFace() const { return face; };
		char GetStrong() const { return strong; };
		bool GetDidPhysics() const { return didPhysics; };
		bool GetMoved() const { return moved; };
		std::string GetPath() const { return path; };
		
		/// Set ///
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
	
		void SetXSpeed(int s) { xSpeed = s; };
		void SetYSpeed(int s) { ySpeed = s; };
		
		void SetXMoving(int s) { xMoving = s; };
		void SetYMoving(int s) { yMoving = s; };
		
		void SetWH(int width) { w = width; };
		void SetH(int height) { h = height; };
		
		void SetDir(char d) { dir = d; };
		void SetType(char t) { type = t; };
		void SetFace(char f) { face = f; };
		void SetStrong(char s) { strong = s; };
		void SetDidPhysics(bool d) { didPhysics = d; };
		void SetMoved(bool m) { moved = m; };
		void SetPath(std::string p) { path = p; };
		
		/// Others ///
		void Animate();		// Change block/player face (blinking, etc.)
		
		void Climb(char direction);  // Makes the player climb over the obstacle in the
					     // specified direction (same as dir)
		void Physics();		// Moves the block according to physics
		void PostPhysics();	// Decrements the xMoving and yMoving
		
		SDL_Surface* GetSurface();	// in graphics.cpp
		int GetSurfaceIndex();		// in graphics.cpp
		
		int GetYOffset();	// Find how much player has sunk down from carrying something.
		
		bool OnSolidGround();   // This recursive function will return true only if the block
					// is making contact with a permanent object, either directly
					// or indirectly (e.g. sitting on a pile of blocks which is
					// sitting on a brick)
	private:
		int x, y;
		int xSpeed, ySpeed;
		int xMoving, yMoving;	// Stores current path movement
					// progress, e.g. xMoving = 16
					// would mean this block needs
					// to move 16 more pixels to the
					// right until it finishes its
					// path.
		
		int w, h;	// width, height
		
		char dir;	// Direction the player is facing:
				// 0 left, 1 right, 2 at camera, 3 dead
		
		char type;	// 0 regular block, 1 player
		
		char face;	/* Players */           /* Blocks */	
				// 0 = normal		0 = normal
				// 1 = mouth open
				// 2 = blinking
				// 3 = happy mouth
				// 4 = scared mouth

		
		char strong;	// 0 Regular strength (can only lift/push 1 block at a time)
				// 1 Strong block (used for a strong player and for the block
				//   when a strong player acts upon it.
				// 2 Pushed by a strong block (temporary, means this block
				//   was pushed by a strong block, and can in turn push other
				//   blocks, but this is reset to 0 at the end of the frame.
		
		bool didPhysics;	// Did the block have physics
					// applied to it this frame yet?
					
		bool moved;		// Did the block move this frame yet?
					// Blocks are only allowed to move once per frame
					// (no diagonals, etc.)
		
		std::string path;	// Stores a path that the block will
					// follow, e.g. "-16y16x" would be used
					// for picking up a block if the player
					// were facing left.  The block will
					// move up 16px, then right 16px.
					//
					// "100s" would make the block sleep for
					// 100 milliseconds (useful for NPCs)
};




int block::GetYOffset() {
	int a;
	
	if (y < 0) {
		a = abs(y % TILE_H);
		if (a > 0) a = TILE_H - a;
	}
	else {
		a = y % TILE_H;
	}
	
	return a;
}




/* Bricks are stationary "land" */
class brick {
	public:
		/// Constructor ///
		brick():
			w(TILE_W),
			h(TILE_H)
			{};
	
		/// Get ///
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetW() const { return w; };
		int GetH() const { return h; };

		
		int GetType() const { return type; };
	
		/// Set ///
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void SetType(char t) { type = t; };
	private:
		int x, y;
		int w, h;
		char type;	// 0 normal brick
};



class torch {
	public:
		/// Constructor ///
		torch():flame(static_cast<char>(rand() % NUM_TORCH_FLAMES)) {};
	
		/// Get ///
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetFlame() const { return flame; };
	
		/// Set ///
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void SetFlame(char f) { flame = f; };
		
		/// Other ///
		void FlickerFlame();
	private:
		int x, y;
		char flame;	// Current frame of animated flame
};

void torch::FlickerFlame() {
	if (rand() % 2 == 0) {
		flame -= static_cast<char>((rand() % 2) + 1);
	}
	else {
		flame += static_cast<char>((rand() % 2) + 1);
	}
	
	if (flame < 0) flame += 2;
	if (flame > NUM_TORCH_FLAMES - 1) flame -= 2;
}




class spike {
	public:
		/// Get ///
		int GetX() const { return x; };
		int GetY() const { return y; };
	
		/// Set ///
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
	private:
		int x, y;
};




// Each telepad is actually a pair of two telepads
class telepad {
	public:
		/// Get ///
		int GetX1() const { return x1; };
		int GetY1() const { return y1; };
		int GetX2() const { return x2; };
		int GetY2() const { return y2; };

		int GetOccupant1(); // Returns which block is currently on the first telepad
		int GetOccupant2(); // Returns which block is currently on the second telepad
	
		/// Set ///
		void SetX1(int xPos) { x1 = xPos; };
		void SetY1(int yPos) { y1 = yPos; };
		void SetX2(int xPos) { x2 = xPos; };
		void SetY2(int yPos) { y2 = yPos; };
		
		/// Other ///
		bool NeedsToTeleport();	// in physics.cpp
		void Teleport(); 	// in physics.cpp
		SDL_Surface* GetSurface(); // Returns correct surface, based on state (in graphics.cpp)

	private:
		int x1, y1;	// First telepad
		int x2, y2;	// Second telepad
		char state;	// 0 = off, 1 = waiting to teleport, 2 = teleporting
		int occupant1;	// Block on first telepad
		int occupant2;	// Block on second telepad
		bool occupant1Teleported; // Records if occupant1 teleported yet
		bool occupant2Teleported; // Records if occupant2 teleported yet
};

int telepad::GetOccupant1() {
	return BlockNumber(x1, y1 + 11, TILE_W, 1);
}

int telepad::GetOccupant2() {
	return BlockNumber(x2, y2 + 11, TILE_W, 1);
}






/******* GLOBAL VARIABLES (part 2) *******/
SDL_Surface *screenSurface;
SDL_Surface *brickSurface;
SDL_Surface *blockSurface;
SDL_Surface *torchSurface[NUM_TORCH_FLAMES];
SDL_Surface *spikeSurface;
SDL_Surface *telepadSurface[NUM_TELEPAD_STATES];
SDL_Surface *exitSurface;
SDL_Surface *bgSurface;
SDL_Surface *playerSurface[NUM_PLAYER_SURFACES];

int bgX, bgY, bgW, bgH;  // For scrolling background

brick *bricks = NULL;
block *blocks = NULL;
telepad *telepads = NULL;
torch *torches = NULL;
spike *spikes = NULL;

// Undo stuff
uint maxUndo = 20; // Maximum number of undo levels
block **undoBlocks = NULL;
bool justUndid;

// Keyboard stuff
struct keyBinding {
	SDLKey sym;	// Key name (e.g. SDLK_LEFT)
	SDLMod mod;	// Modifier (e.g. KMOD_LSHIFT) - must be pushed along with 'sym' for key to be 'on'
	bool on; 	// If the key is pushed
	uint timer;	// Used for repeat rate
};

keyBinding gameKeys[NUM_GAME_KEYS];	// Game's keymap (for Quit, Undo, etc.)
keyBinding *playerKeys; 		// Players' keymap (for moving players).  Initialized in LoadLevel()




uint lastTick = 0; // For FPS timing

SDL_Event event;





/******* POST-INCLUDES *******/
#include "game.cpp"
#include "graphics.cpp"
#include "input.cpp"
#include "physics.cpp"
