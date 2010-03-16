/*
 *   Copyright 2009 Andrew Anderson <www.billamonster.com>
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

/*** Function Prototypes ***/
int BlockNumber(int x, int y, int w, int h);
int BrickNumber(int x, int y, int w, int h);
int BoxContents(int x, int y, int w, int h);
bool BoxOverlap (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool OnSolidGround(int b);
