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



//#define DEBUG_REPLAY
/******** replayStep *********/
// A replayStep is one or more repititions of the same "action" (i.e.
// keypress).  Physically, it takes up one line when written to a replay
// file.
class replayStep {
	public:
		/*** Read ***/
		char GetKey() const { return key; };
		uint GetNum() const { return num; };
		char GetSymbol();
		
		/*** Write ***/
		void SetKey(char aKey) { key = aKey; };
		void SetNum(uint n) { num = n; };
	private:
		char key;	// Which key it is.
				// The numbers are the same as in the
				// playerKeys keyBinding, with a couple
				// of additions.
				// -1	Sleep (No keys pressed)
				//  0	Move left
				//  1	Move right
				//  2	Pick up block
				//  3	Set down block
				//  4	Push block
				//  5	Undo
		
		uint num;	// How many times it is successively pushed.
};



// Get the correct letter (to print) based on the key number
char replayStep::GetSymbol() {
	switch (static_cast<int>(key)) {
		case -1: // Sleep
			return 's';
			break;
		case 0: // Left
			return 'l';
			break;
		case 1: // Right
			return 'r';
			break;
		case 2: // Up
			return 'u';
			break;
		case 3: // Down
			return 'd';
			break;
		case 4: // Push
			return 'p';
			break;
		case 5: // Undo
			return 'n';
			break;
	}
	
	return 0;
}







/********** replay ************/
class replay {
	public:
		// Constructor prototype
		replay(char *file, uint buffSize);

		// Destructor prototype
		~replay();

		
		/*** Write ***/
		void SaveKey(char key);
		void DumpBuffer(); // Writes the buffer to the temp file
		
		/*** Read ***/
		void FillBuffer(); // Parses the replay file and loads a chunk of data into the replayStep array
		//char * GetFilename() const { return filename; };
		char GetNextKey();
		void PushKey(int k);
		void PushNextKey();
		void DecrementKey();
		
		/*** Other ***/
		void InitRead();
		void DeInitRead();
		void InitWrite();
		void DeInitWrite();
		
	private:
		char *filename;		// Full path to read/write the replay file from/to.
		
		FILE *fp;		// Pointer to file stream
		
		uint bufferSize;	// Number of replaySteps that will be
					// simultaneously held in the buffer.
		
		replayStep *steps;	// Will point to an array of replaySteps
		
		uint pos;		// The current step (whether recording or playing)
		
		uint *timestamps;	// Will point to an array of timestamps (timestamps come after sleeps)
		uint lastTimestamp;
};

// Constructor
replay::replay(char *file, uint buffSize):
pos(0) {
	bufferSize = buffSize;
	steps = new replayStep[bufferSize];
	timestamps = new uint[bufferSize];
	
	if (file != NULL) {
		filename = new char[strlen(file) + 1];
		strcpy(filename, file);
	}
}

// Destructor
replay::~replay() {
	delete [] filename; filename = NULL;
	delete [] steps; steps = NULL;
	delete [] timestamps; timestamps = NULL;
}



void replay::InitRead() {
	pos = 0;
	fp = fopen(filename, "r");
	
	if (fp == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for reading.", filename);
	}

	FillBuffer();
}




void replay::DeInitRead() {
	if (fclose(fp) != 0) {
		fprintf(stderr, "File error: Could not close replay file \"%s\"", filename);
	}
	
	fp = NULL;
}




void replay::InitWrite() {
	pos = 0;
	lastTimestamp = 0;
	fp = fopen(filename, "w");
	
	if (fp == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for writing.", filename);
	}
}




void replay::DeInitWrite() {
	DumpBuffer(); // Make sure to finish writing anything still in the buffer
	timestamps[pos] = levelTime;
	
	if (fclose(fp) != 0) {
		fprintf(stderr, "File error: Could not close replay file \"%s\"", filename);
	}
	
	//delete fp;
	fp = NULL;
}




void replay::DumpBuffer() {
	for (uint i = 0; (i < pos) && (i < bufferSize); i++) {
		// If the button was pushed more than once, print the number of times first
		if (steps[i].GetNum() > 1) fprintf(fp, "%d", steps[i].GetNum());
		
		// Print the symbol which identifies the action type
		fprintf(fp, "%c\n", steps[i].GetSymbol());
		
		// Print the position of the level timer after each step, so that
		// if the replay is played back with skipSleep, the timer can
		// still be kept accurate.
		if (timestamps[i] > lastTimestamp) {
			lastTimestamp = timestamps[i];
			fprintf(fp, "%d\n", timestamps[i]);
		}
	}
	
	// Reset position
	pos = 0;
}




void replay::FillBuffer() {
	char line[12];
	uint n;
	char k;
	char tempString[11];
	uint i;
	
	for (i = 0; i < bufferSize; i++) {
		#ifdef DEBUG_REPLAY
		printf("[FillBuffer] Filling step %d\n", i);
		#endif
		
		// Break when the end of the file is reached
		if (feof(fp)) break;
		
		// Read a line.  If it wasn't the end of the file, parse it
		if (fgets(line, sizeof(line), fp) != NULL) {
			#ifdef DEBUG_REPLAY
			printf("[FillBuffer] Parsing line...\n");
			#endif
			
			// Empty tempString
			tempString[0] = '\0';
			
			// Look at each character
			for (uint j = 0; j < strlen(line); j++) {
				#ifdef DEBUG_REPLAY
				printf("[FillBuffer] Examining character %d: \"%c\"\n", j, line[j]);
				#endif
				
				// If this character is a lower-case letter
				if (line[j] >= 97 && line[j] <= 122) {
					// Get the number from the first part of the line
					if (strlen(tempString) > 0) {
						n = static_cast<uint>(strtoul(tempString, NULL, 0));
						if (n <= 0) {
							printf("LINE READ AS <= ZERO!!\nvale = %d\n", n);
						}
					}
					else {
						n = 1;
					}
					// Set the number of presses
					steps[i].SetNum(n); 
					
					
					// Determine the key number from the action symbol
					k = -1;
					switch (line[j]) {
						case 's': // Sleep
							k = -1;
							break;
						case 'l': // Left
							k = 0;
							break;
						case 'r': // Right
							k = 1;
							break;
						case 'u': // Up
							k = 2;
							break;
						case 'd': // Down
							k = 3;
							break;
						case 'p': // Push
							k = 4;
							break;
						case 'n': // Undo
							k = 5;
							break;
					}
					// Set the key
					steps[i].SetKey(k);
					
					// Clear the timestamp
					timestamps[i] = 0;
					
					// Stop examining this line
					break;
				}
				// If the line contains nothing but a number, it is a level timer position
				else if (i > 0 && j + 1 == strlen(line)) {
					n = static_cast<uint>(strtoul(tempString, NULL, 0));
					timestamps[i - 1] = n;
					
					// Don't count this as a replayStep
					i--;
					
					// Stop examining this line
					break;
				}
				else {
					// Append this character to the temporary string
					tempString[j] = line[j];
					tempString[j + 1] = 0;
				}
			}
		}
	}
	
	// If the whole buffer was not filled (i.e. EOF was reached)
	if (i < bufferSize) {
		// Raise a flag for PushNextKey to know when to stop
		steps[i - 1].SetKey(100);
		timestamps[i - 1] = 0;
	}
	
	// Reset position
	pos = 0;
}




void replay::SaveKey(char key) {
	/*** Check if the number of presses is at the variable's maximum value ***/
	bool maxNum = false;
	
	// Check if the number of presses has exceeded the variable's max value
	if (pos > 0) {
		// Add one to the num
		steps[pos - 1].SetNum(steps[pos - 1].GetNum() + 1);
		
		// If it's zero now, it overflowed
		if (steps[pos - 1].GetNum() == 0) {
		
			// Set flag to enter next if statment
			maxNum = true;
		}
		// Reset it to it's previous value
		steps[pos - 1].SetNum(steps[pos - 1].GetNum() - 1);
	}


	// If this is the very first key, OR if this is not the same key
	// as the previous key, OR if the previous key has been pushed the maximum number of times
	if (pos == 0 || key != steps[pos - 1].GetKey() || maxNum) {
		#ifdef DEBUG_REPLAY
			printf("[replay] pos = %d\n", pos);
			printf("[replay] New key (%d) was pressed.\n", key);
		#endif
		
		// Update the current level timer position
		if (pos > 0) {
			timestamps[pos - 1] = levelTime;
		}
		

		// If the buffer is filled
		if (pos == bufferSize) {
			#ifdef DEBUG_REPLAY
			printf("[replay] Buffer is full, dumping to file...\n");
			#endif
			
			// Dump the buffer to the replay file
			DumpBuffer();				
		}

		// Save the current key
		steps[pos].SetKey(key);
		steps[pos].SetNum(1);
		
		// Save the current level timer position for the current key (will proabably be updated later)
		timestamps[pos] = levelTime;
		
		// Make pos contain the number of the next step
		pos++;
	}
	// If this is the same key as the previous one
	else {
		//if (key != -1) printf("[replay] Key was repeated (total: %d times)\n", steps[pos - 1].GetNum());
		
		// Increment the number of presses of the last key
		steps[pos - 1].SetNum(steps[pos - 1].GetNum() + 1);
		
		// Save the current level timer position
		timestamps[pos - 1] = levelTime;
	}
}




char replay::GetNextKey() {
	// Check for the flag set by FillBuffer to signal EOF
	if (steps[pos].GetKey() == 100) {
		return 100;
	}
	
	// If we've finished pushing the key the required number of times
	if (steps[pos].GetNum() == 0) {
		// Adjust the timer according to the timestamp
		if (timestamps[pos] != 0) {
			#ifdef DEBUG_REPLAY
				printf("Setting levelTime to %d\n", timestamps[pos]);
			#endif
			
			levelTime = timestamps[pos];
			levelTimeTick = SDL_GetTicks();
		}

		// If this is the last step in the buffer
		if (pos == bufferSize - 1) {
			// Read more steps from the file
			FillBuffer();
		}
		else {
			// Otherwise, Go to the next step
			pos++;
		}

		// Lengthen sleeps if replay is in slow motion
		if (option_replaySpeed == 0 && steps[pos].GetKey() == -1) {
			printf("STEP %d: Lengethed sleep length from %d to ", pos, steps[pos].GetNum());
			
			steps[pos].SetNum(steps[pos].GetNum() * (TILE_W / 2));
			
			printf("%d\n", steps[pos].GetNum());
		}
	}
	
	// Check for the flag set by FillBuffer to signal EOF
	if (steps[pos].GetKey() == 100) {
		return 100;
	}

	return steps[pos].GetKey();
}



void replay::PushKey(int k) {

	// Undo
	if (k == 5) {
		gameKeys[4].on = 1;
	}
	// Regular Key
	else if (k >= 0 && k <= 4) {
		playerKeys[k].on = 1;
	}
}



// Press the next key in the replay file
void replay::PushNextKey() {
	// Check for the flag set by FillBuffer to signal EOF
	if (steps[pos].GetKey() == 100) {
		return;
	}

	int k = GetNextKey();
	
	/*** Turn the correct key on ***/
	PushKey(k);
	
	// Decrement the number of times we must push the key
	steps[pos].SetNum(steps[pos].GetNum() - 1);	
}

// Press the next key in the replay file
void replay::DecrementKey() {
	// Check for the flag set by FillBuffer to signal EOF
	if (steps[pos].GetKey() == 100) {
		return;
	}

	// Decrement the number of times we must push the key
	steps[pos].SetNum(steps[pos].GetNum() - 1);
}
