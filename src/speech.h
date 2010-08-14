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


/** Function prototypes ****/
void AnimateSpeech();
void ClearBubbles();
void ClearSpeechTriggers();
void DrawBubbles(bool decrementTTLs);
void HelpSpeak(int block,const char* text, bool polite = false,
	char postDir = -1);
void HelpSpeak(int block, char* text, bool polite = false, char postDir = -1);
void Speak(int block, const char* text, bool polite = false, char postDir = -1);
void Speak(int block, char* text, bool polite = false, char postDir = -1);
void SpeechTrigger(int block, char* text, int targetFrames, char type, int id);
void SpeechTrigger(int block, const char* text, int targetFrames, char type,
                   int id);


/** Classes ****/
// A speech "bubble"
class bubble {
	public:
		// Constructor
		bubble():
			ttl(0),
			next(NULL)
			{}
		
		// Destructor
		~bubble() {};
		
		// Set
		void SetText(char *str);
		void SetBlock(int aBlock) { block = aBlock; };
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		void SetTTL(uint frames) { ttl = frames; };
		void SetPolite(bool itCanWait) { polite = itCanWait; };
		void SetPostDir(char turnThisWay) { postDir = turnThisWay; };
		void SetNext(bubble* bp) { next = bp; };

		// Get
		txt* GetText() { return &text; };
		int GetBlock() const { return block; };
		int GetX() const { return x; };
		int GetY() const { return y; };
		uint GetTTL() const { return ttl; };
		bool GetPolite() const { return polite; };
		char GetPostDir() const { return postDir; };
		bubble* GetNext() const { return next; };
	
	private:
		txt text;       // txt object which contains the text

		int block;      // The block/player number of the block/player
		                // speaking

		int x;          // Text is centered around this x
		int y;          // Top of text

		uint ttl;       // Time to live (in frames)

		bubble* next;   // Pointer to next bubble

		bool polite;    // true  = The bubble will go to the
		                //         end of the queue and wait its turn.
		                // false = The bubble will delete any current
		                //         speech bubbles from the queue and
		                //         begin instantly. 

		char postDir;   // Direction for player to turn after he's
		                // finished speaking (-1 = disabled)
};

void bubble::SetText(char *str) {
	text.SetText(str);
}



// Speech "trigger"
// If a trigger is held (e.g. the player is standing in a certain spot) for the
// target number of *consecutive* frames, then a speech bubble is generated
class trigger {
	public:
		// Constructor
		trigger():
			id(-1),
			frames(0)
			{
				active = false;
			}
		
		// Destructor
		~trigger() {};
		
		void SetID(int anID) { id = anID; };
		void SetFrames(int n) { frames = n; };
		void SetActive(bool onOff) { active = onOff; };
		
		int GetID() { return id; };
		int GetFrames() { return frames; };
		bool GetActive() { return active; };
		
	private:
		int id;      // Unique (arbitrary) identification number:
		             // insures the same trigger is held between
		             // successive game loops, in cases where other
		             // factors which could be used as identification
		             // are non-constant (e.g. the speech text may
		             // be random)
		             //
		             // -1 signifies that this trigger is free
		             // to be overwritten
		         
		int frames;  // number of consecutive frames for which
		             // this trigger has been held.
		             //
		             // -1 signifies that this trigger is
		             // permanently disabled (for the remainder
		             // of the level)
		
		bool active; // Whether or not this trigger was held
		             // this frame.
};

/** Globals ****/
bubble* curBubble; // Pointer to the current bubble

const uint MAX_TRIGGERS = 3;
trigger *triggers = new trigger[MAX_TRIGGERS];
