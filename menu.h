/*
 *   menu.cpp
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


class menuItem {
	public:
		/*** Contructor ***/
		menuItem(uint textLength);
	
		/*** Accessors ***/
		int GetX() const { return x; };
		int GetY() const { return y; };
		int GetW() const;
	
	private:
		int x;
		int y;
		char *text;
}

menuItem:menuItem() {
}

int menuItem:GetW() {
	return GetText
}




class menu {
	public:
		menu(uint numItems);	// Constructor
			
	private:
		menuItem *items;	
}

menu::menu(uint numItems) {
	items = new menuItem[numItems];
	
}
