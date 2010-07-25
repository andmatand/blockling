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


int GetTextW(char *text, int spacing) {
	if (text == NULL) return 0;
	
	//const int tW = FONT_W * 4; // Tab Width
	
	int w = 0;
	int c;
	
	for (uint i = 0; i < static_cast<unsigned int>(strlen(text)); i++) {
		c = text[i];
		
		// Line break
		if (c == '\n') {
			return w;
		}

		// Tab
		if (c == '\t') {
			// AAAHHH!  I don't know how to figure out the width
			// of text with a tab in it, unless we have the starting x,
			// so I'll just cheat by making the Control Setup menu
			// have a constant x position.
			
			//x2 = (tW * static_cast<int>(x2 / tW)) + tW;
			
			// Go to next character
			continue;
		}
		
		// Get index number (so that '!' starts at 0)
		c -= 33;

		switch (c) {
			case -1: // Space (32)
				w += FONT_W + spacing;
				break;
			default:
				w += font[c].w;
				
				// If this is not the last character, add a bit of space
				if ((i + 1) != static_cast<unsigned int>(strlen(text))) w += 2 + spacing;
				
				break;
		}
	}
	
	return w;
}



int GetTextH(char *text, int wrapWidth, int spacing) {
	if (text == NULL) return 0;
	
	int h = 1; // height (in number of lines of text)
	int lineW = 0; // width of the current line
	int wordW = 0; // width of the current word
	int c; // holds current character
	
	for (uint i = 0; i < static_cast<unsigned int>(strlen(text)); i++) {
		c = text[i];
		
		// Line break
		if (c == '\n') {
			// Increase the height by one line			
			h += 1;

			// Increase the height by the number of lines
			// this text will take up (rounding up)
			h += int((wrapWidth / lineW) + .5);
			
			lineW = 0;
			wordW = 0;
		}
		else if (c != '\t') {
			if (c == ' ') {
				// if the previous character was also a space
				if (i > 0 && text[i - 1] == ' ') {
					// count this space
					lineW += FONT_W + spacing;
				}
				
				lineW += wordW;
				wordW = 0;
			}
			else {
				wordW += font[c - 33].w;

				// If this is not the last character, add a bit
				// of space
				if ((i + 1) != static_cast<unsigned int>(
				               strlen(text)))
				{
					wordW += 2 + spacing;
				}
			}
		}
	}
	
	if (lineW > 0) {
		// Increase the height by the number of lines
		// this text will take up (rounding up)
		h += int((double(lineW) / double(wrapWidth)) + .5);
	}

	// convert h from number of lines to the actual height in pixels
	h = (h * FONT_H) + ((h - 1) * 2);
	
	return h;
}



char LoadFont(const char *file) {
	char fullPath[strlen(DATA_PATH) + strlen(FONT_PATH) + strlen(file) + 1];
	sprintf(fullPath, "%s%s%s", DATA_PATH, FONT_PATH, file);
	#ifdef DEBUG
		printf("Font path = %s\n", fullPath);
	#endif
	
	// The surface holding the big long list of characters
	SDL_Surface *fontSurf = FillSurface(fullPath, 0);
	if (fontSurf == NULL) {
		return 1;
	}
	
	// For holding the rgb values for pre-rendering differently-colored
	// letters
	SDL_Color palette[256];
	uint r, g, b;
	
	// For finding the left and right side of each character (to determine
	// width)
	int leftSide;
	int rightSide;
	
	uint i = 0;
	uint transColor = SDL_MapRGB(fontSurf->format, 0xff, 0x00, 0xff);
	int sourceOffset = 0;
	for (int y = 0; y < fontSurf->h; y += FONT_H) {
		// Lock the surface (for subsequent GetPixel calls)
		SDL_LockSurface(fontSurf);
				
		/*** Determine the letter's width ***/
		// Find left side
		leftSide = -1;
		for (int x2 = 0; x2 < FONT_W; x2++) {
			for (int y2 = y; y2 < y + FONT_H; y2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					leftSide = x2;
					break;
				}
			}
			if (leftSide != -1) break;
		}
		
		// Find right side
		rightSide = -1;
		for (int x2 = FONT_W - 1; x2 > 0; x2--) {
			for (int y2 = y; y2 < y + FONT_H; y2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					rightSide = x2;
					break;
				}
			}
			if (rightSide != -1) break;
		}
		font[i].w = (rightSide - leftSide) + 1;
		
		SDL_UnlockSurface(fontSurf);
		
		// Blit this character and pre-render different color versions
		// of it
		for (uint j = 0; j < NUM_FONT_COLORS; j++) {
			// Prepare a surface for this color of the character
			font[i].surf[j] = MakeSurface(FONT_W, FONT_H);
			
			// Blit the temporary fontSurf onto this letter's
			// surface
			ApplySurface(-leftSide, sourceOffset, fontSurf,
			             font[i].surf[j]);
			
			// Get the rgb values for this color
			switch (j) {
				case 0: // Shadow
					r = 0;
					g = 0;
					b = 0;
					break;
				case 1: // Normal text
					r = 220;
					g = 220;
					b = 220;
					break;
				case 2: // Highlighted text
					r = 251;
					g = 177;
					b = 17;
					break;
				case 3: // Title
					r = 255;
					g = 255;
					b = 255;
			}
			
			// Set the palette of the surface
			for (uint k = 0; k < 256; k++) {
				palette[k].r = static_cast<Uint8>(r);
				palette[k].g = static_cast<Uint8>(g);
				palette[k].b = static_cast<Uint8>(b);
			}
			
			// Change the palette of the surface
			SDL_SetPalette(font[i].surf[j], SDL_LOGPAL, palette, 0,
			               256);
		}
		
		i++;
		if (i > FONT_ARRAY_SIZE - 1) break;
		
		sourceOffset -= FONT_H; // Move the big tall bmp up
	}
	
	SDL_FreeSurface(fontSurf);

	return 0;
}



void UnloadFont() {
	for (uint i = 0; i < FONT_ARRAY_SIZE; i++) {
		for (uint j = 0; j < NUM_FONT_COLORS; j++) {
			SDL_FreeSurface(font[i].surf[j]);
		}
	}
}


// Overloaded to allow for <const char *>
void DrawText(int x, int y, const char *text, int color, int spacing) {
	char temp[strlen(text) + 1];
	strcpy(temp, text);
	
	DrawText(x, y, temp, color, spacing);
}

void DrawText(int x, int y, char *text, int color, int spacing) {
	int x2 = x; // The working x-coordinate (so we don't lose the real x
	            // argument)
	const int tabW = FONT_W * 4; // Tab width
	int c; // current character

	// Guard against invalid color values
	if (color < 0) {
		color = 0;
	} else if (color > static_cast<int>(NUM_FONT_COLORS) - 1) {
		color = NUM_FONT_COLORS - 1;
	}
	
	// Loop over each character of text
	for (uint i = 0; i < static_cast<uint>(strlen(text)); i++) {
		// Store the current character
		c = text[i];
		
		// If this is a line-break character
		if (c == '\n') {
			// Move y down, and add a little padding
			y += FONT_H + 2;
			
			// Move x2 back to the original x
			x2 = x;
			
			// Go to the next character
			continue;
		}

		// Tab
                if (c == '\t') {
                        x2 = (tabW * static_cast<int>(x2 / tabW)) + tabW;
                        
                        // Go to next character
                        continue;
                }	

		// Subtract 33 to find the font index number (so that '!'
		// starts at 0)
		c -= 33;

		// If it's a space
		if (c == -1) {
			x2 += FONT_W + spacing;
		} else {
			// Blit a "shadow" character
			ApplySurface(x2 + 2, y + 2, font[c].surf[0],
				screenSurface);
			
			// Blit the correct color version of the character
			ApplySurface(x2, y, font[c].surf[color], screenSurface);
			
			// Advance the x2 position
			x2 += (font[c].w + 2) + spacing;
		}
	}
}
