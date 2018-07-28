#ifndef LIST_H
#define LIST_H

#include "menus.h"


// Arguments
typedef struct LIST_ARGS {
	wMENU *menu;
	int maxSelected;
	int dItems;
	
	int yItem;
	int cItem;
	int nSelected;
	wMENU *cMenu;  // dans le cas d'un BoxEplorer, pointe vers le path du dossier courant : char *cMenu.
	
	BOOL isActive;
} ListArgs;

Widget *wList(wMENU *m);
Widget *wExList(wMENU *m, int dItems, int maxSelected);
void DrawList(Widget *w);
int  ActivateList(Widget *w);
int OpenMenu(Widget *w, wMENU *m, int xr, int xl, int minWidth, int y);
void CloseList(Widget *w);


// Méthodes
void wList_SetMenu(Widget *w, wMENU *m);
wMENU *wList_GetMenu(Widget *w);
void wList_SetMaxSelectableItem(Widget *w, int max);

wMENU *wList_GetCurrentItem(Widget *w, int *nItem);
char *wList_GetCurrentStr(Widget *w);
void wList_GetCurrentPath(Widget *w, char *output);

wMENU *wList_GetSelectedItem(Widget *w, int *item);
char *wList_GetSelectedStr(Widget *w);
void wList_GetPath(Widget *w, char *output);

int wList_GetNumberOfSelectedItems(Widget *w);
int wList_GetSelectedItems(Widget *w, wMENU **menus, int *items);
int wList_GetSelectedStrings(Widget *w, char **strings);
int wList_GetPaths(Widget *w, char **output);

int wList_SelectItem(Widget *w, wMENU *menu, int item);
int wList_SelectItemStr(Widget *w, const char *str);
int wList_SelectPath(Widget *w, const char *path);
void wList_DeselectAll(Widget *w);


#endif