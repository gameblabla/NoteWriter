#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "general.h"
#include "widgets.h"
#include "keys.h"
#include "Window.h"
#include "menus.h"
#include "List.h"


// Création
Widget *wWindow(const char *title)
{
	return wExWindow(title, NULL, NULL);
}

Widget *wExWindow(const char *title, Uint16 *icon, wMENU *menu)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_WINDOW;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = 0;
	w->background = NULL;
	w->isDynamic = 1;
	w->isLayout = 1;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 320;
	w->bounds.h = 240;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawWindow;
	w->activate	= ActivateWindow;
	w->close		= CloseWindow;
	w->add		= AddWindow;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(WindowArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	WindowArgs *args = w->args;
	args->title			= malloc(strlen(title)+1);
	strcpy(args->title, title);
	args->list			= wList(menu);
	args->widget		= NULL;
	args->title_backg	= NULL;
	
	if (args->list) {
		args->list->bounds.w = 0;
		args->list->bounds.h = 0;
	}
	
	if (icon) {
		args->icon		= nSDL_LoadImage(icon);
		SDL_SetColorKey(args->icon, SDL_SRCCOLORKEY, RGB(255,0,0));
	}
	else
		args->icon		= NULL;
		
	return w;
}

// Ajout de Widget
int AddWindow(Widget *w, Widget *child, int spot)
{
	if (!w || !child || spot != -1) return 0;
	WindowArgs *args = w->args;
	wAddWidgetToConstruct(w->construct, args->widget);
	args->widget = child;
	return 1;
}


// Dessin
void DrawWindow(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	WindowArgs *args = w->args;
	int x;
	Widget *child;
	int y = w->bounds.y;
	if (args->title) y += 14;
	
	
	// on associe le construct à la list
	if (args->list) {
		args->list->freeArgsType = w->freeArgsType;
		wAddWidgetToConstruct(w->construct, args->list);
		if (!args->list->parent) args->list->parent = w;
	}
	
	if (args->widget) {
		// Dessin du Widget principal
		SDL_Rect r = {w->bounds.x+1, y+1, w->bounds.w-2, w->bounds.h-(args->title? 16 : 1)};
		child = args->widget;
		wFindBounds(child, &r, 1, 1);
	
		// if (wIsActivable(child))
			wDrawWidget(child);
	}
	
	// Dessin des bordures
	DrawRect(scr, &w->bounds, t->window_c1);
	
	// Dessin du titre
	if (args->title) {
		DrawFillRectXY(scr, w->bounds.x, y, w->bounds.w, 1, t->window_c1);
		wBG_Draw(t->window_title_bg, &(SDL_Rect) {w->bounds.x+1, y-13, w->bounds.w-2, 13});
		x = nSDL_GetStringWidth(t->window_title_font, args->title);
		DrawClippedStr(scr, t->window_title_font, w->bounds.x + (w->bounds.w - x)/2, y-11, args->title);
		
		// Dessin de l'icone
		if (args->icon)
			DrawSurface(args->icon, NULL, scr, &(SDL_Rect) {w->bounds.x+2, y-12,11,11});
	}
	
}




// Activation
static BOOL dynamicChild(Widget *w)
{
	return w && w->isDynamic;
}

int ActivateWindow(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	WindowArgs *args = w->args;
	ListArgs *lArgs;
	if (args->list) lArgs = args->list->args;
	else lArgs = NULL;
	int ok = ACTION_CONTINUE;
	
	if (!dynamicChild(args->widget)) SDL_Flip(scr);
	
	// on associe le callback de la window à la liste
	if (w->callBack && args->list) {
		if (!args->list->callBack) {
			args->list->callBack = w->callBack;
		}
	}


	if (args->widget == NULL)
		SDL_Flip(scr);
	
	// Début de la boucle
	while (!K_ESC()) {
		if (dynamicChild(args->widget)) {
			ok = wActivateWidget(args->widget);
			if (ok < 1) break;	// Si erreur, appui sur ESC ou actualisation nécéssaire
			if ((!K_MENU() || !lArgs) && ok == 2) break;
		}
		
		if (K_MENU() && lArgs) {
			ok = OpenMenu(args->list, lArgs->menu, w->bounds.x+1, 0, 110, w->bounds.y + (args->title? 15:1));
			if (ok != ACTION_CONTINUE) break;
			if (!dynamicChild(args->widget)) SDL_Flip(scr);
			while (K_ESC() || K_LEFT());
		}
		
		else if (any_key_pressed()) {
			ok = wExecCallback(w, SIGNAL_KEY);
			if (ok != ACTION_CONTINUE) break;
			SDL_Flip(scr);
		}
	}
	
	return ok;
}



// Fermeture
void CloseWindow(Widget *w)
{
	if (!w) return;
	WindowArgs *args = w->args;
	
	if (args) {
		free(args->title);
		wAddWidgetToConstruct(w->construct, args->widget);
		wAddWidgetToConstruct(w->construct, args->list);
		wAddSurfaceToConstruct(w->construct, args->icon);
		if (args->title_backg && wIsFreedArg(w, WBACKG)) {
			wAddEltToConstruct(w->construct, args->title_backg);
			wAddSurfaceToConstruct(w->construct, args->title_backg->image);
		}
	}
}


// Méthodes
void wWindow_SetTitle(Widget *w, const char *title)
{
	if (!w || !title) return;
	WindowArgs *args = w->args;
	args->title = realloc(args->title, strlen(title)+1);
	strcpy(args->title, title);
}


void wWindow_SetTitleBackground(Widget *w, wBACKGROUND *bg)
{
	if (!w || !bg) return;
	WindowArgs *args = w->args;
	args->title_backg = bg;
}

Widget *wWindow_GetList(Widget *w)
{
	if (!w) return NULL;
	WindowArgs *args = w->args;
	return args->list;
}


char *wWindow_GetTitle(Widget *w)
{
	if (!w) return NULL;
	WindowArgs *args = w->args;
	return args->title;
}

