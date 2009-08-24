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


typedef struct {
	uint w;
	//int yOffset;
	SDL_Surface * surf;
} letter;

letter font[(122 - 33) + 1];

void LoadFont(const char * file) {
	int chrW = 16; // character width in BMP file
	int chrH = 16; // character height in BMP file
	
	// The surface holding the big long list of characters
	SDL_Surface *fontSurf = FillSurface(file, 0);
	
	// For finding the left and right side of each character (to determine width)
	int leftSide;
	int rightSide;
	
	uint i = 0;
	uint transColor = SDL_MapRGB(fontSurf->format, 0xff, 0x00, 0xff);
	int sourceOffset = 0;
	for (int y = 0; y < fontSurf->h; y += chrH) {
		/*** Determine the letter's width ***/
		
		// Find left side
		leftSide = -1;
		for (int x2 = 0; x2 < chrW; x2++) {
			for (int y2 = y; y2 < y + chrH; y2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					leftSide = x2;
					break;
				}
			}
			if (leftSide != -1) break;
		}
		
		// Find right side
		rightSide = -1;
		for (int x2 = chrW - 1; x2 > 0; x2--) {
			for (int y2 = y; y2 < y + chrH; y2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					rightSide = x2;
					break;
				}
			}
			if (rightSide != -1) break;
		}
		font[i].w = (rightSide - leftSide) + 1;
		
		std::cout << "\nWidth of character " << i << " is " << font[i].w << "\n";
		std::cout << "leftSide = " << leftSide << "\n";
		std::cout << "rightSide = " << rightSide << "\n";

		/*
		// Determine the letter's yOffset
		font[i].yOffset = -1; // Serves as a flag that yOffset hasn't been set yet.
		for (int y2 = y; y2 < y + chrH; y2++) {
			for (int x2 = 0; x2 < chrW; x2++) {
				if (GetPixel(fontSurf, x2, y2) != transColor) {
					font[i].yOffset = y2 - y;
					break;
				}
			}
			if (font[i].yOffset != -1) break;
		}
		*/
		
		font[i].surf = MakeSurface(chrW, chrH);
		ApplySurface(0, sourceOffset, fontSurf, font[i].surf);
		i++;
		sourceOffset -= chrH; // Move the big tall bmp up
	}
	
	SDL_FreeSurface(fontSurf);
}



void DrawText(char *text, int x, int y) {
	int c;
	for (uint i = 0; i < static_cast<uint>(strlen(text)); i++) {
		// Get index number (so that '!' starts at 0)
		c = text[i] - 33;

		switch (c) {
			case -1:
				//x += font[].w;
				x += 16;
			default:
				ApplySurface(x, y, font[c].surf, screenSurface);
				x += font[c].w + 1;
				break;
		}
	}
}
