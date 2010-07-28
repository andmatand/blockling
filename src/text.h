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


// A txtLine is a line of text used by the txt Class.  Each line has a
// separate x position
class txtLine {
	public:
		// Constructor/Destructor prototypes
		txtLine(int xPos, char* string);
		txtLine();
		~txtLine();

		// Accessor Methods
		int GetX() const { return x; };
		char* GetStr() const { return str; };
		txtLine* GetNext() const { return next; };

		void SetX(int newX) { x = newX; };
		void SetStr(char* newStr);
		void SetNext(txtLine* lp) { next = lp; };

		void AppendStr(char* mas);
		void Truncate(int newLength);

	private:
		void DelStr();

		int x; // The screen x coordinate of this line
		char* str; // The text string
		txtLine* next; // Pointer to next txtLine in the linked-list
};

// Constructor
txtLine::txtLine(int xPos, char* string) {
	x = xPos;
	str = NULL;
	SetStr(string);
	next = NULL;
}
txtLine::txtLine() {
	x = 0;
	str = NULL;
	next = NULL;
}

// Destructor
txtLine::~txtLine() {
	DelStr();
}

void txtLine::DelStr() {
	delete [] str;
	str = NULL;
}

void txtLine::SetStr(char* newStr) {
	DelStr();

	str = new char[strlen(newStr) + 1];
	strcpy(str, newStr);
}

void txtLine::AppendStr(char* mas) {
	// Save a copy of the old string
	char oldStr[strlen(str) + 1];
	strcpy(oldStr, str);

	// Delete the old string
	DelStr();

	// Make a new string of the correct length
	str = new char[strlen(oldStr) + strlen(mas) + 1];

	// Fill it with both strings
	sprintf(str, "%s%s", oldStr, mas);
}

void txtLine::Truncate(int newLength) {
	// Load a temp string with the desired new string
	char newStr[newLength + 1];
	strncpy(newStr, str, newLength);
	newStr[newLength] = '\0';

	// Change str to newStr
	SetStr(newStr);
}



// txt Class
// For text displayed onscreen which needs to be arranged specially, e.g.
// word-wrap, centering, etc.
class txt {
	public:
		// Constructor prototypes
		txt();
		txt(int xPos, int yPos, const char* str);

		// Destructor prototype
		~txt();

		// Accessor Methods
		void SetX(int xPos) {
			x = xPos;

			// Rewrap the text it necessary
			/*
			if (fitToScreen) {
				Wrap();
			}
			*/
		}
		void SetY(int yPos) {
			y = yPos;

			// Rewrap the text it necessary
			/*
			if (fitToScreen) {
				Wrap();
			}
			*/
		};
		void SetText(const char* newStr);
		void SetText(char* newStr);
		bool IsEmpty() const {
			if (lineList == NULL ||
				strlen(lineList->GetStr()) == 0)
			{
				return true;
			}
			else {
				return false;
			}
		}

		// Add/Remove Lines
		void DelText(); // Deletes existing lines of text
		void AddLine(char *str);

		// Methods
		void Wrap(int wrapWidth = SCREEN_W);
		void WrapLine(txtLine* line);
		void Center();
		void AlignY(int ay) { yAlign = ay; };
		void Render();

	private:
		int x, y;
		int wrapW; // If the text is wrapped, it will not exceed this
		           // width

		bool fitToScreen; // If true, the text will adjust itself to
		                  // remain within the boundaries of the
		                  // screen

		int xAlign; // -1 Left align (left edge is at x)
		            //  0 Center (center is at x)
		            //  1 Right align (right edge is at x)

		int yAlign; // -1 Top is at y
		            //  0 Center is at y
		            //  1 Bottom is at y

		int spacing; // Extra spacing between letters
		int color;
		txtLine* lineList; // Pointer to linked-list of textLines
};

// Constructor
txt::txt() {
	x = 0;
	y = 0;
	wrapW = 0;
	fitToScreen = true;
	xAlign = -1;
	yAlign = -1;
	spacing = 0;
	color = 1;
	lineList = NULL;
}
txt::txt(int xPos, int yPos, const char* str) {
	// Set defaults
	wrapW = 0;
	fitToScreen = true;
	xAlign = -1;
	yAlign = -1;
	spacing = 0;
	color = 1;
	lineList = NULL;

	// Set info from arguments
	x = xPos;
	y = yPos;
	SetText(str);
}

// Destructor
txt::~txt() {
	DelText();
}

void txt::DelText() {
	txtLine* temp;

	// Delete each line
	while (lineList != NULL) {
		temp = lineList->GetNext();
		delete lineList;
		lineList = temp;
	}
}

// Overloaded to allow <const char*> and <char*>
void txt::SetText(const char* str) {
	// Copy the <const char*> to a <char *>
	char tempStr[strlen(str) + 1];
	strcpy(tempStr, str);

	// Pass the string to the real SetStr()
	SetText(tempStr);
}
void txt::SetText(char* str) {
	// Delete existing text
	DelText();

	// Add the text as the first line
	AddLine(str);

	// Recalculate the positions/lengths of lines
	//this->Wrap();
}

void txt::AddLine(char* str) {
	txtLine* lastLine = lineList;

	// If there is an existing list
	if (lineList != NULL) {
		// Find the last txtLine in the linked list
		while (lastLine->GetNext() != NULL) {
			lastLine = lastLine->GetNext();
		}
	}

	// Make a new txtLine
	txtLine* lp = new txtLine(0, str);

	// If there is an existing list
	if (lineList != NULL) {
		// Make the new line the "next" line of the current last line
		lastLine->SetNext(lp);
	}
	else {
		lineList = lp;
	}
}


void txt::Wrap(int wrapWidth) {
	if (lineList == NULL) return;

	// Set the wrapW from the argument
	wrapW = wrapWidth;

	// Reassemble all the lines into one long first line
	txtLine* curLine = lineList->GetNext();
	while (curLine != NULL) {
		// Append this line's string to the first line
		lineList->AppendStr(curLine->GetStr());

		// Point the first line's "next" to this line's "next"
		lineList->SetNext(curLine->GetNext());

		// Delete the current line
		delete curLine;

		curLine = lineList->GetNext();
	}

	// Loop through each line in the list
	int numLines = 0;
	curLine = lineList;
	do {
		numLines ++;
		WrapLine(curLine);
		curLine = curLine->GetNext();
	} while (curLine != NULL);


	int margin_top = 0;
	int margin_bottom = FONT_H + 2;


	// Align top of text to y
	if (yAlign == 0) {
		if (fitToScreen) {
			// Constrain top
			if (y < 0) {
				y = 0 + margin_top;
			}
		}
	}
	// Align bottom of text to y
	else if (yAlign == 1) {
		if (fitToScreen) {
			// Constrain bottom
			if (y > SCREEN_H - 1 - margin_bottom) {
				y = SCREEN_H - 1 - margin_bottom;
			}
		}

		// Find the height of the whole text
		int h = (TILE_H * numLines) + (2 * (numLines - 1));

		// Find ideal y position
		y = y - h;

		// Constrain to screen if necessary
		if (fitToScreen) {
			if (y < margin_top) {
				y = margin_top;
			}
		}
	}
}

void txt::WrapLine(txtLine* line) {
	if (strlen(line->GetStr()) < 1) return;

	int x2 = x; // Working x coordinate
	char c; // For holding the current character
	bool ok;
	int lastSpace = 0; // The position of the previous space

	int margin_right = FONT_W / 2;
	int margin_left = margin_right;

	// Constrain x2 and y2 to screen boundaries
	if (fitToScreen) {
		if (x2 < margin_left) {
			x2 = 0 + margin_left;
		} else if (x2 > SCREEN_W - 1 - margin_right) {
			x2 = SCREEN_W - 1 - margin_right;
		}
	}

	line->SetX(x2);


	// Loop through each character in the string
	for (uint i = 0; i < strlen(line->GetStr()); i++) {
		c = line->GetStr()[i];
		
		// If this is a space, a linefeed, or the end of the string
		if (c == ' ' || c == '\n' || i == strlen(line->GetStr()) - 1) {
			ok = true; // assume the line will fit
			char* temp;

			// If it's a linefeed or the end of the string
			if (c == '\n' || i == strlen(line->GetStr()) - 1) {
				// Fill temp up to and including the current
				// character
				temp = new char[i + 2];
				strncpy(temp, line->GetStr(), i + 1);
				temp[i] = '\0';
			} else {
 				// Fill temp with the current line up to (but
				// not including) this character
				temp = new char[i + 1];
				strncpy(temp, line->GetStr(), i);
				temp[i] = '\0';
			}

			// Left-aligned
			if (xAlign == -1) {
				// Store the width
				int tempW = GetTextW(temp, spacing);

				// If temp is too long
				if (x2 + tempW > x2 + (wrapW - 1) ||
					(fitToScreen && x2 + tempW >
					SCREEN_W - 1 - margin_right))
				{
					ok = false;  // the line will not fit
				}
			} else if (xAlign == 0) { // Centered
				// Store temp's half-width
				int halfW = GetTextW(temp, spacing) / 2;

				// If temp is too long
				if ((x2 + halfW) - (x2 - halfW) + 1 > wrapW) {
					ok = false;  // the line will not fit
				}
			}
				

			// If the line does not fit up to this point
			if (ok == false) {
				// Break the line at the previous space, and
				// copy what's after that to a new line
				AddLine(line->GetStr() + lastSpace + 1);

				// Truncate this line
				line->Truncate(lastSpace + 1);

				// End the for-loop
				break;
			}
			else {
				// Save the position of this space
				lastSpace = i;
			}
		}

		// If a linefeed is encountered
		if (c == '\n') {
			// If this whole line isn't just a linefeed
			if (strlen(line->GetStr()) > 1) {
				// Put the rest on another new line
				AddLine(line->GetStr() + i + 1);

				// Truncate this line
				line->Truncate(i + 1);
			}

			// End the for-loop
			break;
		}
	}

	// If the text is centered
	if (xAlign == 0) {
		// Find the starting position of any trailing spaces
		uint spaceStart = strlen(line->GetStr());
		for (uint j = strlen(line->GetStr()) - 1; j > 0; j--) {
			if (line->GetStr()[j] == ' ') {
				spaceStart = j;
			}
			else {
				break;
			}
		}

		// Store a version of the string without trailing spaces
		char* temp;
		if (spaceStart < strlen(line->GetStr()) && spaceStart > 0) {
			temp = new char[spaceStart + 1];
			strncpy(temp, line->GetStr(), spaceStart);
			temp[spaceStart] = '\0';
		}
		else {
			temp = line->GetStr();
		}

		// Store the width of temp
		int w = GetTextW(temp);

		// Find the ideal x position
		x2 = x - (w / 2);

		// Adjust to boundaries
		if (fitToScreen) {
			if (x2 + w > SCREEN_W - 1 - margin_right) {
				x2 = SCREEN_W - 1 - margin_right - w;
			}
			if (x2 < margin_left) {
				x2 = margin_left;
			}
		}

		// Set it
		line->SetX(x2);
	}
}


void txt::Center() {
	xAlign = 0;
}


void txt::Render() {
	int y2 = y; // Working y coordinate

	// Loop through each line in the list
	txtLine* l = lineList;
	do {
		// Draw this line
		DrawText(l->GetX(), y2, l->GetStr());
		
		// Move the y2 coordinate down
		y2 += FONT_H + 2;

		l = l->GetNext();
	} while (l != NULL);
}
