/*
 *   replay.h
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


/********replayStep*********/
// A replayStep is one or more repititions of the same "action" (i.e. keypress)
// Physically, it takes up one line when written to a replay file
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






/**********replay************/
class replay {
	public:
		/*** Constructor ***/
		replay(char *file, uint buffSize);
		replay(); // Alternate contructor (for Load Replay menu); used only to hold meta-data
		
		/*** Write ***/
		void SaveKey(char key);
		void SetFilename(char * fn);
		void DumpBuffer(); // Writes the buffer to the temp file
		void SaveToFile(char *saveFilename, uint theLevel, char *theTitle); // Saves the final replay to a file, with the user-specified metadata
		
		/*** Read ***/
		void FillBuffer(); // Parses the replay file and loads a chunk of data into the replayStep array
		uint GetLevel() const { return level; };
		uint GetMoves() const { return moves; };
		char * GetTitle() const { return title; };
		char * GetFilename() const { return filename; };
		bool PushNextKey();
		
		/*** Other ***/
		void InitRead();
		void DeInitRead();
		void InitWrite();
		void DeInitWrite();
		
	private:
		char *filename;		// Full path to read/write the replay file from/to.
		
		FILE * fp;		// Pointer to file stream
		
		uint bufferSize;	// Number of replaySteps that will be
					// simultaneously held in the buffer.
		
		replayStep *steps;	// Will point to an array of replaySteps
		
		uint pos;		// The current step (whether recording or playing)
		
		/*** Meta-Data ***/
		char levelSet[256];	// Which levelSet the level is in
		uint level;		// Which level this replay is for
		uint moves;		// How many moves were made (not couting sleeps)
		char *title;		// User-created tile of this replay
		char date[17];		// Date/time the replay was recorded (e.g. "1973-02-31 19:57")
};

// Constructor
replay::replay(char *file, uint buffSize):
pos(0), moves(0) {
	bufferSize = buffSize;
	steps = new replayStep[bufferSize];
	
	if (file != NULL) {
		filename = new char[strlen(file) + 1];
		strcpy(filename, file);
	}

	title = new char[MAX_REPLAY_TITLE_LENGTH + 1];
	
	time_t theTime;
  	time( &theTime );   // get the calendar time

	strftime(date, sizeof(date), "%Y-%m-%d %H:%M", localtime(&theTime));
}

// Alternate Constructor (used by Load Replay Menu)
replay::replay():
pos(0) {
	// We don't need any replaySteps, since the replay will hold only meta data
	bufferSize = 0;
	
	title = new char[MAX_REPLAY_TITLE_LENGTH + 1];
	
	filename = NULL;
}




void replay::InitRead() {
	pos = 0;
	fp = fopen(filename, "r");
	
	if (fp == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for reading.", filename);
	}

	// Read the meta-data information at the top of the file
	char line[100];
	uint n = 0; // Line number
	while ( !feof(fp) && n < 4 ) {
		// Read a line.  If it wasn't the end of the file, store it to the correct variable
		if (fgets(line, sizeof(line), fp) != NULL) {
			printf("Reading line %d of replay header...\n", n + 1);
			printf("Line contents: \"%s\"\n", line);

			// Remove \n at end of line
			if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

			// Store the line in the correct variable, based on line number
			switch(n) {
				case 0:
					strncpy(title, line, sizeof(title));
					break;
				case 1:
					level = atoi(line);
					break;
				case 2:
					moves = atoi(line);
					break;
				case 3:
					strncpy(date, line, sizeof(date));
					break;
			}
				
			n++;
		}
	}
	
	printf("title = \"%s\"\nlevel = %d\ndate = \"%s\"\n", title, level, date);
	
	FillBuffer();
}

void replay::DeInitRead() {
	if (fclose(fp) != 0) {
		fprintf(stderr, "File error: Could not close replay file \"%s\"", filename);
	}
	
	//delete fp;
	fp = NULL;
}


void replay::InitWrite() {
	pos = 0;
	fp = fopen(filename, "w");
	
	if (fp == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for writing.", filename);
	}
	
}

void replay::DeInitWrite() {
	DumpBuffer(); // Make sure to finish writing anything still in the buffer
	
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
	}
	
	// Reset position
	pos = 0;
}



void replay::FillBuffer() {
	char line[12];
	char n;
	char tempString[11];
	uint i;
	
	printf("[FillBuffer]==Filling buffer==\n");
	
	for (i = 0; i < bufferSize; i++) {
		printf("[FillBuffer] Filling step %d\n", i);
		
		// Break when the end of the file is reached
		if (feof(fp)) break;
		
		// Read a line.  If it wasn't the end of the file, parse it
		if (fgets(line, sizeof(line), fp) != NULL) {
			printf("[FillBuffer] Parsing line...\n");
			// Empty tempString
			tempString[0] = '\0';
			
			// Look at each character
			for (uint j = 0; j < strlen(line); j++) {
				printf("[FillBuffer] Examining character %d: \"%c\"\n", j, line[j]);
				// If this character is a lower-case letter
				if (line[j] >= 97 && line[j] <= 122) {
					// Get the number from the first part of the line
					if (strlen(tempString) > 0) {
						n = static_cast<char>(atoi(tempString));
					}
					else {
						n = 1;
					}
					// Set the number of presses
					steps[i].SetNum(n); 
					
					
					// Determine the key number from the action symbol
					n = -1; // Recycling!  It's good for the environment or something.
					switch (line[j]) {
						case 's': // Sleep
							n = -1;
							break;
						case 'l': // Left
							n = 0;
							break;
						case 'r': // Right
							n = 1;
							break;
						case 'u': // Up
							n = 2;
							break;
						case 'd': // Down
							n = 3;
							break;
						case 'p': // Push
							n = 4;
							break;
						case 'n': // Undo
							n = 5;
							break;
					}
					// Set the key
					steps[i].SetKey(n);
					
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
		steps[i].SetNum(0);
	}
	
	// Reset position
	pos = 0;
}


void replay::SaveKey(char key) {
	/*** Check if the number of presses is at the variable's maximum value ***/
	bool maxNum = false;
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
		printf("[replay] pos = %d\n", pos);
		printf("[replay] New key (%d) was pressed.\n", key);
		
		// If the buffer is filled
		if (pos == bufferSize) {
			printf("[replay] Buffer is full, dumping to file...\n");
			// Dump the buffer to the replay file
			DumpBuffer();				
		}

		// Save the current key
		steps[pos].SetKey(key);
		steps[pos].SetNum(1);
		
		// Increment moves if this is not a sleep
		if (key != -1) moves++;
		
		// Make pos contain the number of the next step
		pos++;
	}
	// If this is the same key as the previous one
	else {
		//if (key != -1) printf("[replay] Key was repeated (total: %d times)\n", steps[pos - 1].GetNum());
		
		// Increment the number of presses of the last key
		steps[pos - 1].SetNum(steps[pos - 1].GetNum() + 1);

		// Increment moves if this is not a sleep
		if (key != -1) moves++;
	}
}


void replay::SaveToFile(char *saveFilename, uint theLevel, char *theTitle) {
	// Close the file
	DeInitWrite();

	// Open the new file and write to it the meta data header, followed by the contents of the temp file
	FILE * readFile = fopen(filename, "r");
	FILE * writeFile = fopen(saveFilename, "w");
	
	if (readFile == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for reading.\n", filename);
	}
	else if (writeFile == NULL) {
		fprintf(stderr, "File error: Could not open file \"%s\" for writing.\n", saveFilename);
	}
	else {
		// Print the meta data
		fprintf(writeFile, "%s\n%d\n%d\n%s\n", theTitle, theLevel, moves, date);
		
		// Print the rest of the lines
		char line[12];
		while (!feof(readFile)) {
			if (fgets(line, sizeof(line), readFile) != NULL) {
				fprintf(writeFile, "%s", line);
			}
		}

		// Close both files
		fclose(readFile);
		fclose(writeFile);

		// Delete the temp replay file
		if (remove(filename) != 0) {
			fprintf(stderr, "File error: Could not delete temporary replay file \"%s\".\n", filename);
		}
	}
}


// Press the next key in the replay file
bool replay::PushNextKey() {
	// Check for the flag set by FillBuffer to signal EOF
	if (steps[pos].GetNum() == 0) {
		return false;
	}

	int k = steps[pos].GetKey();
	
	// Turn all playerKeys off
	for (uint i = 0; i < NUM_PLAYER_KEYS; i++) {
		playerKeys[i].on = 0;
	}
	
	// Turn undo key off
	gameKeys[1].on = 0;
	
	/*** Turn the correct key on ***/
	// Undo
	if (k == 5) {
		gameKeys[1].on = 1;
	}
	// Regular Key
	else if (k >= 0 && k <= 4) {
		playerKeys[k].on = 1;
	}
	
	// Decrement the number of times we must push the key
	steps[pos].SetNum(steps[pos].GetNum() - 1);
	
	// If we've finished pushing the key the required number of times
	if (steps[pos].GetNum() == 0) {
		// If this is the last step in the buffer
		if (pos == bufferSize - 1) {
			// Read more steps from the file
			FillBuffer();
		}
		else {
			// Otherwise, Go to the next step
			pos++;
		}
		
	}
	
	return true;
}



void replay::SetFilename(char * fn) {
	// Clean up old text data
	delete [] filename;
	filename = NULL;
	
	filename = new char[strlen(fn) + 1];
	strcpy(filename, fn);
}
