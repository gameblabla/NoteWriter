#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "List.h"
#include "BoxList.h"
#include "menus.h"
#include "general.h"
#include "keys.h"
#include "scrollbar.h"
#include "widgets.h"
#include "EasyFiles.h"
#include "dialogs.h"




/* FONCTIONS BOXEXPLORER 
----------------------------------*/

static wMENU *explorerMenu(const char *path, const char *pattern)
{
	// wFastMsg("EXPLORER MENU\n---path---\n%s\n\n---pattern---\%s", path, pattern);
	wMENU *m = wMenu(pattern);
	
	char *elts[512];
	int n = folder_GetElements(path, pattern, elts);
	int x;
	char pathElt[256];
	
	for (x=0; x < n; x++) {
		strcpy(pathElt, path);
		if (!path[0] || path[strlen(path)-1] != '/') strcat(pathElt, "/");
		strcat(pathElt, elts[x]);
		
		if (isFile(pathElt)) 			wMenu_AddItem(m, elts[x]);
		else if (isFolder(pathElt))	wMenu_AddMenu(m, wMenu(elts[x]));
		else wFastMsg("ERROR\nPath is neither a folder nor a file :\n%s", pathElt);
		free(elts[x]);
	}
	
	return m;
}


static void editPath(Widget *explorer, char *folder)
// si folder = NULL, on va un dossier en arrière
// sinon, on ajoute folder au path du menu
// le path du menu est dans args->cMenu
{
	if (!explorer) return;
	if (explorer->type != WIDGET_EXPLORER) return;
	ListArgs *args = explorer->args;
	char *path;
	
	
	if (folder) {
		args->cMenu = realloc(args->cMenu, strlen((char *) args->cMenu) + strlen(folder) + 2);
		path = (char *) args->cMenu;
		
		if ((!path[0] || path[strlen(path)-1] != '/') && folder[0] != '/')
			strcat(path, "/");
		strcat(path, folder);
	}
	else {
		path = strrchr((char *) args->cMenu, '/');
		if (!path) path = (char *) args->cMenu;
		*path = 0;
	}
}




Widget *wBoxExplorer(char *path, char *pattern, int dItems)
{
	Widget *w = wBoxList(explorerMenu(path, pattern), dItems, 0);
	ListArgs *args = w->args;
	w->type = WIDGET_EXPLORER;
	args->cMenu = malloc(strlen(path)+1);
	strcpy((char *) args->cMenu, path);
	return w;
}

/*
----------------------------------*/





Widget *wBoxList(wMENU *m, int dItems, int maxSelected)
{
	if (dItems < 1) dItems = 8;
	Widget *w = wExList(m, dItems, maxSelected);
	if (!w) return NULL;
	
	w->type = WIDGET_BOXLIST;
	w->bounds.h = 12*dItems+4;
	w->bounds.y = (240-w->bounds.h)/2;
	w->draw	= DrawBoxList;
	w->activate	= ActivateBoxList;
	
	return w;
}


void DrawBoxList(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	ListArgs *args = w->args;
	wMENU *m = args->cMenu;
	if (w->type == WIDGET_EXPLORER) m = args->menu;
	wMENU *menu;
	wITEM *itm;
	BOOL scrollB = (m->nItems > args->dItems);
	SDL_Rect clip;
	SDL_GetClipRect(scr, &clip);
	Uint32 c1 = t->request_c2;
	if (args->isActive == 1) c1 = t->text_c1;
	else if (args->isActive == 2) c1 = t->request_c1;
	if (!c1) c1 = t->color1;
	int wd = w->bounds.w-4;
	int x;
	int xr = w->bounds.x+2;
	int y = w->bounds.y+2;
	Uint32 cs1 = Darker(t->menus_c1, 15);	// color selection
	
	// Dessin des rectangles (background)
	DrawFillRectXY(scr, xr, y, w->bounds.w-4, w->bounds.h-4, c1);
	if (!args->isActive)
		DrawRectXY(scr, xr-1, y-1, w->bounds.w-2, w->bounds.h-2, t->color2);
	
	
	// Dessin des ScrollBars
	if (scrollB) {
		int wsb = 11;
		wd -= wsb-1;
		DrawVScrollBar(scr, t, &w->bounds, 12*m->nItems+4, 12*args->yItem);
		
		if (args->isActive < 2) {
			SDL_Surface *s_im;
			s_im = SDL_CreateRGBSurface(SDL_SWSURFACE, wsb, w->bounds.h-4, 16, 0,0,0,0);
			DrawFillRect(s_im, NULL, c1);
			SDL_SetAlpha(s_im, SDL_SRCALPHA, 128);
			DrawSurface(s_im, NULL, scr, &(SDL_Rect) {xr+w->bounds.w-wsb-2, y, wsb, w->bounds.h-4});
			SDL_FreeSurface(s_im);
		}
		DrawFillRectXY(scr, xr+w->bounds.w-wsb-3, y, 1, w->bounds.h-4, t->color3);
	}
	
	SDL_SetClipRect(scr, &(SDL_Rect) {xr, y, wd, w->bounds.h-4});
	
	
	// Dessin des Items
	for (x=0; x < min(m->nItems, args->dItems); x++) {
		itm = m->items[x + args->yItem];
		
		if (x == args->cItem && args->isActive == 2)
			DrawFillRectXY(scr, xr, y+12*x, wd, 12, cs1);
		
		if (itm->magicNumber == 1) {
			menu = (wMENU *) itm;
			
			if (wMenu_HasSelectedItemOrMenu(menu))
				DrawClippedStr(scr, t->menus_font2, xr+1, y+2+12*x, menu->title);
			else
				DrawClippedStr(scr, t->menus_font1, xr+1, y+2+12*x, menu->title);
			
			DrawSurface(t->menus_right,NULL,scr, &(SDL_Rect){xr+wd - (m->nItems > args->dItems? 17:8), y+3+12*x, 5, 7});
		}
		
		else {
			if (itm->isSelected)
				DrawClippedStr(scr, t->menus_font2, xr+1, y+2+12*x, itm->str);
			else
				DrawClippedStr(scr, t->menus_font1, xr+1, y+2+12*x, itm->str);
		}
	}
	
	if (!m->nItems && args->isActive == 2 && args->dItems) DrawFillRectXY(scr, xr, y, wd, 12, cs1);

	SDL_SetClipRect(scr, &clip);
}



int  ActivateBoxList(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	ListArgs *args = w->args;
	wMENU *m;
	wITEM *itm;
	BOOL b;
	BOOL ood = w->construct->onlyOneDynamic;
	if (ood && (args->menu->nItems || w->type == WIDGET_EXPLORER)) goto INLOOP;
	int ok = ACTION_CONTINUE;
	
	
	while (!K_ESC()) {
		args->isActive = 1;
		DrawBoxList(w);
		wDrawBounds(w);
		SDL_Flip(scr);
		wait_key_pressed();
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			|| K_SCRATCHPAD() || K_ENTER() || K_ESC()) break;
		
		if (K_CLICK()) {
		  INLOOP:
			args->isActive = 2;
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				return ok;
			if (!args->menu->nItems && w->type != WIDGET_EXPLORER) {
				while (K_CLICK());
				continue;
			}
		
			do {
				m = args->cMenu;
				if (w->type == WIDGET_EXPLORER) m = args->menu;
				
				if (!m->nItems) itm = NULL;
				else itm = m->items[args->cItem + args->yItem];
				DrawBoxList(w);
				wDrawBounds(w);
				SDL_Flip(scr);
			  ACTIVATION:
				while ((any_key_pressed() && !K_CTRL()) || K_CLICK());
				while (!any_key_pressed());
				if (K_MENU() || K_TAB() || K_SCRATCHPAD()) goto QUIT;
				
				
				if (itm && (K_CLICK() || (K_ENTER() && itm->magicNumber==2 && (!wHasDialogParent(w) || args->maxSelected==1)))) {
					if (itm->magicNumber == 1) goto NEWMENU;
					
					if (args->maxSelected == 1) {
						if (args->nSelected) wMenu_DeselectAll(args->menu);
						wMenu_SelectItem(m, args->yItem + args->cItem);
						args->nSelected = 1;
					}
					
					else if (args->maxSelected > 1) {
						b = wMenu_IsSelected(m, args->yItem + args->cItem);
						if (!b && args->nSelected)
							if (args->nSelected == args->maxSelected) goto ACTIVATION;
						
						wMenu_SelectItem(m, args->yItem + args->cItem);
						if (b) args->nSelected--;
						else args->nSelected++;
					}
					
					if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
						return ok;
				}
				
				if (K_ENTER() || K_ESC()) {
					if (ood) goto QUIT;
					break;
				}
				
				
				if (K_UP() && itm) {
					if (!args->yItem && !args->cItem) {
						if (K_CTRL()) goto ACTIVATION;
						args->cItem = args->dItems-1;
						args->yItem = m->nItems-args->dItems;
						if (args->cItem >= m->nItems) args->cItem = m->nItems-1;
						if (args->yItem < 0) args->yItem = 0;
					}
					else {
						if (args->cItem) args->cItem--;
						else args->yItem--;
					}
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
				}
				
				else if (K_DOWN() && itm) {
					if (args->yItem + args->cItem >= m->nItems-1) {
						if (K_CTRL()) goto ACTIVATION;
						args->cItem = 0;
						args->yItem = 0;
					}
					else {
						if (args->cItem < args->dItems-1) args->cItem++;
						else args->yItem++;
					}
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
				}
				
				else if (K_RIGHT() && itm) {
					if (itm->magicNumber == 2) goto ACTIVATION;
				  NEWMENU:;
					if (w->type == WIDGET_EXPLORER) {
						editPath(w, wList_GetCurrentStr(w));
						wMENU *m = explorerMenu((char *) args->cMenu, args->menu->title);
						wMenu_Close(args->menu);
						args->menu = m;
					}
					else args->cMenu = (wMENU *) itm;
					
					args->yItem = 0;
					args->cItem = 0;
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
				}
				
				else if (K_LEFT()) {
					if (!m->parent && w->type != WIDGET_EXPLORER) goto ACTIVATION;
					if (w->type == WIDGET_EXPLORER) {
						editPath(w, NULL);
						wMENU *m = explorerMenu((char *) args->cMenu, args->menu->title);
						wMenu_Close(args->menu);
						args->menu = m;
					}
					else args->cMenu = m->parent;
					
					args->yItem = 0;
					args->cItem = 0;
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
				}
				
				else {
					if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
						return ok;
					while (K_ESC());
				}
				
				
			} while (!K_ESC());
			
			while(K_ESC());
		}
		
		else {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				return ok;
			DrawBoxList(w);
			SDL_Flip(scr);
			wait_no_key_pressed();
		}
		
	}
	
  QUIT:
	args->isActive = 0;
	
	return ACTION_CONTINUE;
}

