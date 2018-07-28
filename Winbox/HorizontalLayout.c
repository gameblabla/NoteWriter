#include "structures.h"
#include "BasicLayout.h"
#include "HorizontalLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"



// Création
Widget *wHorizontalLayout(int displayBounds)
{
	Widget *w = wBasicLayout(displayBounds);
	w->type = WIDGET_HORIZONTAL_LAYOUT;
	
	w->draw		= DrawHorizontalLayout;
	w->activate	= ActivateHorizontalLayout;
	return w;
}


// Dessin
void DrawHorizontalLayout(Widget *w)
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
			r.x += lChild->bounds.w + 1;
			r.w -= lChild->bounds.w + 1;
			wFindBounds(child, &r, 0, 1);
		}
		else {
			wFindBounds(child, &r, 0, 1);
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
int ActivateHorizontalLayout(Widget *w)
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
		
		if (K_LEFT()) {		// on regarde qui est le prochain widget
			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
			
			if (nWidget == -1) break;  // pas de prochain widget dans ce layout, on va voir s'il y en a un au-dessus
			ok = 3;
		}
		
		else if (K_RIGHT()) {
			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
			
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
