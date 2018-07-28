#include "structures.h"
#include "BasicLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"



// Création
Widget *wBasicLayout(int displayBounds)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_BASIC_LAYOUT;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = displayBounds;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout  = 1;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 160;
	w->bounds.h = 190;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawBasicLayout;
	w->activate	= ActivateBasicLayout;
	w->close		= CloseBasicLayout;
	w->add		= AddBasicLayout;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(BasicLayoutArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	BasicLayoutArgs *args = w->args;
	args->nWidgets	= 0;
	args->widgets	= NULL;
	args->cWidget = 0;
	
	return w;
}


// Ajout de widgets-enfants
int AddBasicLayout(Widget *w, Widget *child, int spot)
{
	if (!w || !child) return 0;
	BasicLayoutArgs *args = w->args;
	
	
	// on trouve le spot
	if (spot != -1) {
		if (spot < 0) return 0;
		if (spot > args->nWidgets)
			spot = args->nWidgets;
	}
	else
		spot = args->nWidgets;
	
	
	// on enregistre le widget
	if (spot < args->nWidgets) {
		wAddWidgetToConstruct(w->construct, args->widgets[spot]);
		args->widgets[spot] = child;
	}
	
	else {
		// on réalloue la mémoire
		void *tmp = realloc(args->widgets, (args->nWidgets+1)*sizeof(Widget *));
		if (!tmp) return 0;
		args->widgets = tmp;
		
		args->widgets[args->nWidgets++] = child;
	}
	
	return 1;
}



// Dessin
void DrawBasicLayout(Widget *w)
{
	BasicLayoutArgs *args = w->args;
	w->isDynamic = 0;
	int x, nDynamics=0;
	Widget *child = NULL;
	SDL_Rect r = (SDL_Rect) {w->bounds.x + 3, w->bounds.y + 3, w->bounds.w - 6, w->bounds.h - 6};
	
	// Calcul des coordonnées de chaque widget par rapport à l'autre
	for (x=0; x < args->nWidgets; x++) {
		child = args->widgets[x];
		
		
		if (x) {
			Widget *lChild = args->widgets[x-1];
			r.y += lChild->bounds.h + 1;
			r.h -= lChild->bounds.h + 1;
			wFindBounds(child, &r, 1, 0);
		}
		else {
			wFindBounds(child, &r, 1, 0);
		}
		
		
		// Si le widget dépasse, on continue
		if (!wIsActivable(child))
			continue;
		
		
		// dessin-- la taille des bounds peut encore être modifiée ici
		wDrawWidget(child);
		
		if (child->isDynamic && w->construct->onlyOneDynamic && nDynamics < 2) {
			nDynamics++;
			if (nDynamics == 2) w->construct->onlyOneDynamic = 0;
		}
		
		w->isDynamic |= child->isDynamic;
	}
}



// Activation
int ActivateBasicLayout(Widget *w)
{
	BasicLayoutArgs *args = w->args;
	if (!args->nWidgets || !args->widgets) return 0;
	int ok;
	int nWidget = args->cWidget;
	
	if (K_DOWN())			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
	else if (K_UP())		nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
	else if (K_RIGHT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
	else if (K_LEFT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
	else if (K_TAB() || !args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])) {
		nWidget = 0;
		while (nWidget < args->nWidgets && (!args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])))
			nWidget++;
		if (nWidget == args->nWidgets) nWidget = -1;
	}
	
	
	do {
		if (nWidget > -1 && nWidget < args->nWidgets) args->cWidget = nWidget;
		ok = wActivateWidget(args->widgets[args->cWidget]);
		if (ok < 2) return ok;
		
		if (K_UP()) {		// on regarde qui est le prochain widget
			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
			
			if (nWidget == -1) break;  // pas de prochain widget dans ce layout, on va voir s'il y en a un au-dessus
			ok = 3;
		}
		
		else if (K_DOWN()) {
			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
			
			if (nWidget == -1) break;  // pas de prochain widget dans ce layout, on va voir s'il y en a un au-dessus
			ok = 3;
		}
		
		else if (K_TAB()) {
			nWidget = args->cWidget;
			do {
				++nWidget;
			} while (nWidget < args->nWidgets && (!args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])));
			if (nWidget == args->nWidgets) nWidget = -1;
			
			if (nWidget == -1) break;
			ok = 3;
		}
				
	} while (ok == 3);
	return 2;
}



// Fermeture
void CloseBasicLayout(Widget *w)
{
	if (!w) return;
	BasicLayoutArgs *args = w->args;
	
	if (args) {
		int x;
		if (args->widgets) {
			for (x=0; x < args->nWidgets; x++)
				wAddWidgetToConstruct(w->construct, args->widgets[x]);
			free(args->widgets);
		}
	}
}


