#include <os.h>
#include <SDL/SDL.h>
#include <Winbox.h>

#include "structures.h"
#include "theme.h"
#include "widgets.h"
#include "general.h"
#include "keys.h"
#include "dialogs.h"


char Clipboard[1024];
wTHEME *Theme;

typedef struct LAYOUT {
	int nWidgets;
	Widget **widgets;
	int cWidget;
} LAYOUT;

typedef struct WINDOW_LAYOUT {
	Widget		*widget;
} WLAYOUT;

/* Ce fichier regroupe toutes les fonctions communes à tous les
widgets : constructs, dessin, acivation, ...
*/

// CONSTRUCT ------------------------------------------------------------------------
wCONSTRUCT *wNewConstruct(Widget *w)
{
	wCONSTRUCT *c = malloc(sizeof(wCONSTRUCT));
	if (!c || !w) return NULL;
	c->scr = SDL_GetVideoSurface();
	c->theme = NULL;
	c->saveImage = 1;
	c->image = NULL;
	c->onlyOneDynamic = 1;
	
	// les widgets
	c->widgets = malloc(sizeof(Widget *));
	if (!c->widgets) {
		free(c);
		return NULL;
	}
	c->widgets[0] = w;
	w->construct = c;
	c->nWidgets = 1;
	
	// les autres
	c->fonts = NULL;
	c->nFonts = 0;
	c->surfaces = NULL;
	c->nSurfaces = 0;
	c->elts = NULL;
	c->nElts = 0;
	
	return c;
}

int wActivateConstruct(Widget *w)
{
	if (!w) return -1;
	int ok = ACTION_REFRESH;
	wCONSTRUCT *c;
	Clipboard[0] = 0;  // on (ré)initialise le Clipboard
	Clipboard[1021] = 0;
	
	// on obtient le construct
	if (!w->construct) c = wNewConstruct(w);
	else					 c = w->construct;
		
	if (!c->image) {
		c->image = SDL_CreateRGBSurface(SDL_SWSURFACE, c->widgets[0]->bounds.w, c->widgets[0]->bounds.h, 16, 0,0,0,0);
		DrawSurface(c->scr, &c->widgets[0]->bounds, c->image, NULL);
	}
	
	
	// on active
	while (ok == ACTION_REFRESH) {
		// on gère le thème
		if (c->theme != Theme) c->theme = Theme;
		wDrawWidget(c->widgets[0]);
		SDL_Flip(c->scr);
		while (K_ENTER() || K_ESC() || K_CLICK() || K_LEFT() || K_RIGHT() || K_UP() || K_DOWN());
		
		if (w->isDynamic)
			ok = wActivateWidget(c->widgets[0]);
		
		else {
			while (!K_ESC() && !K_ENTER()) {
				SDL_Flip(SDL_GetVideoSurface());
				wait_key_pressed();
				if (!K_ESC() && !K_ENTER()) {
					wExecCallback(w, SIGNAL_KEY);
					wDrawWidget(c->widgets[0]);
				}
			}
			
			if (K_ESC()) ok = 0;
			else ok = 1;
		}
	}
	
	return ok;
}


int wExecConstruct(Widget *w)
{
	int ok = wActivateConstruct(w);
	if (ok != -1) wCloseConstruct(w);
	return ok;
}


void wDrawConstruct(Widget *w)
{
	if (!w) return;
	wCONSTRUCT *c;
	
	// on obtient le construct
	if (!w->construct) c = wNewConstruct(w);
	else					 c = w->construct;
		
	if (!c->image) {
		c->image = SDL_CreateRGBSurface(SDL_SWSURFACE, c->widgets[0]->bounds.w, c->widgets[0]->bounds.h, 16, 0,0,0,0);
		DrawSurface(c->scr, &c->widgets[0]->bounds, c->image, NULL);
	}
	
	// on gère le thème
	if (!c->theme) c->theme = Theme;
	
	// on dessine
	wDrawWidget(c->widgets[0]);
	SDL_Flip(c->scr);
}


void wCloseConstruct(Widget *w)
{
	if (!w) return;
	wCONSTRUCT *c = w->construct;
	if (!c) return;
	int x;
	BOOL b = c->saveImage;
	
	if (c->image) {
		if (b) DrawSurface(c->image, NULL, c->scr, &c->widgets[0]->bounds);
		SDL_FreeSurface(c->image);
	}
	
	// Les widgets
	for (x=0; x < c->nWidgets; x++)
		wCloseWidget(c->widgets[x]);
	free(c->widgets);
	
	
	// Les fonts
	for (x=0; x < c->nFonts; x++)
		nSDL_FreeFont(c->fonts[x]);
	free(c->fonts);
	
	
	// Les surfaces
	for (x=0; x < c->nSurfaces; x++)
		SDL_FreeSurface(c->surfaces[x]);
	
	
	// Les éléments
	for (x=0; x < c->nElts; x++)
		free(c->elts[x]);
	free(c->elts);
	
	// on ferme le thème et on actualise l'écran
	if (b) SDL_Flip(c->scr);
	free(c);
	while (K_ENTER() || K_UP() || K_DOWN());
}


void wConstruct_ChangeTheme(Widget *w, wTHEME *theme)
{
	wInitTheme(theme);
	w->construct->theme = Theme;
}


void wConstruct_SaveImage(wCONSTRUCT *c)
{
	if (!c) return;
	c->saveImage = 1;
}


void wConstruct_DontSaveImage(wCONSTRUCT *c)
{
	if (!c) return;
	c->saveImage = 0;
}


void wMoveConstruct(Widget *wid, int x, int y, int w, int h)
{
	if (!wid) return;
	wCONSTRUCT *c = wid->construct;
	if (!c) return;
	Widget *wd = c->widgets[0];
	
	if (c->image) DrawSurface(c->image, NULL, c->scr, &wd->bounds);
	wd->bounds.x = x;
	wd->bounds.y = y;
	wd->bounds.h = h;
	wd->bounds.w = w;
	if (c->image) {
		SDL_FreeSurface(c->image);
		c->image = SDL_CreateRGBSurface(SDL_SWSURFACE, wd->bounds.w, wd->bounds.h, 16, 0,0,0,0);
		DrawSurface(c->scr, &wd->bounds, c->image, NULL);
	}
	wDrawWidget(wd);
}


void wMoveConstructR(Widget *wid, SDL_Rect *r)
{
	return wMoveConstruct(wid, r->x, r->y, r->w, r->h);
}


int wAddWidgetToConstruct(wCONSTRUCT *c, Widget *w)
{
	if (!w || !c) return 0;
	int x;
	void *tmp;
	
	// on vérifie que le widget n'est pas déjà enregistré
	if (w->construct == c) return 0;
	for (x=0; x < c->nWidgets; x++)
		if (w == c->widgets[x]) return 0;
	
	// on l'ajoute
	tmp = realloc(c->widgets, (c->nWidgets+1)*sizeof(Widget *));
	if (!tmp) return 0;
	c->widgets = tmp;
	c->widgets[c->nWidgets++] = w;
	w->construct = c;
	return 1;
}


int wAddFontToConstruct(wCONSTRUCT *c, nSDL_Font *f)
{
	if (!f || !c) return 0;
	int x;
	void *tmp;
	
	// on vérifie que la font n'est pas déjà enregistréé
	for (x=0; x < c->nFonts; x++)
		if (f == c->fonts[x]) return 0;
	
	// on l'ajoute
	tmp = realloc(c->fonts, (c->nFonts+1)*sizeof(nSDL_Font *));
	if (!tmp) return 0;
	c->fonts = tmp;
	c->fonts[c->nFonts++] = f;
	return 1;
}


int wAddSurfaceToConstruct(wCONSTRUCT *c, SDL_Surface *s)
{
	if (!s || !c) return 0;
	int x;
	void *tmp;
	
	// on vérifie que la font n'est pas déjà enregistréé
	for (x=0; x < c->nSurfaces; x++)
		if (s == c->surfaces[x]) return 0;
	
	// on l'ajoute
	tmp = realloc(c->surfaces, (c->nSurfaces+1)*sizeof(SDL_Surface *));
	if (!tmp) return 0;
	c->surfaces = tmp;
	c->surfaces[c->nSurfaces++] = s;
	return 1;
}


int wAddMenuToConstruct(wCONSTRUCT *c, wMENU *m)
{
	if (!m || !c) return 0;
	int x;
	
	// on supprime le menu principal
	wAddEltToConstruct(c, m);
	wAddEltToConstruct(c, m->items);
	
	// on supprime les items et sous-menus
	for (x=0; x < m->nItems; x++) {
		wMENU *sm = m->items[x];
		if (sm->magicNumber == 1) wAddMenuToConstruct(c, sm);
		else wAddEltToConstruct(c, sm);
	}
	return 1;
}



int wAddEltToConstruct(wCONSTRUCT *c, void *e)
{
	if (!e || !c) return 0;
	int x;
	void *tmp;
	
	// on vérifie que l'élément n'est pas déjà enregistréé
	for (x=0; x < c->nElts; x++)
		if (e == c->elts[x]) return 0;
	
	// on l'ajoute
	tmp = realloc(c->elts, (c->nElts+1)*sizeof(void *));
	if (!tmp) return 0;
	c->elts = tmp;
	c->elts[c->nElts++] = e;
	return 1;
}


int wRemoveWidgetFromConstruct(wCONSTRUCT *c, Widget *w)
{
	int x;
	
	for (x=0; x < c->nWidgets; x++) {
		if (c->widgets[x] == w) {
			c->widgets[x] = NULL;
			w->construct = NULL;
			return 1;
		}
	}
	return 0;
}

int wRemoveFontFromConstruct(wCONSTRUCT *c, nSDL_Font *f)
{
	int x;
	
	for (x=0; x < c->nFonts; x++) {
		if (c->fonts[x] == f) {
			c->fonts[x] = NULL;
			return 1;
		}
	}
	return 0;
}

int wRemoveSurfaceFromConstruct(wCONSTRUCT *c, SDL_Surface *s)
{
	int x;
	
	for (x=0; x < c->nSurfaces; x++) {
		if (c->surfaces[x] == s) {
			c->surfaces[x] = NULL;
			return 1;
		}
	}
	return 0;
}

int wRemoveEltFromConstruct(wCONSTRUCT *c, void *e)
{
	int x;
	
	for (x=0; x < c->nElts; x++) {
		if (c->elts[x] == e) {
			c->elts[x] = NULL;
			return 1;
		}
	}
	return 0;
}

int wRemoveMenuFromConstruct(wCONSTRUCT *c, wMENU *m)
{
	// on supprime le menu principal
	int ok = wRemoveEltFromConstruct(c, m);
	if (!ok) return 0;  // il n'est pas dans le construct
	wRemoveEltFromConstruct(c, m->items);
	int x;
	
	// on supprime ses enfants
	for (x=0; x < m->nItems; x++) {
		wMENU *sm = m->items[x];
		if (sm->magicNumber == 1) wRemoveMenuFromConstruct(c, sm);
		else wRemoveEltFromConstruct(c, sm);
	}
	return 1;
}



wCONSTRUCT *wGetConstruct(Widget *w)
{
	if (!w) return NULL;
	return w->construct;
}




// WIDGETS ------------------------------------------------------------------------

void wDrawBackground(Widget *w)
{
	if (!w) return;
	SDL_Surface *scr = w->construct->scr;
	wTHEME *t = w->construct->theme;
	
	if (w->background) {
		wBG_Draw(w->background, &w->bounds);
	}
	
	else {
		Widget *wParent = w;
		do {
			if (!wParent->parent && !wParent->background) break;
			wParent = wParent->parent;
		} while (!wParent->background);
		
		SDL_Rect clip, nClip;
		SDL_GetClipRect(scr, &clip);
		nClip = IntersectRects(&clip, &w->bounds);
		
		SDL_SetClipRect(scr, &nClip);
		
		if (wParent->background)
			wBG_Draw(wParent->background, &wParent->bounds);
		else
			wBG_Draw(t->defbg, &wParent->bounds);
		SDL_SetClipRect(scr, &clip);
	}
}


void wDrawWidget(Widget *w)
{
	if (!w) return;
	
	// 0-- on définit le construct
	if (!w->construct) {
		Widget *p = w->parent;
		if (!p) return;
		
		while (!p->construct) {
			p = p->parent;
			if (!p) return;
		}
		wAddWidgetToConstruct(p->construct, w);
	}
	
	
	if (w == w->construct->widgets[0]) w->construct->onlyOneDynamic = 1;
	
	// 1-- le background
	wDrawBackground(w);
	
	// 2-- le contenu
	if (w->draw) w->draw(w);
	
	// 3-- les bordures
	if (w->displayBounds == 1)
		wDrawBounds(w);
}



/* Cette fonction retourne :
		-2 (ACTION_REFRESH) si on a besoin de réactualiser le construct. Le programme ne quitte pas.
		-1 (ACTION_ERROR) s'il y a eu une erreur. Le programme quitte.
		0 si appui sur ESC ou (ACTION_EXIT) a été appellé. Le programme quitte.
		1 s'il y a eu un appui sur ENTER. Le programme quitte.
		2 s'il y a un déplacement de widget sélectionné. Le programme ne quitte pas. */
int wActivateWidget(Widget *w)
{
	if (!w) return ACTION_ERROR;
	int ok = ACTION_CONTINUE;
		
	if (!w->isDynamic) return ACTION_EXIT;	// si on a un widget statique
	
	
	if (!w->isLayout) {	// si on a un widget dynamique
		ok = wExecCallback(w, SIGNAL_HOVER);
		if (ok != ACTION_CONTINUE) return ok;
		
		wDrawBounds(w);
		w->construct->previousWidgetBounds = w->bounds;
		
		while (!K_ESC()) {
			wait_no_key_pressed();
			
			if (w->activate)
				ok = w->activate(w);
			else {
				wait_key_pressed();
				if (!K_UP() && !K_DOWN() && !K_LEFT() && !K_RIGHT() && !K_MENU() && !K_TAB()
				 && !K_DOC() && !K_SCRATCHPAD()) {
					ok = wExecCallback(w, SIGNAL_KEY);
				}
			}
			
			
			if (ok != ACTION_CONTINUE) return ok;
			
			if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
				 || K_DOC() || K_SCRATCHPAD()) {
				
				if (!w->parent) continue;
				
				ok = wExecCallback(w, SIGNAL_LEAVE);
				if (ok != ACTION_CONTINUE) return ok;
				
				wDrawWidget(w);
				return 2;
			}
			
			if (K_ENTER())
				return 1;
		}
		return 0;
	}
	else	{  // si on a un widget-layout
		if (!w->activate) return ACTION_CONTINUE;
		int ok, ok2;
		
		ok = wExecCallback(w, SIGNAL_HOVER);
		if (ok != ACTION_CONTINUE) return ok;
		
		do {
			ok = w->activate(w);
		} while (!w->parent && ok >= 2);
		
		ok2 = wExecCallback(w, SIGNAL_LEAVE);
		if (ok2 != ACTION_CONTINUE) ok = ok2;
		
		return ok;
	}
}


int wAddWidget(Widget *parent, Widget *child)
{
	return wPlaceWidget(parent, child, -1);
}


int wPlaceWidget(Widget *parent, Widget *child, int spot)
{
	if (!parent || !child || spot < -1) return 0;
	if (!parent->isLayout) return 0;
	if (parent->type == WIDGET_TAB_LAYOUT) return 0;
	if (!parent->add) return 0;
	
	if (!parent->add(parent, child, spot)) return 0;
	
	// on ajoute au parent
	child->parent = parent;
	
	return 1;
}


void wCloseWidget(Widget *w)
{
	if (!w) return;
	
	if (w->background && wIsFreedArg(w, WBACKG)) {
		wAddEltToConstruct(w->construct, w->background);
		wAddSurfaceToConstruct(w->construct, w->background->image);
	}
	if (w->close) w->close(w);
	if (w->args) free(w->args);
	if (w->customArgs) {
		if (w->freeCustomArgs)	w->freeCustomArgs	(w->customArgs);
		else							free					(w->customArgs);
	}
	free(w);
}




// BACKGROUND --------------------------------------------------------------------
wBACKGROUND *wBackgroundColor(Uint32 color1, Uint32 color2, short mode)
{
	wBACKGROUND *bg = malloc(sizeof(wBACKGROUND));
	if (!bg) return NULL;
	bg->color1			= color1;
	bg->color2			= color2;
	bg->image			= NULL;
	bg->mode				= mode;
	return bg;
}

wBACKGROUND *wBackgroundImage(SDL_Surface *img, short mode)
{
	wBACKGROUND *bg = malloc(sizeof(wBACKGROUND));
	if (!bg) return NULL;
	bg->color1			= 0;
	bg->color2			= 0;
	bg->image			= img;
	bg->mode				= mode;
	return bg;
}


void wBG_Draw(wBACKGROUND *bg, SDL_Rect *s)
{
	if (!bg) return;
	SDL_Surface *scr = SDL_GetVideoSurface();
	
	if (bg->mode == BACKG_SOLID)
		DrawFillRect(scr, s, bg->color1);
	
	else if (bg->mode == BACKG_IMG) {
		if (!bg->image) return;
		DrawFillRect(scr, s, bg->color1);
		DrawSurface(bg->image, &(SDL_Rect) {0, 0, min(s->w, bg->image->w), min(s->h, bg->image->h)}, scr, s);
	}
	
	else if (bg->mode == BACKG_HGRAD)
		FillHGradientRect(scr, *s, bg->color1, bg->color2);
	
	else if (bg->mode == BACKG_VGRAD)
		FillVGradientRect(scr, *s, bg->color1, bg->color2);
	
	else if (bg->mode == BACKG_HALO)
		GradientHalo(scr, s, min(s->w, s->h)/2, bg->color1, bg->color2);
	
	else if (bg->mode == BACKG_TRICOLOR) {
		int tmp = s->h;
		s->h /= 2;
		FillHGradientRect(scr, *s, bg->color1, bg->color2);
		s->y += s->h;
		s->h = tmp - s->h;
		FillHGradientRect(scr, *s, bg->color2, bg->color1);
		s->y -= tmp - s->h;
		s->h = tmp;
	}
	
	else if (bg->mode == BACKG_MOSAIC) {
		if (!bg->image) return;
		int x, y;
		SDL_Rect tmp = *s;
		DrawFillRect(scr, s, bg->color1);
		for (y=0; y < (s->h / bg->image->h)+1; y++) {
			for (x=0; x < (s->w / bg->image->w)+1; x++) {
				DrawSurface(bg->image, &(SDL_Rect) {0, 0, min(tmp.w - x*bg->image->w, bg->image->w), min(tmp.h - y*bg->image->h, bg->image->h)}, scr, &tmp);
				tmp.x += bg->image->w;
			}
			tmp.x = s->x;
			tmp.y += bg->image->h;
		}
	}
}

void wBG_SetColor(wBACKGROUND *bg, Uint32 color1, Uint32 color2, short mode)
{
	if (!bg) return;
	bg->color1 = color1;
	bg->color2 = color2;
	bg->mode = mode;
}


void wBG_SetImage(wBACKGROUND *bg, SDL_Surface *img, short mode)
{
	if (!bg) return;
	bg->image = img;
	bg->mode = mode;
}




// AUTRES ------------------------------------------------------------------------


void wSetFreedArgs(Widget *w, int argsType)
{
	if (!w) return;
	w->freeArgsType = argsType;
}


int wIsFreedArg(Widget *w, int argType)
{
	if (!w) return 0;
	return !((w->freeArgsType/argType)%2);
}


void wSetBackground(Widget *w, wBACKGROUND *background)
{
	if (!w) return;
	w->background = background;
}


void wDisplayBounds(Widget *w, BOOL b)
{
	if (!w) return;
	if (w->isLayout || !w->isDynamic)
		w->displayBounds = b;
}

void wDrawBounds(Widget *w)
{
	if (w->displayBounds == -1) return;
	SDL_Surface *scr = SDL_GetVideoSurface();
	wTHEME *theme = w->construct->theme;
	
	DrawRect(scr, &w->bounds, theme->color2);
	DrawRect(scr, &(SDL_Rect) {w->bounds.x+1, w->bounds.y+1, w->bounds.w-2, w->bounds.h-2}, theme->color3);
}


void wSetHeight(Widget *w, int h)
{
	if (!w) return;
	w->Vexpansive = 0;
	w->bounds.h = h;
	w->bounds.y = (SDL_GetVideoSurface()->h - h)/2;
}

void wSetWidth(Widget *w, int width)
{
	if (!w) return;
	w->Hexpansive = 0;
	w->bounds.w = width;
	w->bounds.x = (SDL_GetVideoSurface()->w - width)/2;
}

void wSetSize(Widget *w, int width, int height)
{
	wSetWidth(w, width);
	wSetHeight(w, height);
}

void wSetSpot(Widget *w, int x, int y)
{
	if (!w) return;
	w->bounds.x = x;
	w->bounds.y = y;
}

void wSetPosition(Widget *w, int x, int y, int width, int h)
{
	if (!w) return;
	w->bounds.x = x;
	w->bounds.y = y;
	w->bounds.h = h;
	w->bounds.w = width;
	w->Hexpansive = 0;
	w->Vexpansive = 0;
}

void wSetPositionR(Widget *w, SDL_Rect *r)
{
	if (!w) return;
	w->bounds = *r;
	w->Hexpansive = 0;
	w->Vexpansive = 0;
}

SDL_Rect wGetPosition(Widget *w)
{
	if (!w) return ((SDL_Rect) {0, 0, 0, 0});
	return w->bounds;
}


void wConnect(Widget *w, int (*callBack) (Widget *, int))
{
	if (!w) return;
	w->callBack = callBack;
}

void wSetDynamism(Widget *w, BOOL isDynamic)
{
	if (!w) return;
	// si w est un layout, si isDynamic=1 et si w ne possède pas de widgets-enfants dynamique, cela va crasher
	w->isDynamic = isDynamic;
	if (!w->isLayout && isDynamic) w->displayBounds = 0;
}



int wExecCallback(Widget *w, int signal)
{
	if (!w || (!w->callBack && signal != SIGNAL_KEY)) return ACTION_CONTINUE;
	int r = ACTION_CONTINUE;
	int rp = ACTION_CONTINUE;
	
	if (w->callBack)
		r = w->callBack(w, signal);
	
	if (signal == SIGNAL_KEY) {
		// on appelle les fonctions callbacks des parents
		Widget *wParent = w->parent;
		while (wParent) {
			if (wParent->callBack) {
				rp = wParent->callBack(w, SIGNAL_KEY);
				if (r == ACTION_CONTINUE) r = rp;
				else if (rp == ACTION_EXIT) r = rp;
			}
			wParent = wParent->parent;
		}
	}
	
	// on redessine les bounds
	if (r == ACTION_CONTINUE) {
		if (w->bounds.w > 0 && w->bounds.h > 0 && (w->type != WIDGET_LIST || w->displayBounds != -1)) {
			wDrawWidget(w);
			if (signal != SIGNAL_LEAVE && w->isDynamic && !w->isLayout)
				wDrawBounds(w);
		}
	}
	
	return r;
}

BOOL wIsActivable(Widget *w)
{
	if (!w) return 0;
	if (!w->parent)
		return (w->bounds.x + w->bounds.w < 321 && w->bounds.y + w->bounds.h < 241);
	
	Widget *p = w->parent;
	if (w->bounds.w < 10 || w->bounds.h < 10 || w->bounds.w > 320 || w->bounds.h > 240)
		if (w->type != WIDGET_PIXMAP) return 0;
	if (w->bounds.y < p->bounds.y || w->bounds.x < p->bounds.x)
		return 0;
	if (w->bounds.y + w->bounds.h > p->bounds.y + p->bounds.h)
		return 0;
	if (w->bounds.x + w->bounds.w > p->bounds.x + p->bounds.w)
		return 0;
	return 1;
}

void wFindBounds(Widget *w, SDL_Rect *r, BOOL Hcenter, BOOL Vcenter)
{
	if (!w) return;
	if (w->Hexpansive == 1 || (w->Hexpansive == 2 && w->bounds.w > r->w)) {
		w->bounds.x = r->x;
		w->bounds.w = r->w;
	}
	else if (Hcenter)
		w->bounds.x = r->x + (r->w - w->bounds.w)/2;
	else
		w->bounds.x = r->x;
	if (w->Hexpansive == 2 && w->bounds.w > r->w)
		w->bounds.w = r->w;
	
	if (w->Vexpansive == 1 || (w->Vexpansive == 2 && w->bounds.h > r->h)) {
		w->bounds.y = r->y;
		w->bounds.h = r->h;
	}
	else if (Vcenter)
		w->bounds.y = r->y + (r->h - w->bounds.h)/2;
	else
		w->bounds.y = r->y;
}


int wFindNextWidget(Widget **widgets, int nWidgets, int key, SDL_Rect previousWidgetBounds)
{
	if (!widgets) return -1;
	Widget *w = NULL;
	int chosenOne = -1;
		
	int n;
	int fitting=999, wFitting=0, distance=999, wDistance=0, length=0;
	
	SDL_Rect r = previousWidgetBounds;
	int M;
	int x=0, rw=0, rx=0, wd=0;
	
	
	
	for (n=0; n<nWidgets; n++) {
		w = widgets[n];
		if (!w->isDynamic || !wIsActivable(w)) continue;
		if (w->bounds.x <= r.x && w->bounds.x+w->bounds.w >= r.x+r.w &&
			 w->bounds.y <= r.y && w->bounds.y+w->bounds.h >= r.y+r.h) continue;  // si c'est un de ses parents
		
		if (key == ARROW_BOT) { // BOT
			x = w->bounds.x;
			wd = w->bounds.w;
			rx = r.x;
			rw = r.w;
			wDistance = w->bounds.y - r.y - r.h;
		}
		else if (key == ARROW_TOP) {  // TOP
			x = w->bounds.x;
			wd = w->bounds.w;
			rx = r.x;
			rw = r.w;
			wDistance = r.y - w->bounds.y - w->bounds.h;
		}
		else if (key == ARROW_LEFT) {  // LEFT
			x = w->bounds.y;
			wd = w->bounds.h;
			rx = r.y;
			rw = r.h;
			wDistance = r.x - w->bounds.x - w->bounds.w;
		}
		else if (key == ARROW_RIGHT) {  // RIGHT
			x = w->bounds.y;
			wd = w->bounds.h;
			rx = r.y;
			rw = r.h;
			wDistance = w->bounds.x - r.x - r.w;
		}
		
		// Calcul du Fitting
		M = 2*rx + rw;
		if (x <= rx && x+wd >= rx+rw) wFitting = 0;
		else {
			wFitting = abs(2*(x+wd) - M);
			if (wFitting > 0) {
				wFitting -= rw/20;
				if (wFitting < 0) wFitting = 0;
			}
			if (wFitting > abs(2*x - M)) wFitting = abs(2*x - M);
		}
		
// DEBUG
// SDL_FillRect(w->construct->scr, &(SDL_Rect) {0, 0, 320, 27}, RGB(200,200,200));
// nSDL_DrawString(w->construct->scr, w->construct->theme->font, 0, 0, "r= %i,%i,%i,%i,  w= %i,%i,%i,%i\nfitting= %i(%i), distance= %i(%i), key= %i\nx= %i, wd= %i, rx= %i, rw= %i",
																							// r.x, r.y, r.w, r.h, 
																							// w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h,
																							// wFitting, fitting, wDistance, distance, key,
																							// x, wd, rx, rw);
// DrawRect(w->construct->scr, &w->bounds, RGB(50, 255, 50));
// SDL_Flip(w->construct->scr);
// wait_key_pressed();
////////
		
		// Comparaison
		if (wFitting > rw && fitting <= rw) continue;  // si le widget ne "fit" pas
		if (wDistance <= 0) continue;  // si la distance est négative
		if (distance+3 < wDistance) continue;  // si le widget est plus loin. 3 pixels de marge
		
		if (distance <= wDistance+3) {	// s'ils sont à la même distance on trouve autre chose pour les départager
			if (fitting < wFitting) continue;  // si le widget "fit" moins bien même s'il est à la même distance
			if (fitting < wFitting+3 && wd-5 < length) continue;	// si les fittings sont équivalents (marge de 3), on choisit le widget clairement plus grand.
		}
		
		// on obtient un widget qui "fit", plus proche ou de même distance mais qui alors "fit" mieux
		distance = wDistance;
		fitting = wFitting;
		chosenOne = n;
		length = wd;
	}
	

// DEBUG
// if (w && nWidgets && chosenOne > -1) {
	// w = widgets[chosenOne];
	// SDL_FillRect(w->construct->scr, &(SDL_Rect) {0, 0, 320, 27}, RGB(200,200,200));
	// nSDL_DrawString(w->construct->scr, w->construct->theme->font, 0, 0, "r= %i,%i,%i,%i,  \nw= %i,%i,%i,%i\nfitting= %i, distance= %i, key= %i",
																							// r.x, r.y, r.w, r.h, 
																							// w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h,
																							// fitting, distance, key);

	// if (w) DrawRect(w->construct->scr, &w->bounds, RGB(255,50,50));
	// SDL_Flip(w->construct->scr);
	// wait_key_pressed();
// }
////////////////
	
	return chosenOne;
}


BOOL wHasDialogParent(Widget *w)
{
	Widget *p = w->parent;
	
	while (p) {
		if (p->type == WIDGET_DIALOG) return 1;
		p = p->parent;
	}
	
	return 0;
}


int wLayout_GetNumberOfChilds(Widget *w)
{
	if (!w || !w->isLayout || !w->args) return -1;
	return ((LAYOUT *) w->args)->nWidgets;
}


Widget **wLayout_GetChildsList(Widget *w)
{
	if (!w || !w->isLayout || !w->args || w->type == WIDGET_WINDOW || w->type == WIDGET_DIALOG) return NULL;
	return ((LAYOUT *) w->args)->widgets;
}


Widget **wLayout_GetChilds(Widget *w)  // pareil que la fonction ci-dessus
{
	if (!w || !w->isLayout || !w->args || w->type == WIDGET_WINDOW || w->type == WIDGET_DIALOG) return NULL;
	return ((LAYOUT *) w->args)->widgets;
}


Widget *wLayout_GetChild(Widget *w, int n)
{
	if (!w || !w->isLayout || !w->args || w->type == WIDGET_WINDOW || w->type == WIDGET_DIALOG) return NULL;
	return (((LAYOUT *) w->args)->widgets)[n];
}


int wLayout_GetCurrentChild(Widget *w)
{
	if (!w || !w->isLayout || !w->args || w->type == WIDGET_WINDOW || w->type == WIDGET_DIALOG) return -1;
	return ((LAYOUT *) w->args)->cWidget;
}



void wSetActiveWidget(Widget *w)
{
	if (!w || !w->isDynamic) return;
	
	// on définit le widget actif de tous les layouts-parents
	Widget *child = w, *parent = NULL;
	int n = 0;
	LAYOUT *pArgs = NULL;
	
	while (child) {
		parent = child->parent;
		if (!parent) return;
		
		if (parent->type != WIDGET_WINDOW && parent->type != WIDGET_DIALOG) {
			pArgs = parent->args;
			
			for (n=0; n < pArgs->nWidgets; n++) {
				if (pArgs->widgets[n] == child) {
					pArgs->cWidget = n;
					break;
				}
			}
		}
		
		child = parent;
	}
	
}


Widget *wGetActiveWidget(Widget *w)
{
	if (!w) return NULL;
	LAYOUT *bargs;
	
	
	while (w->isLayout) {
		if (w->type == WIDGET_WINDOW || w->type == WIDGET_DIALOG)
			w = ((WLAYOUT *) w->args)->widget;
		
		else {
			bargs = w->args;
			w = bargs->widgets[bargs->cWidget];
		}
	}
	return w;
}


void wSetCustomArgs(Widget *w, void *customArgs, void (*freeCustomArgs) (void *))
{
	if (!w) return;
	w->customArgs = customArgs;
	w->freeCustomArgs = freeCustomArgs;
}


// CETTE FONCTION N'EST A UTILISER QU'A L'INTERIEUR
// D'UNE FONCTION CALLBACK !!!!!!
// Utiliser ACTION_REFRESH après appel de cette fonction
void wRemoveWidget(Widget *w)
{
	Widget *p = w->parent;
	if (!p) return;
	
	// On ajoute au construct (au cas où)
	wAddWidgetToConstruct(w->construct, w);
	
	// on supprime du parent
	if (p->type == WIDGET_WINDOW || p->type == WIDGET_DIALOG) {
		((WLAYOUT *) p->args)->widget = NULL;
	}
	
	else {
		LAYOUT *args = p->args;
		if (!args->nWidgets) return;	// il s'est passé quelque chose d'anormal !!
		
		args->nWidgets--;
		if (!args->nWidgets) {
			free(args->widgets);
			args->widgets = NULL;
		}
		else {
			int x=0;
			while (args->widgets[x] != w) x++;
			while (x < args->nWidgets) {
				args->widgets[x] = args->widgets[x+1];
				x++;
			}
			
			// on vérifie que le widget actif existe encore
			if (args->cWidget >= args->nWidgets)
				args->cWidget = args->nWidgets-1;
		}
	}
}


// -- AUTRES ---------------------------------------------------------------------------
SDL_Surface *wInitSDL(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	return SDL_SetVideoMode(320, 240, has_colors? 16:8, SDL_SWSURFACE);
}


