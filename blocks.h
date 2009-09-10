/*
 *   blocks.h
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


// Blocks move [via Physics()].  Players are just a special type of block.
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
			won(0),
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
		char GetWon() const { return won; };
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
		void SetWon(char w) { won = w; };
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
		
		char type;	// 0 regular block
				// >= 10 player
				//  < 0  temporarily disabled (for teleportation animation)
				//  -100 permanently disabled for rest of level
		
		char face;	/* Players */           /* Blocks */	
				// 0 = normal		0 = normal
				// 1 = mouth open
				// 2 = blinking
				// 3 = happy mouth
				// 4 = scared mouth

		
		char strong;	// 0 = Regular strength (can only lift/push 1 block at a time)
				// 1 = Strong block (used for a strong player and for the block
				//     when a strong player acts upon it.
				// 2 = Pushed by a strong block (temporary, means this block
				//     was pushed by a strong block, and can in turn push other
				//     blocks, but this is reset to 0 at the end of the frame.
		
		char won;	// 0 = Player has not won the level yet
				// 1 = Player reached the exit, and is waiting for door to open
				// 2 = Player is continuing to walk toward (now open) door
				// 3 = Player is inside door
		
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



// block:Physics() is in physics.cpp
