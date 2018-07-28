#ifndef WINDOW_H
#define WINDOW_H

#include "menus.h"

// Arguments
typedef struct WINDOW_ARGS {
	Widget		*widget;
	char			*title;
	wBACKGROUND *title_backg;
	SDL_Surface	*icon;
	Widget		*list;	// est de type 'WIDGET_LIST'
} WindowArgs;

// Création
Widget *wWindow(const char *title);
Widget *wExWindow(const char *title, Uint16 *icon, wMENU *menu);
void DrawWindow(Widget *w);
int ActivateWindow(Widget *w);
void CloseWindow(Widget *w);
int AddWindow(Widget *w, Widget *child, int spot);

// Méthodes
void wWindow_SetTitleBackground(Widget *w, wBACKGROUND *bg);
void wWindow_SetTitle(Widget *w, const char *title);
Widget *wWindow_GetList(Widget *w);
char *wWindow_GetTitle(Widget *w);

#endif