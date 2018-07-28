#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "Check.h"
#include "Button.h"
#include "widgets.h"


// Création
Widget *wCheck(const char *text, BOOL isChecked)
{
	return wExCheck(text, isChecked, 0);
}


Widget *wExCheck(const char *text, BOOL isChecked, int alignment)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_CHECK;
	w->displayBounds = 0;
	w->Hexpansive = 1;
	w->Vexpansive = 0;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 1;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;
	

	w->bounds.w = 16 + 7*strlen(text);
	w->bounds.h = 17;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw	= DrawCheck;
	w->activate	= ActivateCheck;
	w->close		= NULL;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(CheckArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	CheckArgs *args = w->args;
	args->text		= (char *) text;
	args->checked	= isChecked;
	args->alignment	= alignment;
	
	return w;
}



// Dessin
void DrawCheck(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	CheckArgs *args = w->args;
	int x = w->bounds.x + 3, y = w->bounds.y + 5;
	int height = nSDL_GetStringHeight(theme->font, args->text);
	SDL_Rect sq = {x, y-2, 11, 11};
	SDL_Surface *check = args->checked? theme->img_check_1 : theme->img_check_0;
	

	
	// on calcule l'alignement
	if 		(args->alignment%4)	// si le check est à droite du texte
		sq.x = w->bounds.x + w->bounds.w - 14;
	else											// sinon, s'il est à gauche
		x = w->bounds.x + 9;
	
	if 		(args->alignment/4 == 1)	// MID
		y = w->bounds.y + (w->bounds.h - height)/2;
	else if	(args->alignment/4 == 2)	// BOT
		y = w->bounds.y + w->bounds.h - 3 - height;
	
	// on dessine
	DrawClippedStr(scr, theme->font,
					args->alignment%4? x : x+10, y,
					args->text);
	DrawSurface(check, NULL, scr, &sq);
}




// Activation
int ActivateCheck(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	CheckArgs *args = w->args;
	SDL_Surface *check;
	SDL_Rect sq = {w->bounds.x + 3, w->bounds.y + 3, 11, 11};
	if (args->alignment%4)		// si le check est à droite
		sq.x = w->bounds.x + w->bounds.w - 14;
	int ok = ACTION_CONTINUE;
	
	SDL_Flip(scr);
	
	while (!K_ESC()) {
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			|| K_SCRATCHPAD() || K_ENTER() || K_ESC())
			return ACTION_CONTINUE;
			
		if (K_CLICK()) {
			args->checked = !args->checked;
			check = args->checked? theme->img_check_1 : theme->img_check_0;
			DrawSurface(check, NULL, scr, &sq);
			if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
				return ok;
			SDL_Flip(scr);
			while (K_CLICK());
		}
	}
	return ACTION_CONTINUE;
}



// Méthodes
BOOL wCheck_IsChecked(Widget *w)
{
	if (!w) return 0;
	if (w->type == WIDGET_CHECK) {
		CheckArgs *args = w->args;
		return args->checked;
	}
	
	if (w->type == WIDGET_BUTTON) {
		ButtonArgs *args = w->args;
		if (args->state > -1) return args->state;
	}
	
	return 0;
}


void wCheck_Set(Widget *w, BOOL b)
{
	if (!w) return;
	if (w->type == WIDGET_CHECK) {
		CheckArgs *args = w->args;
		args->checked = b;
	}
	
	else if (w->type == WIDGET_BUTTON) {
		ButtonArgs *args = w->args;
		if (args->state > -1) args->state = b;
	}
}


char *wCheck_GetText(Widget *w)
{
	if (!w) return NULL;
	if (w->type == WIDGET_CHECK) {
		CheckArgs *args = w->args;
		return args->text;
	}
	
	else if (w->type == WIDGET_BUTTON) {
		ButtonArgs *args = w->args;
		return args->text;
	}
	return NULL;
}

