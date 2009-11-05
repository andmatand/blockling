/*
 *   font.cpp
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


int GetTextW(char *text, int spacing) {
	if (text == NULL) return 0;
	
	//const int tW = FONT_W * 4; // Tab Width
	
	int w = 0;
	int lW = 0; // The current line's width
	int c;
	
	for (uint i = 0; i < static_cast<unsigned int>(strlen(text)); i++) {
		c = text[i];
		
		// Line break
		if (c == '\n') {
			lW = 0;
			
			// Go to next character
			continue;
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
				lW += FONT_W + spacing;
				break;
			default:
				lW += font[c].w;
				
				// If this is not the last character, add a bit of space
				if ((i + 1) != static_cast<unsigned int>(strlen(text))) lW += 2 + spacing;
				
				break;
		}
		
		if (lW > w) w = lW;
	}
	
	return w;
}


void LoadFont(const char *file) {
	char fullPath[256];
	sprintf(fullPath, "%s%s%s", DATA_PATH, FONT_PATH, file);
	printf("fullPath = %s\n", fullPath);
	
	// The surface holding the big long list of characters
	SDL_Surface *fontSurf = FillSurface(fullPath, 0);
	
	// For holding the rgb values for pre-rendering differently-colored letters
	SDL_Color palette[256];
	uint r, g, b;
	
	// For finding the left and right side of each character (to determine width)
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
		
		//std::cout << "\nWidth of character " << static_cast<char>(i + 33) << " is " << font[i].w << "\n";
		//std::cout << "leftSide = " << leftSide << "\n";
		//std::cout << "rightSide = " << rightSide << "\n";

		/*
		// Determine the letter's yOffset
		font[i].yOffset = -1; // Serves as a flag that yOffset hasn't been set yet.
		for (int y2 = y; y2 < y + FONT_H; y2++) {
			for (int x2 = 0; x2 < chrW; x2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					font[i].yOffset = y2 - y;
					break;
				}
			}
			if (font[i].yOffset != -1) break;
		}
		*/

		SDL_UnlockSurface(fontSurf);
		
		/*** Blit this character and pre-render different color versions of it **/
		for (uint j = 0; j < NUM_FONT_COLORS; j++) {
			// Prepare a surface for this color of the character
			font[i].surf[j] = MakeSurface(FONT_W, FONT_H);
			
			// Blit the temporary fontSurf onto this letter's surface
			ApplySurface(-leftSide, sourceOffset, fontSurf, font[i].surf[j]);
			
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
			SDL_SetPalette(font[i].surf[j], SDL_LOGPAL, palette, 0, 256);
		}
		
		i++;
		if (i > FONT_ARRAY_SIZE - 1) break;
		
		sourceOffset -= FONT_H; // Move the big tall bmp up
	}
	
	SDL_FreeSurface(fontSurf);
}



void UnloadFont() {
	for (uint i = 0; i < FONT_ARRAY_SIZE; i++) {
		for (uint j = 0; j < NUM_FONT_COLORS; j++) {
			SDL_FreeSurface(font[i].surf[j]);
		}
	}
}




void DrawText(int x, int y, char *text, int spacing, int color) {
	const int tW = FONT_W * 4; // Tab Width
	int x2 = x;
	int c;

	// Guard against invalid color values
	if (color < 0) color = 0;
	if (color > static_cast<int>(NUM_FONT_COLORS) - 1) color = NUM_FONT_COLORS - 1;
	
	for (uint i = 0; i < static_cast<uint>(strlen(text)); i++) {
		c = text[i];
		
		// Line break
		if (c == '\n') {
			y += FONT_H + 2;
			x2 = x;
			
			// Go to next character
			continue;
		}
		
		// Tab
		if (c == '\t') {
			x2 = (tW * static_cast<int>(x2 / tW)) + tW;
			
			// Go to next character
			continue;
		}
	
		// Get font index number (so that '!' starts at 0)
		c -= 33;

		switch (c) {
			case -1: // Space (32)
				x2 += FONT_W + spacing;
				break;
			default:
				// Blit "shadow" character
				ApplySurface(x2 + 2, y + 2, font[c].surf[0], screenSurface);
				
				// Blit the correct color version of the character
				ApplySurface(x2, y, font[c].surf[color], screenSurface);
				
				x2 += (font[c].w + 2) + spacing;
				break;
		}
	}
}
