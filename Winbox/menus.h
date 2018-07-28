#ifndef MENUS_H
#define MENUS_H

#include <SDL/SDL.h>
#include "structures.h"

typedef struct WINBOX_MENU {
	int magicNumber;	// = 1
	char *title;
	int nItems;
	void **items;	// pointe vers un tableau de Menus et d'Items
	int nSelected;
	struct WINBOX_MENU *parent;
} wMENU;

typedef struct WINBOX_ITEM {
	int magicNumber;	// = 2
	char *str;
	BOOL isSelected;
} wITEM;

// Méthodes de Création / Destruction
wMENU *wMenu(const char *title);
wMENU *wMenu_NewItemList(const char *title, const char *str);
wMENU *wMenu_AddItem(wMENU *m, const char *str);
wMENU *wMenu_AddItemList(wMENU *m, const char *str);
void wMenu_SetItem(wMENU *m, const char *str, int nItem);
void wMenu_SetTitle(wMENU *m, const char *str);
wMENU *wMenu_AddMenu(wMENU *parent, wMENU *m);
wMENU *wMenu_FileList(const char *title, const char *path, const char *pattern, int depth);
wMENU *wMenu_FolderList(const char *title, const char *path, const char *pattern);
void   wMenu_Close(wMENU *m);


int wMenu_GetSelectedItem(wMENU *m);
char *wMenu_GetSelectedStr(wMENU *m);
BOOL wMenu_HasSelectedItemOrMenu(wMENU *m);
BOOL wMenu_IsSelected(wMENU *m, int item);

// Ces fonctions ne devraient pas être utilisées par l'utilisateur
void wMenu_SelectItem(wMENU *m, int item);
void wMenu_DeselectAll(wMENU *m);
int wMenu_GetItemFromStr(wMENU *m, const char *itemStr);


#endif
