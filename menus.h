/*
 *   menu.h
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

/***** MENUITEM CLASS *****/
class menuItem {
	public:
		// Constructor
		menuItem():text(NULL) {}
	
		/*** Get ***/
		int GetX() const { return x; };
		int GetY() const { return y; };
		int GetW() const { return GetTextW(text); }; // GetTextW is in font.cpp
		char* GetText() const { return text; };
	
		/*** Set ***/
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void DelText() { delete [] text; text = NULL; }; // This must be called before SetText
		void SetText(char *txt);
	
	private:
		int x;
		int y;
		char *text;
};

void menuItem::SetText(char *txt) {
	text = new char[strlen(txt) + 1];
	strcpy(text, txt);
}





/***** MENU CLASS *****/
class menu {
	public:
		// Constructor protoype
		menu(uint numberOfItems);
		
		/*** Get ***/
		int GetSel() const { return sel; };
		
		/*** Set ***/
		void Move(int xPos, int yPos) { x = xPos; y = yPos; };
		void NameItem(uint item, const char *name);
		void NameItem(uint item, char *name);
		
		void SetTitle(const char *aTitle);
		void SetTitle(char *aTitle);
		
		/*** Other ***/
		void AutoArrange(char type);
		void Display();
		int Input();
	private:
		char *title;		// Title of the menu.
		uint numItems;		// How many menuItems there are
		menuItem *items;	// Pointer to hold array of menuItems
		int sel;		// Currently selected item
		int x, y;		// Position of the entire menu
		int titleX, titleY;	// Position of the title (influenced by AutoArrange's type)
};

// Constructor
menu::menu(uint numberOfItems):
	title(NULL),
	sel(0)
{
	numItems = numberOfItems;
	items = new menuItem[numItems];
}

// Overloaded for both <const char *> and <char *>
void menu::SetTitle(const char *aTitle) {
	// Clean up old text data
	delete [] title;
	title = NULL;
	
	// Convert <const char *> to <char *>
	char tempTitle[strlen(aTitle) + 1];
	strcpy(tempTitle, aTitle);

	title = new char[strlen(tempTitle) + 1];
	strcpy(title, tempTitle);
}
void menu::SetTitle(char *aTitle) {
	// Clean up old text data
	delete [] title;
	title = NULL;
	
	title = new char[strlen(aTitle) + 1];
	strcpy(title, aTitle);
}

// Overloaded for both "const char *" and "char *"
void menu::NameItem(uint item, const char *name) {
	// Clean up old text data
	items[item].DelText();
	
	char tempName[strlen(name) + 1];
	strcpy(tempName, name);
	items[item].SetText(tempName);
}
void menu::NameItem(uint item, char *name) {
	// Clean up old text data
	items[item].DelText();
	
	items[item].SetText(name);
}


// Arranges menu items according to type:
//	type = 0
//	Position first item at the menu's (x, y) and all other items
//	below it with a bit of space between each of them.
//
//	type = 1
//	Same as type 0, but items are centered on the menu's x-coordinate
void menu::AutoArrange(char type) {
	titleX = x;
	titleY = y;
	// Position first item right below title, and to the right a little
	items[0].SetX(x + TILE_W);
	items[0].SetY(y + TILE_H + (TILE_H / 4));
	
	// Type 0
	for (uint i = 1; i < numItems; i++) {
		items[i].SetX(items[i - 1].GetX());
		items[i].SetY(items[i - 1].GetY() + TILE_H + (TILE_H / 4));
	}

	// Type 1 uses type 0's y-positioning, but changes the x to be centered
	if (type == 1) {
		titleX = x - (GetTextW(title) / 2);
		for (uint i = 0; i < numItems; i++) {
			items[i].SetX(x - (items[i].GetW() / 2));
		}
	}
}


void menu::Display() {
	uint r, g, b;
	
	if (title != NULL) {
		DrawText(titleX, titleY, title, 255, 255, 255);
	}
	for (uint i = 0; i < numItems; i++) {
		if (i == static_cast<uint>(sel)) {
			r = 251;
			g = 177;
			b = 17;
			//r = g = b = 255;
		}
		else {
			//r = 186;
			//g = 109;
			//b = 33;
			r = g = b = 220;
		}
		DrawText(items[i].GetX(), items[i].GetY(), items[i].GetText(), r, g, b);
	}
}


int menu::Input() {
	int key = MenuInput(); // In input.cpp
	
	switch (key) {
		case 1: // Up
			sel--;
			break;
		case 2: // Down
			sel++;
			break;
		case 5: // Enter
			return 1;
			break;
		case 6: // Home
			sel = 0;
			break;
		case 7: // End
			sel = numItems - 1;
			break;
		case 8: // Esc
			return -1;
			break;
		case 9: // Close window
			return -2;
			break;
	}
	
	if (sel < 0) sel = 0;
	if (sel > static_cast<int>(numItems - 1)) sel = static_cast<int>(numItems - 1);
	
	return 0;
}
