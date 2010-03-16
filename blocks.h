/*
 *   Copyright 2009 Andrew Anderson <www.billamonster.com>
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


int *playerBlock = NULL; 	// Stores which block the player is currently picking
				// up and waiting on until he can move again.
				// Not a member variable so as to save memory, since
				// in most levels only one block is a player.

// Blocks move [via Physics()].  Players are just a special type of block.
class block {
	public:
		/** Constructor **/
		block():
			xMoving(0),
			yMoving(0),
			w(TILE_W),
			h(TILE_H),
			dir(0),
			type(0),
			face(0),
			strong(0),
			won(0),
			path(NULL)
			{};

		/** Copy Assignment Operator **/
		block& operator = (const block& other);
		
		/** Destructor **/
		~block();
	
		/** Get **/
		int GetX() const { return x; };
		int GetY() const { return y; };
		
		int GetXSpeed() const { return xSpeed; };
		int GetYSpeed() const { return ySpeed; };
		
		int GetXMoving() const { return xMoving; };
		int GetYMoving() const { return yMoving; };
		
		int GetW() const { return w; };
		int GetH() const { return h; };
		
		char GetDir() const { return dir; };
		char GetType() const { return type; };
		char GetFace() const { return face; };
		char GetStrong() const { return strong; };
		char GetWon() const { return won; };
		bool GetDidPhysics() const { return didPhysics; };
		bool GetMoved() const { return moved; };
		char* GetPath() const { return path; };
		int GetPathLength() const;
		
		/** Set **/
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
		void SetWon(char w) { won = w; };
		void SetDidPhysics(bool d) { didPhysics = d; };
		void SetMoved(bool m) { moved = m; };
		void SetPath(const char *p);
		void SetPath(char *p);
		
		/** Others **/
		void Animate();              // Change block/player face (blinking, etc.)
		
		int Climb(char direction);  // Makes the player climb over the obstacle in the
		                             // specified direction (same as dir)
		                             //
		                             // returns false if the block has something immediately
		                             // above it blocking its path
		
		void Physics();              // Moves the block according to physics (in physics.cpp)
		
		void PostPhysics();	     // Decrements the xMoving and yMoving
		
		SDL_Surface* GetSurface();   // in graphics.cpp
		int GetSurfaceIndex();       // in graphics.cpp
		
		int GetYOffset();            // Find how much player has sunk down from carrying something.
		
		bool OnSolidGround();        // This recursive function will return true only if the block
		                             // is making contact with a permanent object, either directly
		                             // or indirectly (e.g. sitting on a pile of blocks which is
		                             // sitting on a brick)
	private:
		int x, y;
		int xSpeed, ySpeed;
		int xMoving, yMoving;   // Stores current path movement
					// progress, e.g. xMoving = 16
					// would mean this block needs
					// to move 16 more pixels to the
					// right until it finishes its
					// path.
		
		int w, h;       // width, height
		
		char dir;       // Direction the player is facing:
		                // 0 left, 1 right, 2 at camera, 3 dead
		
		char type;      // 0 regular block
		                // >= 10 player
		                //  < 0  temporarily disabled (for teleportation animation)
		                //  -100 permanently disabled for rest of level
		
		char face;      // Players:            Blocks:	
		                // 0 = normal          0 = normal
		                // 1 = mouth open
		                // 2 = blinking
		                // 3 = happy mouth
		                // 4 = scared mouth

		
		char strong;    // 0 = Regular strength (can only lift/push 1 block at a time)
		                // 1 = Strong block (used for a strong player and for the block
		                //     when a strong player acts upon it.)
		                // 2 = Pushed by a strong block (temporary, means this block
		                //     was pushed by a strong block, and can in turn push other
		                //     blocks, but this is reset to 0 at the end of the frame.
		
		char won;       // 0 = Player has not won the level yet
		                // 1 = Player reached the exit, and is waiting for door to open
		                // 2 = Player is continuing to walk toward (now open) door
		                // 3 = Player is inside door
		
		bool didPhysics;  // Did the block have physics
		                  // applied to it this frame yet?
					
		bool moved;       // Did the block move this frame yet?
		                  // Blocks are only allowed to move once per frame
		                  // (no diagonals, etc.)
		
		char *path;     // Stores a path that the block will
		                // follow, e.g. "-16y16x" would be used
		                // for picking up a block if the player
		                // were facing left.  The block will
		                // move up 16px, then right 16px.
		                //
				// "100s" would make the block sleep for
				// 100 milliseconds (useful for NPCs)
};


// Copy Assignment Operator
block& block::operator = (const block& other) {
	x = other.GetX();
	y = other.GetY();
	xMoving = other.GetXMoving();
	yMoving = other.GetYMoving();
	w = other.GetW();
	h = other.GetH();
	dir = other.GetDir();
	type = other.GetType();
	face = other.GetFace();
	strong = other.GetStrong();
	won = other.GetWon();
	didPhysics = other.GetDidPhysics();
	moved = other.GetMoved();
	
	delete [] path;
	path = NULL;
	if (other.GetPathLength() > 0) {
		path = new char[other.GetPathLength() + 1];
		strcpy(path, other.GetPath());
	}
	
	return *this;
}

// Destructor
block::~block() {
	delete [] path;
	path = NULL;
}


void block::SetPath(const char *p) {
	// Clean up old text data
	delete [] path;
	path = NULL;

	// Store the const char * in a char *
	char tempPath[strlen(p) + 1];
	strcpy(tempPath, p);
	
	// Hand off the char * to SetPath()
	SetPath(tempPath);
}



void block::SetPath(char *p) {
	// Clean up old text data
	delete [] path;
	path = NULL;

	path = new char[strlen(p) + 1];

	strcpy(path, p);
}

int block::GetPathLength() const {
	if (path == NULL) return 0;
	return static_cast<int>(strlen(path));
}



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
