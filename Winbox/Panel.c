#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "common.h"
#include "theme.h"
#include "Panel.h"
#include "widgets.h"


// --- Création ---
Widget *wPanel(BOOL displayBounds)
{
	Widget *w = malloc(sizeof(Widget));
	w->type = WIDGET_PANEL;
	w->displayBounds = displayBounds;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isLayout = 0;
	w->isDynamic = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;

	w->bounds.w = 160;
	w->bounds.h = 190;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->draw	= NULL;
	w->activate	= NULL;
	w->close		= NULL;
	w->add = NULL;
	
	w->args = NULL;
		
	return w;
}



// Méthodes
SDL_Rect wPanel_GetArea(Widget *w)
{
	if (!w) return (SDL_Rect) {0,0,0,0};
	if (w->type != WIDGET_PANEL) return (SDL_Rect) {0,0,0,0};
	SDL_Rect r = wGetPosition(w);
	if (w->displayBounds) r.x+=2, r.y+=2, r.w-=4, r.h-=4;
	return r;
}

void wPanel_Connect(Widget *w, void (*draw) (Widget *))
{
	if (!w) return;
	if (w->type != WIDGET_PANEL) return;
	w->draw = draw;
}

