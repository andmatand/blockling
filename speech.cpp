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


void ClearSpeechTriggers() {
	/** Get rid of all inactive triggers and then reset active bits for the next check ****/
	for (uint i = 0; i < MAX_TRIGGERS; i++) {
		// If this trigger was not pushed last frame, and it is
		// not permanently disabled
		if (triggers[i].GetActive() == false && triggers[i].GetFrames() != -1) {
			triggers[i].SetID(-1);
		}
		
		// Reset the active bit for the next check
		triggers[i].SetActive(false);
	}
}




// Overloaded for both <const char *> and <char *>
void Speak(int block, const char *text) {
	char temp[strlen(text) + 1];
	strcpy(temp, text);
	
	Speak(block, temp);
}


// polite = true    Indicates that the message is unimportant and if
// there is already another active bubble, don't bother speaking
void Speak(int block, char *text) {
	/** Find the first empty spot in the bubbles array (queue) ****/
	for (uint i = 0; i < MAX_BUBBLES; i++) {
		if (bubbles[i].GetTTL() == 0) {
			// Set the block/player number
			bubbles[i].SetBlock(block);
			
			// Position the bubble above the block/player (in actual screen coordinates)
			bubbles[i].SetX(blocks[block].GetX() + (blocks[block].GetW() / 2) - cameraX);
			bubbles[i].SetY(blocks[block].GetY() - cameraY);
			
			/** Set the TTL ****/
			// If the text is blank, pause a little
			if (text == NULL || text[0] == '\0') {
				bubbles[i].SetTTL(30);
			}
			else {  // otherwise set it based on the length of the text
				bubbles[i].SetTTL(static_cast<uint>(strlen(text)) * 2);
			}
			
			
			// Set the text
			bubbles[i].SetText(text);
			
			// Stop looking for empty bubbles
			break;
		}
	}
}



void DrawBubbles(bool decrementTTLs) {
	int wrapW = FONT_W * 14;
	
	uint i = 0;
	if (bubbles[i].GetTTL() > 0) {
		// Draw the text
		DrawText(
			//bubbles[i].GetX(),
			blocks[bubbles[i].GetBlock()].GetX() - cameraX,
			//bubbles[i].GetY() - GetTextH(bubbles[i].GetText(), wrapW, 0) - FONT_H - 2,
			blocks[bubbles[i].GetBlock()].GetY() - cameraY - GetTextH(bubbles[i].GetText(), wrapW, 0) - (FONT_H * 2) - 2,
			bubbles[i].GetText(),
			true,
			wrapW,
			0,
			1);

		// Decrement the TTL
		if (decrementTTLs) {
			bubbles[i].SetTTL(bubbles[i].GetTTL() - 1);
		
			// If this speech bubble is done
			if (bubbles[i].GetTTL() == 0) {
				// rotate the queue
				for (uint i = 0; i < MAX_BUBBLES - 1; i++) {
					bubbles[i] = bubbles[i + 1];
				}
				bubbles[MAX_BUBBLES - 1].SetTTL(0);
			}
		}
	}
}



// Makes speaking players' mouths move
void AnimateSpeech() {
	int b; // will hold block number
	char f; // will hold "face" number
	
	uint i = 0;
	if (bubbles[i].GetTTL() > 0) {
		b = bubbles[i].GetBlock();
		
		// Every 2 frames, move the player's mouth
		if (bubbles[i].GetTTL() % 4 == 0) {
			switch (blocks[b].GetFace()) {
				case 0: // closed
					if (rand() % 4 == 0) { // less likely
						f = 4; // "scared" mouth, which looks like an "o"
					}
					else {
						f = 1; // open
					}
					
					break;
				case 1: // open
					if (rand() % 4 == 0) { // less likely
						f = 4; // "scared" mouth, which looks like an "o"
					}
					else {
						f = 0; // closed
					}
					
					break;
				case 4: // "scared"
					if (rand() % 2 == 0) {
						f = 0; // closed
					}
					else {
						f = 1; // open
					}
					
					break;
			}
			
			blocks[b].SetFace(f);
		}
		
		// Close the mouth when the bubble is about to disappear
		if (bubbles[i].GetTTL() == 1)
			blocks[b].SetFace(0);
	}
}



// Overloaded for both <const char *> and <char *>
void SpeechTrigger(int block, const char *text, int targetFrames, char type, int id) {
	char temp[strlen(text) + 1];
	strcpy(temp, text);
	
	SpeechTrigger(block, temp, targetFrames, type, id);
}



void SpeechTrigger(int block, char *text, int targetFrames, char type, int id) {
	int t = -1; // the trigger number in the "triggers" array
	
	// Check if this trigger already exists
	for (uint i = 0; i < MAX_TRIGGERS; i++) {
		if (triggers[i].GetID() == id) {
			// If this trigger has been permanently disabled
			if (triggers[i].GetFrames() == -1) {
				// exit the function
				return;
			}	
			else {
				t = i;
			}
			break;
		}
	}
	
	// If it was not found, create it in an empty slot
	if (t == -1) {
		for (uint i = 0; i < MAX_TRIGGERS; i++) {
			// If this slot is empty
			if (triggers[i].GetID() == -1) {
				t = i;
				
				triggers[i].SetID(id); 
				triggers[i].SetFrames(0);
				triggers[i].SetActive(true);
				break;
			}
		}
	}
	
	// If there was no room in the "triggers" array, exit the
	// function here
	if (t == -1) return;
	
	// Increment the trigger's frames
	if (triggers[t].GetFrames() <= targetFrames) {
		triggers[t].SetFrames(triggers[t].GetFrames() + 1);
		triggers[t].SetActive(true);
	}

	// Check if this trigger has been held long enough
	if (triggers[t].GetFrames() == targetFrames) {
		Speak(block, text);
		
		// if this is a one-time-only trigger
		if (type == 1) {
			// mark it as permanently disabled
			triggers[t].SetFrames(-1);
		}
	}
}
