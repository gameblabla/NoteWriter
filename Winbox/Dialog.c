#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "general.h"
#include "widgets.h"
#include "keys.h"
#include "Dialog.h"
#include "Button.h"

Widget *wDialog1(const char *title, const char *but)
{
	return wDialog2(title, but, NULL);
}

Widget *wDialog2(const char *title, const char *button1, const char *button2)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_DIALOG;
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
	
	w->bounds.w = 240;
	w->bounds.h = 180;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawDialog;
	w->activate	= ActivateDialog;
	w->close		= CloseDialog;
	w->add		= AddDialog;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(DialogArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	DialogArgs *args = w->args;
	if (title) {
		args->title			= malloc(strlen(title)+1);
		strcpy(args->title, title);
	}
	else args->title = NULL;
	args->button1		= button1? wButton(button1, BUTTON_PRESS) : NULL;
	args->button2		= button2? wButton(button2, BUTTON_PRESS) : NULL;
	args->widget		= NULL;
	args->title_backg	= NULL;
	
	return w;
}


// Ajout de Widget
int AddDialog(Widget *w, Widget *child, int spot)
{
	if (!w || !child || spot != -1) return 0;
	DialogArgs *args = w->args;
	wAddWidgetToConstruct(w->construct, args->widget);
	args->widget = child;
	return 1;
}



// Dessin
void DrawDialog(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	DialogArgs *args = w->args;
	int x;
	Widget *child;
	int y = w->bounds.y;
	
	// Définition des caractéristiques des boutons
	wButton_SetBackground(args->button1, t->dialog_bg1, t->button_bg_selected);
	wButton_SetBackground(args->button2, t->dialog_bg2, t->button_bg_selected);
		
	// Dessin des bordures
	DrawRect(scr, &w->bounds, t->window_c1);
	DrawFillRectXY(scr, w->bounds.x, y+14, w->bounds.w, 1, t->window_c1);
	
	// Dessin du titre
	if (args->title) {
		wBG_Draw(t->window_title_bg, &(SDL_Rect) {w->bounds.x+1, y+1, w->bounds.w-2, 13});
		x = nSDL_GetStringWidth(t->window_title_font, args->title);
		DrawClippedStr(scr, t->window_title_font, w->bounds.x + (w->bounds.w - x)/2, y+3, args->title);
		y += 14;
	}
	
	
	// Dessin des boutons
	if (args->button1) {
		args->button1->construct = w->construct;
		args->button1->parent = w;
		
		if (args->button2) {	// deux boutons
			args->button2->construct = w->construct;
			args->button2->parent = w;
			
			args->button1->bounds = (SDL_Rect) {w->bounds.x+(7*w->bounds.w)/100, w->bounds.y+w->bounds.h-21, (36*w->bounds.w)/100, 18};
			args->button2->bounds = (SDL_Rect) {w->bounds.x+(57*w->bounds.w)/100, args->button1->bounds.y, (36*w->bounds.w)/100, 18};
			wDrawWidget(args->button1);
			wDrawWidget(args->button2);
		}
		
		else { // Un bouton
			args->button1->bounds = (SDL_Rect) {w->bounds.x+(14*w->bounds.w)/100, w->bounds.y+w->bounds.h-21, (72*w->bounds.w)/100, 18};
			wDrawWidget(args->button1);
		}
	}
	
	// Dessin du Widget principal
	SDL_Rect r = {w->bounds.x+1, y+1, w->bounds.w-2, w->bounds.h-(args->title? 38 : 24)};
	if (!args->button1) r.h += 22;
	child = args->widget;
	wFindBounds(child, &r, 1, 1);
	if (wIsActivable(child))
		wDrawWidget(child);
}


// Activation
static BOOL dynamicChild(Widget *w)
{
	return w && w->isDynamic;
}

int ActivateDialog(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	DialogArgs *args = w->args;
	int ok = 2;
	ButtonArgs *bArgs = NULL;
	
	if (!dynamicChild(args->widget)) {
		SDL_Flip(scr);
		while (K_ENTER());
	}
	
	// Début de la boucle
	while (!K_ESC() && !K_ENTER()) {
		if (dynamicChild(args->widget)) {
			ok = wActivateWidget(args->widget);
			if (ok < 2) break;
		}
	}
	
	if (K_ESC() && args->button1 && args->button2) {
		bArgs = args->button2->args;
		bArgs->state = 2;
		DrawButton(args->button2);
		SDL_Flip(scr);
		
		while (K_ESC());
		bArgs->state = BUTTON_PRESS;
		DrawButton(args->button2);
		SDL_Flip(scr);
		return 0;
	}
	else if (K_ENTER() || K_ESC()) {
		if (args->button1) {
			bArgs = args->button1->args;
			bArgs->state = 2;
			DrawButton(args->button1);
			SDL_Flip(scr);
		}
		
		while (K_ESC() || K_ENTER());
		
		if (args->button1) {
			bArgs->state = BUTTON_PRESS;
			DrawButton(args->button1);
			SDL_Flip(scr);
		}
		return 1;
	}
	return ok;
}


// Fermeture
void CloseDialog(Widget *w)
{
	if (!w) return;
	DialogArgs *args = w->args;
	
	if (args) {
		free(args->title);
		wAddWidgetToConstruct(w->construct, args->widget);
		if (args->title_backg && wIsFreedArg(w, WBACKG)) {
			wAddEltToConstruct(w->construct, args->title_backg);
			wAddSurfaceToConstruct(w->construct, args->title_backg->image);
		}
		if (args->button1) free(args->button1);
		if (args->button2) free(args->button2);
	}
}



// Méthodes
void wDialog_SetTitle(Widget *w, const char *title)
{
	if (!w || !title) return;
	DialogArgs *args = w->args;
	args->title = realloc(args->title, strlen(title)+1);
	strcpy(args->title, title);
}


char *wDialog_GetTitle(Widget *w)
{
	if (!w) return NULL;
	DialogArgs *args = w->args;
	return args->title;
}



void wDialog_SetTitleBackground(Widget *w, wBACKGROUND *bg)
{
	if (!w || !bg) return;
	DialogArgs *args = w->args;
	args->title_backg = bg;
}



