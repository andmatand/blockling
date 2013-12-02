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


void ClearBubbles() {
	bubble* temp;
	while (curBubble != NULL) {
		temp = curBubble->GetNext();
		delete curBubble;
		curBubble = temp;
	}
}

void ClearSpeechTriggers() {
	// Get rid of all inactive triggers and then reset active bits for the
	// next check
	for (uint i = 0; i < MAX_TRIGGERS; i++) {
		// If this trigger was not pushed last frame, and it is
		// not permanently disabled
		if (triggers[i].GetActive() == false &&
		    triggers[i].GetFrames() != -1)
		{
			triggers[i].SetID(-1);
		}
		
		// Reset the active bit for the next check
		triggers[i].SetActive(false);
	}
}


// Overloaded for both <const char*> and <char*>
void HelpSpeak(int block,const char* text, bool polite, char postDir)
{
	// Copy the <const char*> to a <char*>
	char temp[strlen(text) + 1];
	strcpy(temp, text);

	HelpSpeak(block, temp, polite, postDir);
}
void HelpSpeak(int block, char* text, bool polite, char postDir) {
	// Enforce the help-speech option
	if (option_helpSpeech == false) return;

	Speak(block, text, polite, postDir);
}

// Overloaded for both <const char*> and <char*>
void Speak(int block, const char* text, bool polite, char postDir) {
	// Copy the <const char*> to a <char*>
	char temp[strlen(text) + 1];
	strcpy(temp, text);
	
	Speak(block, temp, polite, postDir);
}
void Speak(int block, char* text, bool polite, char postDir) {
	bubble* lastBubble = NULL;

	// If the new bubble is not polite
	if (polite == false) {
		// Get rid of all existing bubbles
		ClearBubbles();
	} else if (curBubble != NULL) {
 		// Find the last bubble in the linked list
		lastBubble = curBubble;
		while (lastBubble->GetNext() != NULL) {
			lastBubble = lastBubble->GetNext();
		}
	}

	// Make the new bubble
	bubble* bp = new bubble();

	// Set the block/player number
	bp->SetBlock(block);
	
	// Position the bubble above the block/player (in actual screen
	// coordinates)
	bp->SetX(blocks[block].GetX() + (blocks[block].GetW() / 2) -
			cameraX);
	bp->SetY(blocks[block].GetY() - cameraY);
	
	/** Set the TTL ****/
	// If the text is blank, pause a little
	if (text == NULL || text[0] == '\0') {
		bp->SetTTL(30);
	}
	else {  // otherwise set it based on the length of the text
		bp->SetTTL(static_cast<uint>(strlen(text)) * 2);
		
		// enforce a minimum TTL
		if (bp->GetTTL() < static_cast<int>(FPS)) {
			bp->SetTTL(FPS);
		}
	}
	
	bp->SetText(text);
	bp->SetPostDir(postDir);

	// Attach the new bubble to the end of the list
	if (lastBubble != NULL) {
		lastBubble->SetNext(bp);
	} else {
		// Set current bubble pointer to the new bubble
		curBubble = bp;
	}
}


void DrawBubbles(bool decrementTTLs) {
	if (curBubble == NULL) return;

	int x, y;
	
	y = blocks[curBubble->GetBlock()].GetY() - TILE_H - cameraY;
	
	// If it's a player
	if (curBubble->GetBlock() < static_cast<int>(numPlayers)) {
		// It he's carrying a block
		if (BlockNumber(
			blocks[curBubble->GetBlock()].GetX(),
			blocks[curBubble->GetBlock()].GetY() - 1,
			blocks[curBubble->GetBlock()].GetW(),
			1) >= 0)
		{
			// Move the y up a little so it doesn't cover up his
			// block
			y -= TILE_H;
		}
	}

	x = blocks[curBubble->GetBlock()].GetX() - cameraX;	

	// Position the text
	curBubble->GetText()->SetX(x);
	curBubble->GetText()->SetY(y);
	curBubble->GetText()->Center();
	curBubble->GetText()->AlignY(1); // Align to bottom
	curBubble->GetText()->Wrap(FONT_W * 20);

	// Draw the text
	curBubble->GetText()->Render();

	// If we aren't to decrement the TTL, turn back here
	if (decrementTTLs == false)
		return;

	curBubble->SetTTL(curBubble->GetTTL() - 1);

	// If this speech bubble is done
	if (curBubble->GetTTL() == 0) {
		// Turn the player the direction specified by postDir
		if (curBubble->GetPostDir() > -1) {
			blocks[curBubble->GetBlock()].SetDir(
				curBubble->GetPostDir());
		}

		// Save the address of the next bubble
		bubble* tempNext = curBubble->GetNext();

		// Free the current bubble's memory
		delete curBubble;

		// Point the current bubble pointer to the next bubble
		curBubble = tempNext;
	}
}


// Makes the speaking player's mouths move
void AnimateSpeech() {
	if (curBubble == NULL) return;

	int b; // will hold block number
	static char f; // holds current "face" number
	
	if (curBubble->GetText()->IsEmpty() == false) {
		b = curBubble->GetBlock();
		
		// Every 2 frames, move the player's mouth
		if (curBubble->GetTTL() % 4 == 0) {
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
		}
		
		// If the player has "scared" face (e.g. he's on a spike)
		// and the mouth is going to be set to "closed", then
		// prevent the "closed" mouth & replace it with "scared"
		//if (blocks[b].GetFace() == 4 && f == 0) {
		//	f = 4;
		//}

		// Close the mouth when the bubble is about to disappear
		if (curBubble->GetTTL() == 1)
			f = 0;

		blocks[b].SetFace(f);
	}
}



// Overloaded for both <const char *> and <char *>
void SpeechTrigger(int block, const char *text, int targetFrames, char type,
	int id)
{
	char temp[strlen(text) + 1];
	strcpy(temp, text);
	
	SpeechTrigger(block, temp, targetFrames, type, id);
}

void SpeechTrigger(int block, char *text, int targetFrames, char type, int id) {
	if (option_helpSpeech == false) return;

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
	
	// Mark the trigger as active this frame
	triggers[t].SetActive(true);
	
	// Increment the trigger's frames
	if (triggers[t].GetFrames() <= targetFrames) {
		triggers[t].SetFrames(triggers[t].GetFrames() + 1);
	}

	// Check if this trigger has been held long enough
	if (triggers[t].GetFrames() == targetFrames) {
		Speak(block, text, 0);
		
		// if this is a one-time-only trigger
		if (type == 1) {
			// mark it as permanently disabled
			triggers[t].SetFrames(-1);
		}
	}
}
