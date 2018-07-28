#include "structures.h"
#include "CardinalLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"

// Création
Widget *wCardinalLayout(int displayBounds)
{
	int x;
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_CARDINAL_LAYOUT;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = displayBounds;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout = 1;
	w->callBack= NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 240;
	w->bounds.h = 200;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawCardinalLayout;
	w->activate	= ActivateCardinalLayout;
	w->close		= CloseCardinalLayout;
	w->add		= AddCardinalLayout;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(CardinalLayoutArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	CardinalLayoutArgs *args = w->args;
	args->nWidgets	= 5;
	args->cWidget = 0;
	args->widgets	= malloc(5 * sizeof(Widget *));
	
	for(x=0; x<5; x++) {
		args->widgets[x] = NULL;
		args->sideSize[x] = 0;
	}
	
	return w;
}


// Ajout de Widgets
int AddCardinalLayout(Widget *parent, Widget *child, int spot)
{
	if (!parent || !child || spot < 0 || spot > 4) return 0;
	CardinalLayoutArgs *args = parent->args;
	
	wAddWidgetToConstruct(parent->construct, args->widgets[spot]);
	args->widgets[spot] = child;
	return 1;
}




// Dessin
void DrawCardinalLayout(Widget *w)
{
	CardinalLayoutArgs *args = w->args;
	int x, nDynamics=0;
	Widget *child = NULL;
	int dy = 0;
	int dh = 0;
	int dx = 0;
	int dw = 0;
	int side[4];
	w->isDynamic = 0;

	
	
	// Calcul des coordonnées des tailles des côtés
	for (x=0; x<4; x++) {
		if (args->sideSize[x])
			side[x] = args->sideSize[x];
		else if (x < 2)
			side[x] = w->bounds.h/8;
		else
			side[x] = w->bounds.w/8;
		
		if (!args->sideSize[x] && side[x] < 20) side[x] = 20;
	}
	
	
	// Calcul des coordonnées de chaque widget par rapport à l'autre
	for (x=0; x < 5; x++) {
		child = args->widgets[x];
		if (!child) continue;

		
		if (x <= 1) { // UP & DOWN
			if (child->Hexpansive || child->bounds.w + 6 > w->bounds.w) 
				child->bounds.w = w->bounds.w - 6;
			if (child->Vexpansive) 
				child->bounds.h = side[x] - 3;
			else
				side[x] = child->bounds.h + 3;
			
			child->bounds.x = w->bounds.x + 3;
			child->bounds.y = w->bounds.y + 3;
			
			if (x) {
				dh += side[1] - 1;
				child->bounds.y += w->bounds.h - side[1] - 3;
			}
			else {
				dy = side[0] - 1;
				dh = side[0] - 1;
			}
		}
		
		else if (x < 4) { // LEFT & RIGHT			
			if (child->Hexpansive) 
				child->bounds.w = side[x] - 3;
			else 
				side[x] = child->bounds.w + 3;
			if (child->Vexpansive || child->bounds.h + dh + 6 > w->bounds.h)
				child->bounds.h = w->bounds.h - dh - 6;
			
			child->bounds.x = w->bounds.x + 3;
			child->bounds.y = w->bounds.y + dy + 3;
			
			if (x == 3) {
				dw += side[3] - 1;
				child->bounds.x += w->bounds.w - side[x] - 3;
			}
			else {
				dx = side[2] - 1;
				dw = side[2] - 1;
			}
		}
		
		else { // CENTER
			child->bounds.x = w->bounds.x + 3 + dx;
			if (child->Hexpansive || child->bounds.w + 6 > w->bounds.w - dw)
				child->bounds.w = w->bounds.w - dw - 6;
			else 
				child->bounds.x += ((w->bounds.w - dw) - child->bounds.w)/2 - 2;
				
			child->bounds.y = w->bounds.y + 3 + dy;
			if (child->Vexpansive || child->bounds.h + 6 > w->bounds.h - dh)
				child->bounds.h = w->bounds.h - dh - 6;
			else 
				child->bounds.y += ((w->bounds.h - dh) - child->bounds.h)/2 - 2;
		}
		
		if (!wIsActivable(child))
			continue;
		
		// dessin-- la taille des bounds peut encore être modifiée ici
		wDrawWidget(child);
		
		if (child->isDynamic && w->construct->onlyOneDynamic && nDynamics < 2) {
			nDynamics++;
			if (nDynamics == 2) w->construct->onlyOneDynamic = 0;
		}
		if (!w->isDynamic && child->isDynamic)
			w->isDynamic = 1;
	}
}



// Activation
int ActivateCardinalLayout(Widget *w)
{
	CardinalLayoutArgs *args = w->args;
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
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT()) {
			if (K_DOWN())			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
			else if (K_UP())		nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
			else if (K_RIGHT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
			else if (K_LEFT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
			
			if (nWidget == -1) break;
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
void CloseCardinalLayout(Widget *w)
{
	if (!w) return;
	CardinalLayoutArgs *args = w->args;
	
	if (args) {
		int x;
		if (args->widgets) {
			for (x=0; x < args->nWidgets; x++)
				wAddWidgetToConstruct(w->construct, args->widgets[x]);
			free(args->widgets);
		}
	}
}


