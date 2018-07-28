#include "structures.h"
#include "BasicLayout.h"
#include "ScrolledLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"
#include "scrollbar.h"


Widget *wScrolledLayout()
{
	Widget *w = wBasicLayout(1);
	if (!w) return NULL;
	
	w->type = WIDGET_SCROLLED_LAYOUT;
	w->draw = DrawScrolledLayout;
	w->activate = ActivateScrolledLayout;
	
	ScrolledLayoutArgs *args = w->args;
	args->cWidget = 0;
	args->y = 0;
	
	return w;
}




// Dessin
static BOOL isFullyDrawn(Widget *child)
{
	Widget *w = child->parent;
	if (!w) return 0;
	
	if (child->bounds.y < w->bounds.y+3) return 0;
	if (child->bounds.y + child->bounds.h+3 > w->bounds.y + w->bounds.h) return 0;
	return 1;
}

static void SetY(Widget *w, int y)
{
	int x;
	ScrolledLayoutArgs *args = w->args;
	for (x=0; x<args->nWidgets; x++)
		args->widgets[x]->bounds.y -= y - args->y;
	args->y = y;
}


void DrawScrolledLayout(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	ScrolledLayoutArgs *args = w->args;
	int x, nDynamics=0;
	Widget *child = NULL;
	SDL_Rect r = (SDL_Rect) {w->bounds.x + 3, w->bounds.y + 3, w->bounds.w - 6, w->bounds.h - 6};
	int hsize = 0;
	int sVexp;
	SDL_Rect clip;
	SDL_GetClipRect(scr, &clip);
	SDL_SetClipRect(scr, &(SDL_Rect) {w->bounds.x+2, w->bounds.y+2, w->bounds.w-4, w->bounds.h-4});
	int sy = args->y;
	args->y = 0;
	BOOL hasSB;
	
	// on calcule la hauteur totale des widgets pour savoir s'il y a besoin de scrollbar ou non
	for (x=0; x < args->nWidgets; x++) {
		child = args->widgets[x];
		if (x) hsize += 1;
		if (child->bounds.h+6 > w->bounds.h)
			child->bounds.h = w->bounds.h - 6;
		hsize += child->bounds.h;
	}
	hasSB = (hsize+6 > w->bounds.h);
	if (hasSB) r.w -= 9;
	w->isDynamic = hasSB;
	
	// Calcul des coordonnées de chaque widget par rapport à l'autre
	for (x=0; x < args->nWidgets; x++) {
		child = args->widgets[x];
		
		sVexp = child->Vexpansive;
		child->Vexpansive = 0;
		
		if (x) {
			Widget *lChild = args->widgets[x-1];
			r.y += lChild->bounds.h + 1;
			r.h -= lChild->bounds.h + 1;
			wFindBounds(child, &r, 1, 0);
		}
		else {
			wFindBounds(child, &r, 1, 0);
		}
		
		child->Vexpansive = sVexp;
	}
	
	
	SetY(w, sy);
	
	// On vérifie que le widget sélectionné est bien affiché
	if (!isFullyDrawn(args->widgets[args->cWidget])) {
		SetY(w, 0);
		if (!isFullyDrawn(args->widgets[args->cWidget])) {
			// on le fait apparaître tout au bout du ScrolledLayout
			Widget *wd = args->widgets[args->cWidget];
			SetY(w, wd->bounds.y + wd->bounds.h + 3 - w->bounds.y - w->bounds.h);
		}
	}
	
	// on vérifie qu'il n'y a pas d'espace vide
	if (args->y) {
		Widget *last = args->widgets[args->nWidgets-1];
		if (last->bounds.y + last->bounds.h + 3 < w->bounds.y + w->bounds.h) {
			SetY(w, 0);
			SetY(w, last->bounds.y + last->bounds.h + 3 - w->bounds.y - w->bounds.h);
		}
	}
	
	
	// on dessine les enfants
	for (x=0; x < args->nWidgets; x++) {
		child = args->widgets[x];
		if (child->bounds.y + child->bounds.h < w->bounds.y+3) continue;
		if (child->bounds.y > w->bounds.y-4 + w->bounds.h) continue;
		
		wDrawWidget(child);
		
		if (child->isDynamic && w->construct->onlyOneDynamic && nDynamics < 2) {
			nDynamics++;
			if (nDynamics == 2) w->construct->onlyOneDynamic = 0;
		}
		if (!w->isDynamic && child->isDynamic && isFullyDrawn(child))
			w->isDynamic = 1;
	}
	
	
	// Dessin des Scrollbars
	if (hasSB)
		DrawVScrollBar(scr, w->construct->theme, &w->bounds, hsize+6, args->y);
	
	SDL_SetClipRect(scr, &clip);
}




// Activation
int ActivateScrolledLayout(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	ScrolledLayoutArgs *args = w->args;
	if (!args->nWidgets || !args->widgets) return 0;
	int ok = 3;
	int nWidget = args->cWidget;
	int n = 0;
	BOOL dynamic = 0;
	
	
	if (K_DOWN() || K_TAB())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
	else if (K_UP())		nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
	else if (K_RIGHT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
	else if (K_LEFT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
	
	if ((nWidget == -1 || nWidget == args->cWidget) && args->cWidget >= 0 && !args->widgets[args->cWidget]->isDynamic) {
		args->cWidget =- 1, nWidget = -1;
		while (!isFullyDrawn(args->widgets[n])) n++;
		while (isFullyDrawn(args->widgets[n])) {
			if (args->widgets[n]->isDynamic) {
				nWidget = n;
				break;
			}
			n++;
			if (n == args->nWidgets) break;
		}
	}
	// w->construct->previousWidgetBounds.y += args->y;
	
	

	if (nWidget > -1 && nWidget < args->nWidgets) args->cWidget = nWidget;
	if (args->cWidget == -1) SDL_Flip(scr);
	
	
	// Début de la boucle
	while (ok == 3) {
		dynamic = 0;
		if (args->cWidget >= 0) {
			ok = wActivateWidget(args->widgets[args->cWidget]);
			if (ok < 2) return ok;
		}
		else {
			ok = 2;
			wait_key_pressed();
			if (K_ENTER()) return 1;
			else if (K_ESC()) return 0;
			dynamic = 1;
		}
		
		
		if (K_UP()) {		// on regarde qui est le prochain widget
			
			// S'il n'y a aucun widget actif visible
			if (args->cWidget == -1) {
			  UP:
				n=0;
				while (!isFullyDrawn(args->widgets[n])) n++;
				if (!n) continue;
				
				int y = args->widgets[n-1]->bounds.y + args->y - w->bounds.y - 3;
				ok = 3;
				
				w->construct->previousWidgetBounds.y -= y - args->y;
				SetY(w, y);
				wDrawWidget(w);
				
				if (args->widgets[n-1]->isDynamic) args->cWidget = n-1;
				else {
					if (args->cWidget >= 0) {
						if(!isFullyDrawn(args->widgets[args->cWidget])) {
							args->cWidget = -1;
							SDL_Flip(scr);
						}
					}
					else
						SDL_Flip(scr);
				}
			}
			
			else {
				if (!args->cWidget) continue;
				
				// s'il y a un widget dynamique au dessus pleinement visible on le sélectionne
				n = args->cWidget-1;
				while (isFullyDrawn(args->widgets[n])) {
					if (args->widgets[n]->isDynamic) {
						args->cWidget = n;
						ok = 3;
						break;
					}
					if (!n--) break;
				}
				
				// sinon on affiche le prochain
				if (args->cWidget != n) {
					if (n == -1) continue;
					goto UP;
				}
			}
		}
		
		else if (K_DOWN() || K_TAB()) {
			Widget *wx;
			
			// S'il n'y a aucun widget actif visible
			if (args->cWidget == -1) {
			  DOWN:
				n = args->nWidgets-1;
				while (!isFullyDrawn(args->widgets[n])) n--;
				
				if (n == args->nWidgets-1) continue;
				wx = args->widgets[n+1];
				int y = wx->bounds.y + wx->bounds.h + args->y - w->bounds.y - w->bounds.h + 3;
				ok = 3;
				
				w->construct->previousWidgetBounds.y -= y - args->y;
				SetY(w, y);
				wDrawWidget(w);
				
				if (wx->isDynamic) args->cWidget = n+1;
				else {
					if (args->cWidget >= 0) {
						if(!isFullyDrawn(args->widgets[args->cWidget])) {
							args->cWidget = -1;
							SDL_Flip(scr);
						}
					}
					else
						SDL_Flip(scr);
				}
			}
			
			else {
				if (args->cWidget == args->nWidgets-1) continue;
				
				// s'il y a un widget dynamique au dessous pleinement visible on le sélectionne
				n = args->cWidget+1;
				
				while (isFullyDrawn(args->widgets[n])) {
					if (args->widgets[n]->isDynamic) {
						args->cWidget = n;
						ok = 3;
						break;
					}
					if ((++n) == args->nWidgets) break;
				}
				
				// sinon on affiche le prochain
				if (args->cWidget != n) {
					if (n == args->nWidgets) continue;
					goto DOWN;
				}
			}
		}
	}
	
	if (dynamic) w->construct->previousWidgetBounds = w->bounds;
	
	return 2;
}




