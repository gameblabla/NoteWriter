#include <os.h>
#include <SDL/SDL.h>
#include "structures.h"
#include "keys.h"
#include "general.h"
#include "widgets.h"
#include "Map.h"
#include "scrollbar.h"
#include "Button.h"


// Création
Widget *wMap(int dCols, int nCols, int dRows, int nRows, int minSpace, int mode)
{
	Widget *w = malloc(sizeof(Widget));
	w->type = WIDGET_MAP;
	w->displayBounds = 0;
	w->Hexpansive = 0;
	w->Vexpansive = 0;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 0;
	w->bounds.h = 0;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->draw	= DrawMap;
	w->activate	= ActivateMap;
	w->close		= CloseMap;
	w->add = NULL;
	
	w->args = malloc(sizeof(MapArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	MapArgs *args = w->args;
	args->dCols		= dCols<1? 1:dCols;
	args->nCols		= nCols<1? 1:nCols;
	args->dRows		= dRows<1? 1:dRows;
	args->nRows		= nRows<1? 1:nRows;
	if (args->nCols < args->dCols) args->nCols = args->dCols;
	if (args->nRows < args->dRows) args->nRows = args->dRows;
	args->minSpace	= minSpace;
	args->nElements= 0;
	args->elements = NULL;
	args->mode 		= mode;
	
	args->w	= 0;
	args->h	= 0;
	args->x	= 0;
	args->cx	= 0;
	args->y	= 0;
	args->cy	= 0;
	args->state = 0;
	return w;
}

void wMap_AddElement(Widget *w, SDL_Surface *pic0, SDL_Surface *pic1)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	if (!pic0) return;
	if (pic0->w > 300 || pic0->h > 220) return;	// par précaution
	if (pic1 && (pic1->w > 300 || pic1->h > 220)) return;
	MapArgs *args = w->args;
	if (args->nElements >= args->nCols * args->nRows) return;	// on a atteint le max
	BOOL vsb = ((args->nElements-1)/args->nCols > args->dRows-1);
	BOOL hsb = (args->nElements > args->dCols && args->dCols < args->nCols);
	MapElement *el;
	void *tmp = realloc(args->elements, sizeof(MapElement)*(args->nElements+1));
	if (!tmp) return;
	
	args->elements = tmp;
	el = &args->elements[args->nElements++];
	el->pic0	= pic0;
	el->pic1	= pic1;
	el->isSelected = 0;
	
	// on recalcule les coordonnées du widget
	if (pic0->w > args->w) args->w = pic0->w;
	if (pic0->h > args->h) args->h = pic0->h;
	if (pic1) {
		if (pic1->w > args->w) args->w = pic1->w;
		if (pic1->h > args->h) args->h = pic1->h;
	}
	
	w->isDynamic = 1;
	w->bounds.w = 10 + args->dCols*(args->w + args->minSpace) - args->minSpace;
	w->bounds.h = 10 + args->dRows*(args->h + args->minSpace) - args->minSpace;
	if (hsb) w->bounds.h += 9;	// scrollbars
	if (vsb) w->bounds.w += 9;	// scrollbars
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
}


// Dessin
void wMap_DrawElement(Widget *w, int elt)
{
	MapArgs *args = w->args;
	if (elt > args->nElements-1) return;
	int xp = elt % args->nCols;
	int yp = elt / args->nCols;
	if (xp < args->x || xp >= args->x + args->dCols) return;
	if (yp < args->y || yp >= args->y + args->dRows) return;
	xp -= args->x;
	yp -= args->y;
	MapElement *el = &args->elements[elt];
	if (!el->pic0) return;	// c'est un élément nul
	
	SDL_Surface *scr = w->construct->scr;
	
	SDL_Surface *pic = el->pic0;
	if (el->isSelected && el->pic1) pic = el->pic1;
	
	int x = w->bounds.x + 5 + xp*(args->w + args->minSpace);
	int y = w->bounds.y + 5 + yp*(args->h + args->minSpace);
	SDL_Rect area = {x + (args->w - pic->w)/2, y + (args->h - pic->h)/2, pic->w, pic->h};
	
	DrawSurface(pic, NULL, scr, &area);
	
	if (el->isSelected && !el->pic1) {
		SDL_Surface *sf = SDL_CreateRGBSurface(SDL_SWSURFACE, pic->w, pic->h, 16, 0, 0, 0, 0);
		DrawFillRect(sf, &area, 0);
		SDL_SetAlpha(sf, SDL_SRCALPHA, 128);
		DrawSurface(sf, NULL, scr, &area);
		SDL_FreeSurface(sf);
	}
}


void DrawMap(Widget *w)
{
	MapArgs *args = w->args;
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	int x;
	int nCols = min(args->nCols, args->nElements);
	int nRows = min(args->nRows, (args->nElements-1)/args->nCols + 1);
	BOOL vsb = ((args->nElements-1)/args->nCols > args->dRows-1);
	BOOL hsb = (args->nElements > args->dCols && args->dCols < args->nCols);
	int ms = args->minSpace;
	Uint32 c = t->color1;
	
	DrawFillRect(scr, &w->bounds, c);
	
	// Dessin des images
	for (x=0; x < args->nElements; x++)
		wMap_DrawElement(w, x);
	
	// Dessin des scrollbars
	if (vsb) {
		int ypos = args->y*(args->h + args->minSpace);
		if (hsb) w->bounds.h -= 9;
		DrawVScrollBar(scr, t, &w->bounds, 10-ms+nRows*(args->h+ms), ypos);
		if (hsb) w->bounds.h += 9;
	}
	if (hsb) {
		int xpos = args->x*(args->w + args->minSpace);
		if (vsb) w->bounds.w -= 9;
		DrawHScrollBar(scr, t, &w->bounds, 10-ms+nCols*(args->w+ms), xpos);
		if (vsb) w->bounds.w += 9;
	}
	
	// Dessin des bordures
	if (args->state) {
		DrawRect(scr, &w->bounds, Contrasted(c, 60));
		DrawRectXY(scr, w->bounds.x+1, w->bounds.y+1, w->bounds.w-2, w->bounds.h-2,
				  Contrasted(c, 50));
	}
	else {
		DrawRect(scr, &w->bounds, Contrasted(c, 50));
	}
}



// Activation
int ActivateMap(Widget *w)
{
	MapArgs *args = w->args;
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	int elt;
	int ok = ACTION_CONTINUE;
	int xhalo, yhalo;
	MapElement *el;
	SDL_Surface *pic;
	int dRows = args->dRows;
	int dCols = args->dCols;
	int nCols = min(args->nCols, args->nElements);
	int nRows = min(args->nRows, (args->nElements-1)/args->nCols + 1);
	BOOL vsb = ((args->nElements-1)/args->nCols > args->dRows);
	BOOL hsb = (args->nElements > args->dCols && args->dCols < args->nCols);
	SDL_Rect saveR = {w->bounds.x, w->bounds.y, w->bounds.w - (vsb? 9:0), w->bounds.h - (hsb? 9:0)};
	SDL_Surface *save = SDL_CreateRGBSurface(SDL_SWSURFACE, saveR.w, saveR.h, 16, 0, 0, 0, 0);
	Uint32 c = t->color1;
	BOOL ood = w->construct->onlyOneDynamic;
	
	DrawSurface(scr, &saveR, save, NULL);
	DrawRect(scr, &w->bounds, Contrasted(c, 60));
	DrawRectXY(scr, w->bounds.x+1, w->bounds.y+1, w->bounds.w-2, w->bounds.h-2,
				  Contrasted(c, 50));
	if (ood) goto INLOOP;
	SDL_Flip(scr);
	
	
	while (!K_ESC()) {
		wait_key_pressed();
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB() || K_SCRATCHPAD() || K_ENTER() || K_ESC()) break;

		if (K_CLICK()) {
		  INLOOP:
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				break;
			args->state = 1;
			
			do {
				DrawMap(w);
				DrawSurface(scr, &saveR, save, NULL);
				
			  CURSOR:
				elt = args->x + args->cx + (args->y + args->cy)*args->nCols;
				el = &args->elements[elt];
				pic = el->pic0;
				if (el->isSelected && el->pic1) pic = el->pic1;
				
				// on dessine le 'curseur'
				xhalo = w->bounds.x + 5 + args->cx*(args->w + args->minSpace)-3+(args->w-pic->w)/2;
				yhalo = w->bounds.y + 5 + args->cy*(args->h + args->minSpace)-3+(args->h-pic->h)/2;
				GradientHalo(scr, &(SDL_Rect){xhalo, yhalo, pic->w+6,pic->h+6}, 3, t->map_c1, 0);
				
// Debug
// DrawFillRect(scr, &((SDL_Rect) {0, 0, 319, 11}), SDL_MapRGB(scr->format, 255, 255, 255));
// DrawClippedStr(scr, t->font, 0, 0, "x=%i(%i), y=%i(%i)", args->x, args->cx, args->y, args->cy);
				
			  ACTIVATION:
				if (!K_CLICK()) {
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						break;
				}
				SDL_Flip(scr);
				wait_key_pressed();
				
				if (K_MENU() || K_TAB() || K_DOC() || K_SCRATCHPAD() || K_ENTER() || K_ESC()) {
					if (ood) goto QUIT;
					break;
				}
				
				if (K_CLICK()) {
					if (!el->pic0) goto ACTIVATION;
					
					if (args->mode == BUTTON_PRESS) {
						el->isSelected = 1;
						wMap_DrawElement(w, elt);
						SDL_Flip(scr);
						while (K_CLICK());
						el->isSelected = 0;
						if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
							break;
						wMap_DrawElement(w, elt);
					}
					
					else {
						el->isSelected = !el->isSelected;
						if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
							break;
						DrawSurface(save, NULL, scr, &saveR);
						wMap_DrawElement(w, elt);
						DrawSurface(scr, &saveR, save, NULL);
						goto CURSOR;
					}
					goto ACTIVATION;
				}
				
				else if (K_UP()) {
					if (!args->y && !args->cy) {
						args->x = 0;
						args->cx = 0;
					}
					else {
						if (args->cy) {
							args->cy--;
							DrawSurface(save, NULL, scr, &saveR);
							goto CURSOR;
						}
						else args->y--;
					}
				}
				
				else if (K_DOWN()) {
					if (elt == args->nElements-1) goto ACTIVATION;
					if (args->y + args->cy >= nRows-1) {
						args->cx = (args->nElements-1) % nCols;
						args->x = 0;
						if (args->cx > dCols-1) {
							args->x = args->cx + 1 - dCols;
							args->cx = dCols-1;
						}
					}
					else {
						if (args->cy < dRows-1) {
							args->cy++;
							if (args->x+args->cx + nCols*(args->y+args->cy) >= args->nElements) {
								args->cx = args->nElements-1-args->x-nCols*(args->y+args->cy);
								if (args->cx < 0) {
									args->x += args->cx;
									args->cx = 0;
									continue;
								}
							}
							DrawSurface(save, NULL, scr, &saveR);
							goto CURSOR;
						}
						else  {
							args->y++;
							if (args->x+args->cx + nCols*(args->y+args->cy) >= args->nElements) {
								args->cx = args->nElements-1-args->x-nCols*(args->y+args->cy);
								if (args->cx < 0) {
									args->x += args->cx;
									args->cx = 0;
								}
							}
						}
					}
				}
				
				else if (K_LEFT()) {
					if (!args->x && !args->cx) {
						if (!args->y && !args->cy) goto ACTIVATION;
						if (args->cy) args->cy--;
						else args->y--;
						args->x = args->nCols - dCols;
						args->cx = dCols - 1;
					}
					else {
						if (args->cx) {
							args->cx--;
							DrawSurface(save, NULL, scr, &saveR);
							goto CURSOR;
						}
						else args->x--;
					}
				}
				
				else if (K_RIGHT()) {
					if (elt == args->nElements-1) goto ACTIVATION;
					
					if (args->x + args->cx >= args->nCols-1) {
						args->x = 0;
						args->cx = 0;
						if (args->cy < dRows-1) args->cy++;
						else args->y++;
					}
					else {
						if (args->cx < dCols-1) {
							args->cx++;
							DrawSurface(save, NULL, scr, &saveR);
							goto CURSOR;
						}
						else args->x++;
					}
				}
				
				else {
					if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
						break;
				}
				
			} while (!K_ESC());
			DrawSurface(save, NULL, scr, &saveR);
			SDL_Flip(scr);
			while(K_ESC());
		}
		
	}
	
  QUIT:
	args->state = 0;
	SDL_FreeSurface(save);
	return ok;
}





// Fermeture
void CloseMap(Widget *w)
{
	if (!w) return;
	if (w->args) {
		MapArgs *args = w->args;
		if (args->elements) {
			free(args->elements);
			int x;
			
			if (wIsFreedArg(w, WSURFACE)) {
				for (x=0; x < args->nElements; x++) {
					wAddSurfaceToConstruct(w->construct, args->elements[x].pic0);
					wAddSurfaceToConstruct(w->construct, args->elements[x].pic1);
				}
			}
		}
	}
}



// Autres méthodes
BOOL wMap_IsElementSelected(Widget *w, int elt)
{
	if (!w) return 0;
	if (w->type != WIDGET_MAP) return 0;
	MapArgs *args = w->args;
	if (!args->mode) return 0;
	if (elt < 0 || elt >= args->nElements) return 0;
	
	return args->elements[elt].isSelected;
}


void wMap_SelectElement(Widget *w, int elt)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	MapArgs *args = w->args;
	if (!args->mode) return;
	if (elt < 0 || elt >= args->nElements) return;
	
	args->elements[elt].isSelected = 1;
}

void wMap_UnselectElement(Widget *w, int elt)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	MapArgs *args = w->args;
	if (!args->mode) return;
	if (elt < 0 || elt >= args->nElements) return;
	
	args->elements[elt].isSelected = 0;
}

void wMap_SelectAll(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	MapArgs *args = w->args;
	if (!args->mode) return;
	int x;
	
	for (x=0; x < args->nElements; x++) {
		args->elements[x].isSelected = 1;
	}
}


void wMap_UnselectAll(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	MapArgs *args = w->args;
	if (!args->mode) return;
	int x;
	
	for (x=0; x < args->nElements; x++) {
		args->elements[x].isSelected = 0;
	}
}


void wMap_SetElementImage(Widget *w, int elt, SDL_Surface *pic0, SDL_Surface *pic1)
{
	if (!w) return;
	if (w->type != WIDGET_MAP) return;
	if (!pic0) return;
	MapArgs *args = w->args;
	if (elt < 0 || elt >= args->nElements) return;
	
	args->elements[elt].pic0 = pic0;
	args->elements[elt].pic1 = pic1;
}


MapElement *wMap_GetElementStruct(Widget *w, int elt)
{
	if (!w) return NULL;
	if (w->type != WIDGET_MAP) return NULL;
	MapArgs *args = w->args;
	if (elt < 0 || elt >= args->nElements) return NULL;
	
	return &args->elements[elt];
}





