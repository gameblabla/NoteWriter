#include <os.h>
#include <SDL/SDL.h>

#include "widgets.h"
#include "general.h"
#include "keys.h"
#include "ProgressBar.h"

// Création
Widget *wProgressBar(int max, int value, int color1, int color2)
{	
	return wExProgressBar(max, value, color1, color2, 0);
}

Widget *wActiveProgressBar(int max, int value, int color1, int color2)
{
	return wExProgressBar(max, value, color1, color2, 1);
}

Widget *wExProgressBar(int max, int value, int color1, int color2, BOOL isActivable)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_PROGRESS_BAR;
	w->Hexpansive = 1;
	w->Vexpansive = 0;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = 0;
	w->background = NULL;
	w->isDynamic = isActivable;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 100;
	w->bounds.h = 14;
	if (isActivable) w->bounds.h = 20;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawProgressBar;
	w->activate	= ActivateProgressBar;
	w->close		= NULL;
	w->add		= NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(ProgressBarArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	ProgressBarArgs *args = w->args;
	args->max		= max;
	args->value		= value;
	args->color1	= color1;
	args->color2	= color2;
	
	return w;
}

Widget *wProgressBarV(int max, int value, int color1, int color2)
{	
	return wExProgressBarV(max, value, color1, color2, 0);
}

Widget *wActiveProgressBarV(int max, int value, int color1, int color2)
{
	return wExProgressBarV(max, value, color1, color2, 1);
}

Widget *wExProgressBarV(int max, int value, int color1, int color2, BOOL isActivable)
{
	
	Widget *w = wExProgressBar(max, value, color1, color2, isActivable);
	if (!w) return NULL;
	w->type = WIDGET_PROGRESS_BAR_V;
	w->Hexpansive = 0;
	w->Vexpansive = 1;
	
	w->bounds.w = 14;
	w->bounds.h = 100;
	if (isActivable) w->bounds.w = 20;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	return w;
}


// Dessin
void DrawProgressBar(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	ProgressBarArgs *args = w->args;
	int value = args->value;
	if (value < 0) value = 0;
	if (value > args->max) value = args->max;
	
	SDL_Rect bar = w->bounds;
	SDL_Rect inbar;
	
	if (w->type == WIDGET_PROGRESS_BAR) {
		if (w->isDynamic || w->displayBounds)
			bar.x += 3, bar.y += 3, bar.w -= 6, bar.h -= 6;
		if (w->isDynamic) bar.x += 3, bar.w -= 6;
		inbar = bar;
		inbar.x++, inbar.y++, inbar.h-=2;
		inbar.w = ((bar.w-2) * value) / args->max;
	}
	
	else {
		if (w->isDynamic || w->displayBounds)
			bar.x += 3, bar.y += 3, bar.w -= 6, bar.h -= 6;
		if (w->isDynamic) bar.y += 3, bar.h -= 6;
		inbar = bar;
		inbar.x++, inbar.w-=2;
		inbar.h = ((bar.h-2) * value) / args->max;
		inbar.y = bar.y + bar.h - 1 - ((bar.h-2) * value) / args->max;
	}

	DrawFillRect(scr, &bar, 0);
	DrawRect(scr, &bar, Darker(args->color1, 15));
	if (w->type == WIDGET_PROGRESS_BAR)
		FillVGradientRect(scr, inbar, args->color1, args->color2);
	else
		FillHGradientRect(scr, inbar, args->color2, args->color1);
}



// Activation
int ActivateProgressBar(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ProgressBarArgs *args = w->args;
	int value;
	BOOL isH = (w->type == WIDGET_PROGRESS_BAR);
	int ok = ACTION_CONTINUE;
	
	SDL_Rect bar = w->bounds;
	SDL_Rect inbar;
	BOOL ood = w->construct->onlyOneDynamic;
	if (isH) {
		if (w->isDynamic || w->displayBounds)
			bar.x += 3, bar.y += 3, bar.w -= 6, bar.h -= 6;
		if (w->isDynamic) bar.x += 3, bar.w -= 6;
		inbar = bar;
		inbar.x++, inbar.y++, inbar.h-=2;
	}
	
	else {
		if (w->isDynamic || w->displayBounds)
			bar.x += 3, bar.y += 3, bar.w -= 6, bar.h -= 6;
		if (w->isDynamic) bar.y += 3, bar.h -= 6;
		inbar = bar;
		inbar.x++, inbar.w-=2;
	}
	
	SDL_Rect cursor;
	Uint32 c1 = Darker(args->color1, 15);
	Uint32 c2 = Darker(args->color2, 25);
	SDL_Surface *sCursor;
	if (isH) {
		cursor = (SDL_Rect) {0, inbar.y+(inbar.h-9)/2, 9, 9};
		sCursor = t->pbar_cursor;
	}
	else {
		cursor = (SDL_Rect) {inbar.x+(inbar.w-9)/2, 0, 9, 9};
		sCursor = t->pbar_cursor_v;
	}
	
	
	
	
	do {
		DrawRect(scr, &w->bounds, c1);
		DrawRectXY(scr, w->bounds.x+1,w->bounds.y+1,w->bounds.w-2,w->bounds.h-2, c2);
		if (ood) goto INLOOP;
		SDL_Flip(scr);
		wait_key_pressed();
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB() ||
			 K_SCRATCHPAD() || K_ENTER() || K_ESC()) break;
		
		else if (K_CLICK()) {
			
		  INLOOP:
			if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
				goto QUIT;
			do {
				value = args->value;
				if (value < 0) value = 0;
				if (value > args->max) value = args->max;
				
				if (isH) {
					inbar.w = ((bar.w-2) * value) / args->max;
					cursor.x = inbar.x + inbar.w - 5;
				}
				else {
					inbar.y = bar.y + bar.h - 1 - ((bar.h-2) * value) / args->max;
					inbar.h = ((bar.h-2) * value) / args->max;
					cursor.y = inbar.y - 5;
				}
				
				wDrawWidget(w);
				DrawRect(scr, &w->bounds, c1);
				DrawRectXY(scr, w->bounds.x+1,w->bounds.y+1,w->bounds.w-2,w->bounds.h-2, c2);
				DrawSurface(sCursor, NULL, scr, &cursor);
				SDL_Flip(scr);
				
			  KEY:
				if (!K_CTRL()) while (any_key_pressed());
				while (!any_key_pressed());
				
				if ((K_RIGHT() && isH) || (K_UP() && !isH)) {
					if (value >= args->max) goto KEY;
					args->value++;
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						goto QUIT;
				}
				
				else if ((K_LEFT() && isH) || (K_DOWN() && !isH)) {
					if (!value) goto KEY;
					args->value--;
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						goto QUIT;
				}
				
				
				else if (!K_CTRL() && (K_UP() || K_DOWN() || K_RIGHT() || K_LEFT() || K_MENU() || K_TAB()
							|| K_SCRATCHPAD()))
					goto QUIT;
				
				else if (K_ESC()) {
					if (ood) goto QUIT;
					break;
				}
				
				else if (!K_CTRL()) {
					if (K_CLICK() || K_ENTER())
						ok = wExecCallback(w, SIGNAL_CLICK);
					else
						ok = wExecCallback(w, SIGNAL_KEY);
					if (ok != ACTION_CONTINUE) goto QUIT;
				}
				
			} while (K_CTRL() || ((!K_CLICK() || ood) && !K_ENTER()));
			if (ood) goto QUIT;
		}
		
		
		else {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				break;
		}
		
		wDrawWidget(w);
	} while (1);
	
  QUIT:
	return ok;
}





// Méthodes
int wPBar_GetValue(Widget *w)
{
	if (!w) return 0;
	if (w->type != WIDGET_PROGRESS_BAR && w->type != WIDGET_PROGRESS_BAR_V)
		return 0;
	ProgressBarArgs *args = w->args;
	return args->value;
}


void wPBar_SetValue(Widget *w, int value)
{
	if (!w) return;
	if (w->type != WIDGET_PROGRESS_BAR && w->type != WIDGET_PROGRESS_BAR_V)
		return;
	ProgressBarArgs *args = w->args;
	args->value = value;
}


void wPBar_SetColors(Widget *w, Uint32 color1, Uint32 color2)
{
	if (!w) return;
	if (w->type != WIDGET_PROGRESS_BAR && w->type != WIDGET_PROGRESS_BAR_V)
		return;
	ProgressBarArgs *args = w->args;
	args->color1 = color1;
	args->color2 = color2;
}


int wPBar_GetMax(Widget *w)
{
	if (!w) return 0;
	if (w->type != WIDGET_PROGRESS_BAR && w->type != WIDGET_PROGRESS_BAR_V)
		return 0;
	ProgressBarArgs *args = w->args;
	return args->max;
}


void wPBar_SetMax(Widget *w, int max)
{
	if (!w) return;
	if (w->type != WIDGET_PROGRESS_BAR && w->type != WIDGET_PROGRESS_BAR_V)
		return;
	ProgressBarArgs *args = w->args;
	args->max = max;
}



