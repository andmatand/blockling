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
void Speak(int block, char *text, bool important, bool polite);
void Speak(int block, const char *text, bool important, bool polite);
void Speak(int block, const char *text);
void Speak(int block, char *text);
void SpeechTrigger(int block, char *text, int targetFrames, char type, bool important, int id);
void SpeechTrigger(int block, const char *text, int targetFrames, char type, bool important, int id);


/** Classes ****/
// === speech "bubble" ===
class bubble {
	public:
		// Constructor
		bubble():
			text(NULL),
			ttl(0)
			{}
		
		bubble& operator = (const bubble& other) {
			SetText(other.GetText());
			//other.DelText();
			
			block = other.GetBlock();
			x = other.GetX();
			y = other.GetY();
			ttl = other.GetTTL();
			
			return *this;
		}

		
		// Destructor
		~bubble() {
			DelText();
		}
		
		void SetText(char *txt);
		void SetBlock(int aBlock) { block = aBlock; };
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		void SetTTL(uint frames) { ttl = frames; };
		void SetImportant(bool thisIsSerious) { important = thisIsSerious; };
		void SetPolite(bool itCanWait) { polite = itCanWait; };
	
		char* GetText() const { return text; };
		int GetBlock() const { return block; };
		int GetX() const { return x; };
		int GetY() const { return y; };
		uint GetTTL() const { return ttl; };
		bool GetImportant() const { return important; };
		bool GetPolite() const { return polite; };

		void DelText() { delete [] text; text = NULL; };
	
	private:

		char *text;     // Pointer to the text string
		int block;      // The block/player number of the block/player speaking
		int x;          // Text is centered around this x
		int y;          // Top of text
		uint ttl;       // Time to live (in frames)
		bool important; // If false, this speech bubble can be cut short by another one's creation
		bool polite;    // If true, this speech bubble will go to the end of the queue and wait its turn
};



		

void bubble::SetText(char *txt) {
	if (txt == NULL) return;
	
	DelText();
	
	text = new char[strlen(txt) + 1];
	strcpy(text, txt);
}





// === speech "trigger" ===
// If a trigger is held (e.g. the player is standing in a certain spot)
// for the target number of *consecutive* frames, then a speech bubble is generated
class trigger {
	public:
		// Constructor
		trigger():
			id(-1),
			frames(0)
			{}
		
		// Destructor
		//~trigger() {}
		
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
const uint MAX_BUBBLES = 3; // Maximum number of queued speech "bubbles"
bubble *bubbles = new bubble[MAX_BUBBLES];

const uint MAX_TRIGGERS = 3;
trigger *triggers = new trigger[MAX_TRIGGERS];
