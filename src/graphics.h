/*
 *   graphics.h
 *	  
 *   Copyright 2010 Andrew Anderson <billamonster.com>
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


void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
void CenterCamera(char instant);
void DrawBackground();
SDL_Surface* FillSurface(const char *file, bool transparent);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);
void LimitFPS();
void LoadTileset(const char *tilesetDir);
bool LockSurface(SDL_Surface *surf);
void ManualCameraMovement();
void MoveCamera();
SDL_Surface* MakeSurface(int width, int height);
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

void Render(char flags);
// Flags:
enum {
	RENDER_UPDATESCREEN = 0x1,
	RENDER_MOVECAMERA   = 0x2,
	RENDER_ANIMATE      = 0x4,
	RENDER_BG           = 0x8
};	

void SetCameraTargetBlock(uint b);
SDL_Surface* TileSurface(char *path, const char *file, bool transparent);
void ToggleFullscreen();
void UnlockSurface(SDL_Surface *surf);
void UnloadTileset();
void UpdateScreen();
