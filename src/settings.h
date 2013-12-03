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


/** Function Prototypes ***/
char* ReadLine(FILE *file, uint maxLineLength);
void SaveSettings();
void LoadSettings();


/** Constants ***/
const uchar NUM_OPTIONS = 13;


/** Options ***/
uint option_undoSize;
bool option_soundOn;
bool option_musicOn;
uchar option_levelSet;
bool option_replayOn;
uchar option_replaySpeed;
uchar option_background;
bool option_timerOn;
uchar option_cameraMode;
char option_tileset[16 + 1];
bool option_helpSpeech;
uchar option_levelMax0;
uchar option_levelMax1;
