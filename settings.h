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


/** Function Prototypes ***/
void SaveSettings();
void LoadSettings();


/** Constants ***/
uchar NUM_OPTIONS = 9;


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
