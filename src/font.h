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


/* Function Prototypes */
void DrawText(int x, int y, const char *text, int color = 1, int spacing = 0);
void DrawText(int x, int y, char *text, int color = 1, int spacing = 0);
int GetTextW(char *text, int spacing = 0);
int GetTextH(char *text, int wrapWidth = 0, int spacing = 0);
char LoadFont(const char *file);


/* Variable/Constant Declarations */
const uint FONT_ARRAY_SIZE = (122 - 33) + 1;
const int FONT_H = 16;
const int FONT_W = 16;
const uint NUM_FONT_COLORS = 4; // Three colors, plus the shadow color (0)


/* letter Struct */
typedef struct {
	uint w;
	SDL_Surface * surf[NUM_FONT_COLORS];
} letter;

// Global struct which holds all available letters in the font
letter font[FONT_ARRAY_SIZE];
