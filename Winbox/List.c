#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "List.h"
#include "menus.h"
#include "general.h"
#include "keys.h"
#include "scrollbar.h"
#include "widgets.h"

#include "dialogs.h"

#define MENU_QUIT 99

// Création
Widget *wList(wMENU *m)
{
	return wExList(m, 0, 1);
}

Widget *wExList(wMENU *m, int dItems, int maxSelected)
{
	if (!m) return NULL;
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_LIST;
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

	w->bounds.w = 80;
	w->bounds.h = 20;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->draw	= DrawList;
	w->activate	= ActivateList;
	w->close		= CloseList;
	w->add = NULL;
	
	w->args = malloc(sizeof(ListArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	ListArgs *args		= w->args;
	args->menu			= m;
	args->maxSelected	= maxSelected;
	args->dItems		= dItems;
	args->yItem			= 0;
	args->cItem			= 0;
	args->nSelected	= 0;
	args->isActive		= 0;
	args->cMenu			= m;
	
	return w;
}



// Dessin
void DrawList(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ListArgs *args = w->args;
	wMENU *m = args->menu;
	Uint32 c = Darker(t->color1, 60);
	Uint32 c2 = args->isActive? t->request_c1:t->request_c2;
	if (!c2) c2 = t->color1;
	char *s;

	int ws = nSDL_GetStringWidth(t->font, m->title);
	int xw = (w->bounds.w*3)/5;
	if (!m->title)
		xw = w->bounds.w - 6;
	else if (xw + ws + 9 > w->bounds.w)
		xw = w->bounds.w - ws - 9;
	int x = w->bounds.x + w->bounds.w - xw - 3;
	
	if (m->title)
		DrawClippedStr(scr, t->font, w->bounds.x+3, w->bounds.y+6, m->title);
	DrawFillRectXY(scr, x, w->bounds.y + 3, xw, 14, c2);
	DrawRectXY(scr, x, w->bounds.y + 3, xw, 14, c);
	
	if (!args->isActive) SDL_SetAlpha(t->menus_right, SDL_SRCALPHA, 128);
	DrawSurface(t->menus_right, NULL, scr, &(SDL_Rect) {x+xw-8, w->bounds.y+6, 5, 7});
	if (!args->isActive) SDL_SetAlpha(t->menus_right, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	
	if (args->nSelected) {
		s = wMenu_GetSelectedStr(m);
		SDL_Rect clip;
		SDL_GetClipRect(scr, &clip);
		SDL_SetClipRect(scr, &(SDL_Rect) {x+2, w->bounds.y, xw-11, w->bounds.h});
		
		if (args->nSelected>1) {
			char str[strlen(s)+5];
			sprintf(str, "%s,...", s);
			DrawClippedStr(scr, t->menus_font1, x+2, w->bounds.y+6, str);
		}
		else if (args->nSelected==1) DrawClippedStr(scr, t->menus_font1, x+2, w->bounds.y+6, s);
		
		SDL_SetClipRect(scr, &clip);
	}
	else {
		DrawClippedStr(scr, t->menus_font1, x+2 + (xw-4)/2, w->bounds.y+6, "-");
	}
}


// Activation
int  ActivateList(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ListArgs *args = w->args;
	args->isActive = 1;
	wMENU *m = args->menu;
	int ok = ACTION_CONTINUE;
	int db = w->displayBounds;
	
	
	do {
		DrawList(w);
		SDL_Flip(scr);
		wait_key_pressed();
		
		if (K_CLICK() && args->menu->nItems) {
			int ws = nSDL_GetStringWidth(t->font, m->title);
			int xw = (w->bounds.w*3)/5;
			if (!m->title)
				xw = w->bounds.w - 6;
			else if (xw + ws + 9 > w->bounds.w)
				xw = w->bounds.w - ws - 9;
			int x = w->bounds.x + w->bounds.w - xw - 3;
			w->displayBounds = -1;
			
			ok=OpenMenu(w, m, x+1, 0, xw, w->bounds.y+4);
			w->displayBounds = db;
			wDrawBounds(w);
			if (ok == MENU_QUIT) ok = ACTION_CONTINUE;
			if (ok != ACTION_CONTINUE) break;
			
			while (K_ESC() || K_LEFT() || K_ENTER());
		}
		
		else if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB() ||
			 K_SCRATCHPAD() || K_ENTER()) break;
		
		else if (any_key_pressed()) {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				break;
			SDL_Flip(scr);
		}
		
	} while (!K_ESC());
	
	args->isActive = 0;
	return ok;
}


int OpenMenu(Widget *w, wMENU *m, int xr, int xl, int minWidth, int y)
{
	if (!w || !m) return MENU_QUIT;
	if (!m->nItems || !m->items) return MENU_QUIT;

	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ListArgs *args = w->args;
	wITEM *itm;
	wMENU *menu;
	int x, l;
	SDL_Rect area;
	SDL_Surface *saveImage;
	Uint32 c = Darker(t->color1, 60);
	Uint32 cs1 = Darker(t->menus_c1, 15);	// color selection
	args->cMenu = m;
	args->yItem = 0;
	args->cItem = 0;
	int dItems = min(args->dItems, m->nItems);
	if (!dItems) dItems = m->nItems;
	SDL_Rect clip;
	SDL_GetClipRect(scr, &clip);
	int b;
	int ok = ACTION_CONTINUE;
	
	// calcul de la hauteur
	int h = 10*dItems+2;
	while (h > 234) h-=10, dItems-=1;
	if (y+h > scr->h) y = scr->h - h;
	
	// calcul de la largeur
	int wd = 0;
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		l = nSDL_GetStringWidth(t->menus_font1, itm->str);
		if (l > wd) wd = l;
		l = nSDL_GetStringWidth(t->menus_font2, itm->str);
		if (l > wd) wd = l;
	}
	wd += 10;
	if (m->nItems > dItems) wd += 7;	// pour les scrollbars
	if (wd < minWidth) wd = minWidth;
	if (wd > scr->w) wd = scr->w;
	if (xr+wd > scr->w) {
		if (xl-wd > 0) xr = xl - wd;
		else xr = scr->w - wd;
	}
	
	// sauvegarde de l'image
	area = (SDL_Rect) {xr, y, wd, h};
	saveImage = SDL_CreateRGBSurface(SDL_SWSURFACE, wd, h, 16, 0, 0, 0, 0);
	DrawSurface(scr, &area, saveImage, NULL);
	if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE) {
		SDL_FreeSurface(saveImage);
		return ok;
	}
	
	do {
		// dessin !!!
		SDL_SetClipRect(scr, &area);
		DrawFillRect(scr, &area, t->menus_c1);
		DrawRect(scr, &area, c);
		
		
		for (x=0; x < dItems; x++) {
			itm = m->items[x + args->yItem];
			
			if (x == args->cItem)
				DrawFillRectXY(scr, xr+1, y+1+10*x, wd-2, 10, cs1);
			
			if (itm->magicNumber == 1) {
				menu = (wMENU *) itm;
				
				if (wMenu_HasSelectedItemOrMenu(menu))
					DrawClippedStr(scr, t->menus_font2, xr+2, y+2+10*x, menu->title);
				else
					DrawClippedStr(scr, t->menus_font1, xr+2, y+2+10*x, menu->title);
				
				DrawSurface(t->menus_right,NULL,scr, &(SDL_Rect){xr+wd - (m->nItems > dItems? 17:8), y+2+10*x, 5, 7});
			}
			
			else {
				if (itm->isSelected)
					DrawClippedStr(scr, t->menus_font2, xr+2, y+2+10*x, itm->str);
				else
					DrawClippedStr(scr, t->menus_font1, xr+2, y+2+10*x, itm->str);
			}
		}
		if (m->nItems > dItems)
			DrawVScrollBar(scr, t, &area, 10*m->nItems+2, 10*args->yItem);
		SDL_SetClipRect(scr, &clip);
		
		
		// activation !!!
		itm = m->items[args->cItem + args->yItem];
	  ACTIVATION:
		SDL_Flip(scr);
		while ((any_key_pressed() && !K_CTRL()) || K_CLICK());
		while (!any_key_pressed());
		
		if (K_CLICK() || K_ENTER()) {
			if (K_ENTER() && args->maxSelected != 1) {
				ok = wExecCallback(w, SIGNAL_CLICK);
				break;
			}
			if (itm->magicNumber == 1) goto NEWMENU;
			
			if (args->maxSelected == 1) {
				if (args->nSelected) wMenu_DeselectAll(args->menu);
				wMenu_SelectItem(m, args->yItem + args->cItem);
				args->nSelected = 1;
				
				DrawSurface(saveImage, NULL, scr, &area);
				SDL_FreeSurface(saveImage);
				saveImage = NULL;
				ok = wExecCallback(w, K_CLICK() || K_ENTER()? SIGNAL_CLICK : SIGNAL_ACTION);
				break;
			}
			
			else if (args->maxSelected >= 0) {
				b = wMenu_IsSelected(m, args->yItem + args->cItem);
				if (!b && args->nSelected)
					if (args->nSelected == args->maxSelected) goto ACTIVATION;
				
				wMenu_SelectItem(m, args->yItem + args->cItem);
				if (b) args->nSelected--;
				else args->nSelected++;
			}
			if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
				break;
		}
		
		else if (K_UP()) {
			if (!args->yItem && !args->cItem) {
				if (K_CTRL()) goto ACTIVATION;
				args->cItem = dItems-1;
				args->yItem = m->nItems-dItems;
			}
			else {
				if (args->cItem) args->cItem--;
				else args->yItem--;
			}
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				break;
		}
		
		else if (K_DOWN()) {
			if (args->yItem + args->cItem >= m->nItems-1) {
				if (K_CTRL()) goto ACTIVATION;
				args->cItem = 0;
				args->yItem = 0;
			}
			else {
				if (args->cItem < dItems-1) args->cItem++;
				else args->yItem++;
			}
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				break;
		}
		
		else if (K_RIGHT() && itm->magicNumber == 1) {
		  NEWMENU:;
			int yI = args->yItem, cI = args->cItem;
			ok = OpenMenu(w, (wMENU *) itm, xr+wd-3, xr, minWidth, y+10*args->cItem);
			
			args->cMenu = m;
			args->yItem = yI;
			args->cItem = cI;
			if (K_ESC() || K_ENTER() || (K_CLICK() && args->maxSelected == 1) || ok != ACTION_CONTINUE) break;
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				break;
		}
		
		else if (K_LEFT()) break;
	} while (!K_ESC());
	
	
	if (saveImage) {
		DrawSurface(saveImage, NULL, scr, &area);
		SDL_FreeSurface(saveImage);
	}
	return ok;
}




// Fermeture
void CloseList(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST && w->type != WIDGET_EXPLORER) return;
	
	if (w->args) {
		ListArgs *args = w->args;
		if (wIsFreedArg(w, WMENU)) wAddMenuToConstruct(w->construct, args->menu);
		if (w->type == WIDGET_EXPLORER) free(args->cMenu);
	}
}



// Méthodes
// 0 - Méthodes de base
void wList_SetMenu(Widget *w, wMENU *m)
{
	if (!w || !m) return;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return;
	ListArgs *args = w->args;
	
	if (w->construct) wAddMenuToConstruct(w->construct, args->menu);
	else wMenu_Close(args->menu);
	args->menu = m;
	args->cMenu = m;
	args->nSelected = 0;
	wMenu_DeselectAll(m);
	
	if ((args->yItem || args->cItem) && (args->yItem + args->cItem >= m->nItems)) {
		args->cItem = min(m->nItems, args->dItems)-1;
		args->yItem = max(0, m->nItems - args->cItem - 1);
	}
}


wMENU *wList_GetMenu(Widget *w)
{
	if (!w) return NULL;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return NULL;
	ListArgs *args = w->args;
	return args->menu;
}


void wList_SetMaxSelectableItem(Widget *w, int max)
{
	if (!w) return;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return;
	ListArgs *args = w->args;
	if (max < 0) max = 0;
	
	if (max < args->nSelected) wList_DeselectAll(w);
	args->maxSelected = max;
}




// I - GetCurrent...
wMENU *wList_GetCurrentItem(Widget *w, int *nItem)
{
	if (nItem) *nItem = -1;
	if (!w) return NULL;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST && w->type != WIDGET_EXPLORER) return NULL;
	ListArgs *args = w->args;
	wMENU *m = args->cMenu;
	if (w->type == WIDGET_EXPLORER) m = args->menu;
	if (!m) return NULL;
	
	
	if (nItem && m->nItems > 0) *nItem = args->yItem + args->cItem;
	return m;
}

char *wList_GetCurrentStr(Widget *w)
{
	if (!w) return NULL;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST && w->type != WIDGET_EXPLORER) return NULL;
	ListArgs *args = w->args;
	wMENU *m = args->cMenu;
	if (w->type == WIDGET_EXPLORER) m = args->menu;
	if (!m || m->nItems < 1) return NULL;
	
	wITEM *itm = m->items[args->yItem + args->cItem];
	return itm->str;
}

void wList_GetCurrentPath(Widget *w, char *output)
{
	if (!w || !output) return;
	ListArgs *args = w->args;
	
	if (w->type == WIDGET_EXPLORER) {
		strcpy(output, (char *) args->cMenu);
		return;
	}
	
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return;
	if (!args->menu || args->menu->nItems < 1) return;
	
	
	output[0] = 0;
	wMENU *cm = args->cMenu;
	output[0] = 0;
	int x;
	int l;
	
	while (cm) {
		if (cm->title) {
			l = strlen(output);
			memmove(output+strlen(cm->title)+1, output, l+1);
			
			x = -1;
			output[0] = '/';
			while (cm->title[++x]) output[x+1] = cm->title[x];
		}
		
		cm = cm->parent;
	}
}


// II - GetSelected...
wMENU *wList_GetSelectedItem(Widget *w, int *item)
{
	if (item) *item = -1;
	if (!w) return NULL;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return NULL;
	ListArgs *args = w->args;
	if (args->maxSelected != 1) return NULL;
	if (args->nSelected != 1) return NULL;
	wMENU *cm = args->menu;
	wMENU *sm;
	wITEM *itm;
	
	int x;
	
	for (x=0; x < cm->nItems; x++) {
		itm = cm->items[x];
		
		if (itm->magicNumber == 2) {
			if (itm->isSelected) {
				if (item) *item = x;
				return cm;
			}
		}
		
		else {
			sm = (wMENU *) itm;
			if (wMenu_HasSelectedItemOrMenu(sm)) {	
				cm = sm;
				x = -1;
			}
		}
	}
	return NULL;	// ne devrait pas arriver
}

char *wList_GetSelectedStr(Widget *w)
{
	if (!w) return NULL;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return NULL;
	ListArgs *args = w->args;
	if (args->maxSelected != 1) return NULL;
	if (args->nSelected != 1) return NULL;
	wMENU *cm = args->menu;
	wMENU *sm;
	wITEM *itm;
	
	int x;
	
	for (x=0; x < cm->nItems; x++) {
		itm = cm->items[x];
		
		if (itm->magicNumber == 2) {
			if (itm->isSelected) return itm->str;
		}
		
		else {
			sm = (wMENU *) itm;
			if (wMenu_HasSelectedItemOrMenu(sm)) {
				cm = sm;
				x = -1;
			}
		}
	}
	return NULL;	// ne devrait pas arriver
}

void wList_GetPath(Widget *w, char *output)
{
	if (!w || !output) return;
	output[0] = 0;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return;
	ListArgs *args = w->args;
	if (args->maxSelected != 1) return;
	if (args->nSelected != 1) return;
	wMENU *cm = args->menu;
	wMENU *sm;
	wITEM *itm;
	
	int x;
	
	for (x=0; x < cm->nItems; x++) {
		itm = cm->items[x];
		
		if (itm->magicNumber == 2) {
			if (itm->isSelected) {
				strcat(output, "/");
				strcat(output, itm->str);
				return;
			}
		}
		
		else {
			sm = (wMENU *) itm;
			if (wMenu_HasSelectedItemOrMenu(sm)) {	
				strcat(output, "/");
				strcat(output, sm->title);
				cm = sm;
				x = -1;
			}
		}
	}
}




// III - GetSelected...s !
int wList_GetNumberOfSelectedItems(Widget *w)
{
	if (!w) return -1;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return -1;
	ListArgs *args = w->args;
	
	return args->nSelected;
}


static void SearchSelectedItemsInMenu(wMENU *m, wMENU **menus, int *items, int *p)
{
	if (!m) return;
	wITEM *itm;
	int x;
	
	// On regarde les items sélectionnés
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) continue;
		if (itm->isSelected) {
			if (menus) menus[*p] = m;
			if (items) items[(*p)++] = x;
		}
	}
	
	// on regarde dans les sous-menus
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) SearchSelectedItemsInMenu((wMENU *) itm, menus, items, p);
	}
}


int wList_GetSelectedItems(Widget *w, wMENU **menus, int *items)
{
	if (!w) return 0;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return 0;
	ListArgs *args = w->args;
	if (!args->menu || args->menu->nItems < 1) return 0;
	if (args->nSelected < 1) return 0;
	
	int x;
	int max = args->maxSelected;
	int p = 0;
	
	// on initialise les tableaux
	for (x=0; x < max; x++) {
		if (menus) menus[x] = NULL;
		if (items) items[x] = -1;
	}
	
	// on trouve à présent tous les éléments sélectionnés
	SearchSelectedItemsInMenu(args->menu, menus, items, &p);
	
	return 1;
}



static void SearchSelectedStringsInMenu(wMENU *m, char **itemStr, int *p)
{
	if (!m) return;
	wITEM *itm;
	int x;
	
	// On regarde les items sélectionnés
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) continue;
		if (itm->isSelected) itemStr[(*p)++] = itm->str;
	}
	
	// on regarde dans les sous-menus
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) SearchSelectedStringsInMenu((wMENU *) itm, itemStr, p);
	}
}

int wList_GetSelectedStrings(Widget *w, char **itemStr)
{
	if (!w || !itemStr) return 0;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return 0;
	ListArgs *args = w->args;
	if (!args->menu || args->menu->nItems < 1) return 0;
	if (args->nSelected < 1) return 0;
	
	int x;
	int max = args->maxSelected;
	int p = 0;
	
	// on initialise les tableaux
	for (x=0; x < max; x++) itemStr[x] = NULL;
	
	// on trouve à présent tous les éléments sélectionnés
	SearchSelectedStringsInMenu(args->menu, itemStr, &p);
	
	return 1;
}



static void SearchSelectedPathsInMenu(wMENU *m, char *path, char **paths, int *p)
{
	if (!m) return;
	wITEM *itm;
	int x;
	char subPath[512];
	
	// On regarde les items sélectionnés
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) continue;
		if (itm->isSelected) {
			strcpy(paths[*p], path);
			strcat(paths[*p], "/");
			strcat(paths[*p], itm->str);
			(*p)++;
		}
	}
	
	// on regarde dans les sous-menus
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 1) {
			sprintf(subPath, "%s/%s", path, itm->str);
			SearchSelectedPathsInMenu((wMENU *) itm, subPath, paths, p);
		}
	}
}

int wList_GetPaths(Widget *w, char **paths)
{
	if (!w || !paths) return 0;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return 0;
	ListArgs *args = w->args;
	if (!args->menu || args->menu->nItems < 1) return 0;
	if (args->nSelected < 1) return 0;
	char path[1] = "";
	
	int x;
	int max = args->maxSelected;
	int p = 0;
	
	// on initialise les tableaux
	for (x=0; x < max; x++) paths[x][0] = 0;
	
	// on trouve à présent tous les éléments sélectionnés
	SearchSelectedPathsInMenu(args->menu, path, paths, &p);
	
	return 1;
}




// IV - Select...
int wList_SelectItem(Widget *w, wMENU *menu, int item)
{
	if (!w) return -1;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return -1;
	ListArgs *args = w->args;
	if (args->maxSelected < 0) return 0;
	if (!args->menu) return -1;
	if (!menu) menu = args->menu;
	if (item >= menu->nItems) return -1;
	wITEM *itm = menu->items[item];
	
	if (itm->magicNumber != 2) return 0;
	
	if (args->maxSelected == 1) {
		wList_DeselectAll(w);
		itm->isSelected = 1;
		menu->nSelected = 1;
		args->nSelected = 1;
	}
	
	else {
		if (args->maxSelected && args->nSelected == args->maxSelected && !itm->isSelected) return 0;  // si le nombre max d'items sélectionnés est atteint
		
		itm->isSelected = !itm->isSelected;
		if (itm->isSelected) args->nSelected++, menu->nSelected++;
		else args->nSelected--, menu->nSelected--;
	}
	
	return 1;
}


static int CheckStrInMenu(Widget *w, wMENU *m, const char *str)
{
	int n;
	
	if ((n=wMenu_GetItemFromStr(m, str)) >= 0) {
		wList_SelectItem(w, m, n);
		return 1;
	}
	
	else {
		int x;
		wMENU *sm;
		for (x=0; x < m->nItems; x++) {
			sm = m->items[x];
			if (sm->magicNumber == 1 && CheckStrInMenu(w, sm, str)) return 1;
		}
		return 0;
	}
}


int wList_SelectItemStr(Widget *w, const char *str)
{
	if (!w) return -1;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return -1;
	ListArgs *args = w->args;
	if (args->maxSelected < 0) return 0;
	
	return CheckStrInMenu(w, args->menu, str);
}



int wList_SelectPath(Widget *w, const char *path)
{
	if (!w || !path || !path[0] || !path[1]) return -1;
	ListArgs *args = w->args;
	if (!args || !args->menu) return -1;
	if (args->maxSelected < 0) return 0;
	
	wMENU *m = args->menu;
	wITEM *itm;
	int x=0, ni;
	char *p = (char *) path;
	if (*p == '/') p++;
	char *c = p;
	// char debug[64];
	
	
	while (c) {
		c = strchr(p, '/');
		if (c) *c = 0;
		
		// on cherche si la phrase est un sous-menu ou non
		ni = m->nItems;
		for (x=0; x < ni; x++) {
			itm = m->items[x];
			// sprintf(debug, "%s\25%s\25%i", p, itm->str, strcmp(itm->str, p));
			// wShowMsg("DEBUG", debug);
			
			if (!strcmp(itm->str, p)) {  // on a trouvé un résultat !!!
				if ((c!=NULL) == itm->magicNumber-1) continue;  // si le dernier du path est un menu, ou un élément précédent du path est un item
				
				if (!c) wList_SelectItem(w, m, x);  // c'est l'item
				else m = (wMENU*) itm;  // c'est un sous-menu
				break;
			}
		}
				
		if (c) *c = '/';
		if (x == ni) return 0;  // on n'a pas trouvé la phrase !!!
		p = c+1;
	}
	
	return 1;
}





void wList_DeselectAll(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_LIST && w->type != WIDGET_BOXLIST) return;
	ListArgs *args = w->args;
	
	wMenu_DeselectAll(args->menu);
	args->nSelected = 0;
}








