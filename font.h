/*
 *   font.h
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


/*** Function Prototypes ***/
void DrawText(int x, int y, char *text, int spacing, uint r, uint g, uint b);
int GetTextW(char *text, int spacing);




/*** Variable Declarations ***/
const uint FONT_ARRAY_SIZE = (122 - 33) + 1;
const int FONT_H = 16;
const int FONT_W = 16;

typedef struct {
	uint w;
	SDL_Surface * surf;
} letter;

// Global struct which holds all available letters in the font
letter font[FONT_ARRAY_SIZE];
