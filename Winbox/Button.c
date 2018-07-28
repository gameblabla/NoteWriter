#include <os.h>
#include <SDL/SDL.h>
#include "structures.h"
#include "keys.h"
#include "general.h"
#include "widgets.h"
#include "Button.h"


// Création
Widget *wButton(const char *text, int mode)
{
	Widget *w = malloc(sizeof(Widget));
	w->type = WIDGET_BUTTON;
	w->displayBounds = 0;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 1;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;

	w->bounds.w = 8 + 8*strlen(text);
	w->bounds.h = 17;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw	= DrawButton;
	w->activate	= ActivateButton;
	w->close		= CloseButton;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(ButtonArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	ButtonArgs *args = w->args;
	args->text				= (char *) text;
	args->state				= mode-1;
	args->font				= NULL;
	args->font_selected	= NULL;
	args->bg					= NULL;
	args->bg_selected		= NULL;
	
	return w;
}

Widget *wButtonImage(SDL_Surface *img, SDL_Surface *img_selected, int mode)
{
	Widget *w = wButton(NULL, mode);
	if (!img || !img_selected) return w;
	ButtonArgs *args = w->args;
	w->Hexpansive = 0;
	w->Vexpansive = 0;
	
	w->bounds.w = 6 + img->w;
	w->bounds.h = 6 + img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	args->bg = wBackgroundImage(img, BACKG_IMG);
	args->bg_selected = wBackgroundImage(img_selected, BACKG_IMG);
	
	return w;
}



// Dessin
void DrawButton(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ButtonArgs *args = w->args;
	SDL_Rect rect = w->bounds, inRect = w->bounds;
	rect.x   += 2, rect.y   +=2, rect.w   -= 4, rect.h   -= 4;
	inRect.x += 3, inRect.y +=3, inRect.w -= 6, inRect.h -= 6;
	int x;
	int dy;
	wBACKGROUND *bg;
	nSDL_Font *f;
	
	
	// Dessin du Background
	DrawRect(scr, &rect, t->color4);
	if (args->state < 1)	// affichage normal
		bg = args->bg? args->bg : t->button_bg;
	else						// affichage pressé
		bg = args->bg_selected? args->bg_selected : t->button_bg_selected;
	wBG_Draw(bg, &inRect);
		
	
	// Dessin de la chaîne
	if (args->text) {
		if (args->state < 1)	// affichage normal
			f = args->font? args->font : t->button_font;
		else						// affichage pressé
			f = args->font_selected? args->font_selected : t->button_font_selected;
		
		x = rect.x + (rect.w - nSDL_GetStringWidth(f, args->text))/2;
		dy = inRect.y + 1 + (inRect.h - nSDL_GetStringHeight(f, args->text))/2;
		DrawClippedStr(scr, f, x, dy, args->text);
	}
}

// Activation
int  ActivateButton(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	ButtonArgs *args = w->args;	
	SDL_Flip(scr);
	int ok = ACTION_CONTINUE;
		
	while (!K_ESC()) {
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			 || K_SCRATCHPAD() || K_ENTER() || K_ESC())
			return ACTION_CONTINUE;
			
		else if (K_CLICK()) {
			
			if (args->state < 0) {
				args->state = 2;
				DrawButton(w);
				SDL_Flip(scr);
				while (K_CLICK());
				args->state = -1;
				if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
					return ok;
				DrawButton(w);
				SDL_Flip(scr);
			}
			
			else {
				args->state = !args->state;
				DrawButton(w);
				if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
					return ok;
				SDL_Flip(scr);
				while (K_CLICK());
			}
		}
		
		else if (any_key_pressed()) {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				return ok;
			SDL_Flip(scr);
		}
	}
	return ACTION_CONTINUE;
}


// Fermeture
void CloseButton(Widget *w)
{
	if (!w) return;
	if (w->args) {
		ButtonArgs *args = w->args;
		
		if (args->bg && wIsFreedArg(w, WBACKG)) {
			wAddEltToConstruct(w->construct, args->bg);
			wAddSurfaceToConstruct(w->construct, args->bg->image);
		}
		if (args->bg_selected && wIsFreedArg(w, WBACKG)) {
			wAddEltToConstruct(w->construct, args->bg_selected);
			wAddSurfaceToConstruct(w->construct, args->bg_selected->image);
		}
		if (wIsFreedArg(w, WFONT)) {
			wAddFontToConstruct(w->construct, args->font);
			wAddFontToConstruct(w->construct, args->font_selected);
		}
	}
}



// Méhodes
void wButton_SetText(Widget *w, const char *text)
{
	if (!w) return;
	if (w->type != WIDGET_BUTTON) return;
	ButtonArgs *args = w->args;
	args->text = (char *) text;
}

void wButton_SetMode(Widget *w, int mode)
{
	if (!w) return;
	if (w->type != WIDGET_BUTTON) return;
	ButtonArgs *args = w->args;
	args->state = mode;
}

int wButton_GetMode(Widget *w)
{
	if (!w) return 0;
	if (w->type != WIDGET_BUTTON) return 0;
	ButtonArgs *args = w->args;
	return args->state;
}

void wButton_SetFont(Widget *w, nSDL_Font *font, nSDL_Font *font_selected)
{
	if (!w) return;
	if (w->type != WIDGET_BUTTON) return;
	ButtonArgs *args = w->args;
	args->font = font;
	args->font_selected = font_selected;
}

void wButton_SetBackground(Widget *w, wBACKGROUND *bg, wBACKGROUND *bg_selected)
{
	if (!w) return;
	if (w->type != WIDGET_BUTTON) return;
	ButtonArgs *args = w->args;
	args->bg = bg;
	args->bg_selected = bg_selected;
}

