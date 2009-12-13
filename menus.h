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


/*** Function Prototypes ***/
int ControlSetupMenu(bool inGame);
int Credits();
int EndOfLevelMenu();
int HelpMenu(bool inGame);
int MainMenu();
int OptionsMenu(bool inGame);
int PauseMenu();
int ReplayPauseMenu();
int SelectLevelMenu();





/*** menuItem class ***/
class menuItem {
	public:
		// Constructor
		menuItem():
			text(NULL),
			leftArrow(false),
			rightArrow(false)
			{}
	
		// Destructor Prototype
		~menuItem();
	
		/*** Get ***/
		int GetX() const { return x; };
		int GetY() const { return y; };
		int GetW(int letterSpacing) const;
		char* GetText() const { return text; };
		bool GetLeftArrow() const { return leftArrow; };
		bool GetRightArrow() const { return rightArrow; };
	
		/*** Set ***/
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void DelText() { delete [] text; text = NULL; }; // This must be called before SetText
		void SetText(char *txt);
		
		void SetLeftArrow(bool on) { leftArrow = on; };
		void SetRightArrow(bool on) { rightArrow = on; };
	
	private:
		int x;
		int y;
		char *text;
		bool leftArrow;		// Left flashing arrow
		bool rightArrow;	// Right flashing arrow
};

// Destructor
menuItem::~menuItem() {
	DelText();
}


int menuItem::GetW(int letterSpacing) const {
	// GetTextW is in font.cpp
	return GetTextW(text, letterSpacing);
};

void menuItem::SetText(char *txt) {
	DelText();
	
	text = new char[strlen(txt) + 1];
	strcpy(text, txt);
}





/*** menu class ***/
class menu {
	public:
		// Constructor protoype
		menu(uint numberOfItems);
		
		// Destructor prototype
		~menu();
		
		/*** Get ***/
		int GetItemX(uint item) { return items[item].GetX(); };
		int GetItemY(uint item) { return items[item].GetY(); };
		int GetItemW(uint item, int spacing) { return items[item].GetW(spacing); };
		int GetSel() const { return sel; };
		
		/*** Set ***/
		void SetSel(int s) { sel = s; };
		void Move(int xPos, int yPos) { x = xPos; y = yPos; };
		void MoveItem(uint item, int xPos, int yPos);
		
		void NameItem(uint item, const char *name);
		void NameItem(uint item, char *name);

		void SetTitle(const char *aTitle);
		void SetTitle(char *aTitle);
		
		void SetLeftArrow(uint item, bool on) { items[item].SetLeftArrow(on); };
		void SetRightArrow(uint item, bool on) { items[item].SetRightArrow(on); };
		
		/*** Other ***/
		void AutoArrange(char type);
		void Display();
		int Input();
		void SpaceItems(uint startItem);
	
	private:
		char *title;		// Title of the menu.
		int titleLetterSpacing;	// Letter spacing increase for title text.
		uint numItems;		// How many menuItems there are
		menuItem *items;	// Pointer to hold array of menuItems
		int sel;		// Currently selected item
		int x, y;		// Position of the entire menu
		int titleX, titleY;	// Position of the title (influenced by AutoArrange's type)
		uint arrowTimer;	// Timer for flashing arrows
};

// Constructor
menu::menu(uint numberOfItems):
	title(NULL),
	titleLetterSpacing(4),
	sel(0),
	arrowTimer(0)
{
	numItems = numberOfItems;
	items = new menuItem[numItems];
}

// Destructor
menu::~menu() {
	delete [] title;
	title = NULL;
	
	delete [] items;
	items = NULL;
}



// Overloaded for both <const char *> and <char *>
void menu::SetTitle(const char *aTitle) {
	// Clean up old text data
	delete [] title;
	title = NULL;
	
	// Convert <const char *> to <char *>
	char tempTitle[strlen(aTitle) + 1];
	strcpy(tempTitle, aTitle);

	this->SetTitle(tempTitle);
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
	char tempName[strlen(name) + 1];
	strcpy(tempName, name);
	items[item].SetText(tempName);
}
void menu::NameItem(uint item, char *name) {
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
	items[0].SetX(x + FONT_W);
	items[0].SetY(y + FONT_H + (FONT_H / 4));
	
	// Type 0 (left-aligned)
	for (uint i = 1; i < numItems; i++) {
		items[i].SetX(items[i - 1].GetX());
		items[i].SetY(items[i - 1].GetY() + FONT_H + (FONT_H / 4));
	}

	// Type 1 uses type 0's Y-positionings, but changes the Xs to be centered
	if (type == 1) {
		titleX = x - (GetTextW(title, titleLetterSpacing) / 2);

		for (uint i = 0; i < numItems; i++) {
			items[i].SetX(x - (items[i].GetW(0) / 2));	
		}
	}
}



void menu::MoveItem(uint item, int xPos, int yPos) {
	items[item].SetX(xPos);
	items[item].SetY(yPos);
}




void menu::Display() {
	int x; // Keeps track of text cursor x position
	char arrow[2];
	static bool arrowFlash = true;
	
	// Make the arrow flash, staying on longer than off
	if (SDL_GetTicks() <= arrowTimer + 750) {
		arrowFlash = true;
	}
	else {
		arrowFlash = false;
	}
	if (SDL_GetTicks() >= arrowTimer + 1000) {
		arrowFlash = false;
		arrowTimer = SDL_GetTicks();
	}


	if (title != NULL) {
		DrawText(titleX, titleY, title, false, 0, titleLetterSpacing, 3);
	}
	for (uint i = 0; i < numItems; i++) {
		// Draw the item's text
		if (items[i].GetText() != NULL) {
			DrawText(items[i].GetX(), items[i].GetY(), items[i].GetText(), false, 0, 0,
				(i == static_cast<uint>(sel)) ? 2 : 1);
		}
	
		

		if (sel == static_cast<int>(i)) {
			if (arrowFlash && items[i].GetLeftArrow()) {
				x = items[i].GetX() - (FONT_W * 2);
				
				sprintf(arrow, "<");
				DrawText(x, items[i].GetY(), arrow, 2);
			}
		
			if (arrowFlash && items[i].GetRightArrow()) {
				x = items[i].GetX() + GetTextW(items[i].GetText(), 0) + FONT_W;
				
				sprintf(arrow, ">");
				DrawText(x, items[i].GetY(), arrow, 2);
			}
		}
	}	
}


int menu::Input() {
	int key = MenuInput(); // In input.cpp
	int oldSel = sel;
	
	switch (key) {
		case 1: // Up
			sel--;
			arrowTimer = SDL_GetTicks();
			break;
		case 2: // Down
			sel++;
			arrowTimer = SDL_GetTicks();
			break;
		case 3: // Left
			if (items[sel].GetLeftArrow()) {
				PlaySound(5);
				arrowTimer = SDL_GetTicks();
			}
			return 3;
			break;
		case 4: // Right
			if (items[sel].GetRightArrow()) {
				PlaySound(5);
				arrowTimer = SDL_GetTicks();
			}
			return 4;
			break;
		case 5: // Enter
			PlaySound(6);
			return 1;
			break;
		case 6: // Home
			sel = 0;
			arrowTimer = SDL_GetTicks();
			break;
		case 7: // End
			sel = numItems - 1;
			arrowTimer = SDL_GetTicks();
			break;
		case 10: // Page Up
			sel -= 7;
			arrowTimer = SDL_GetTicks();
			break;
		case 11: // Page Down
			sel += 7;
			arrowTimer = SDL_GetTicks();
			break;
		case 8: // Esc
			PlaySound(7);
			return -1;
			break;
		case 9: // Close window
			return -2;
			break;
	}
	
	if (sel < 0) sel = 0;
	if (sel > static_cast<int>(numItems - 1)) sel = static_cast<int>(numItems - 1);
	
	if (sel != oldSel) PlaySound(5);
	
	return 0;
}


void menu::SpaceItems(uint startItem) {
	for (uint i = startItem; i < numItems; i++) {
		items[i].SetY(items[i].GetY() + FONT_H);
	}
}
