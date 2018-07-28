#include "structures.h"
#include "TabLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"

// Création
Widget *wTab()
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_TAB_LAYOUT;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = 0;
	w->background = NULL;
	w->isDynamic = 1;
	w->isLayout = 1;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 160;
	w->bounds.h = 190;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawTabLayout;
	w->activate	= ActivateTabLayout;
	w->close		= CloseTabLayout;
	w->add		= NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(TabLayoutArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	TabLayoutArgs *args = w->args;
	args->nWidgets	= 0;
	args->widgets	= NULL;
	args->cWidget = 0;
	args->hWidget = -1;
	args->tabs = NULL;
	args->nTabs = 0;
	
	return w;
}


// Ajout d'onglets
int wTab_AddTab(Widget *w, const char *text, Widget *child)
{
	return wTab_AddExTab(w, text, child, 0);
}


int wTab_AddExTab(Widget *w, const char *text, Widget *child, BOOL canBeDeleted)
{
	return wTab_SetExTab(w, text, child, canBeDeleted, -1);
}


int wTab_SetTab(Widget *w, const char *text, Widget *child, int spot)
{
	return wTab_SetExTab(w, text, child, 0, spot);
}


int wTab_SetExTab(Widget *w, const char *text, Widget *child, BOOL canBeDeleted, int spot)
{
	if (!w || !child) return 0;
	if (w->type != WIDGET_TAB_LAYOUT) return 0;
	TabLayoutArgs *args = w->args;
	void *tmp;
	
	if (spot > args->nWidgets || spot == -1)
		spot = args->nWidgets;
	
	
	if (spot == args->nWidgets) {
		tmp = realloc(args->widgets, (args->nWidgets+1)*sizeof(Widget *));
		if (!tmp) return 0;
		args->widgets = tmp;
		
		tmp = realloc(args->tabs, (args->nWidgets+1)*sizeof(struct TAB));
		if (!tmp) return 0;
		args->tabs = tmp;
		
		args->nWidgets++;
		args->tabs[spot].text = NULL;
	}
	else
		wAddWidgetToConstruct(w->construct, args->widgets[spot]);

	
	
	args->tabs[spot].text			= realloc(args->tabs[spot].text, strlen(text)+1);
	args->tabs[spot].canBeDeleted	= canBeDeleted;
	args->widgets[spot]				= child;
	child->parent						= w;
	strcpy(args->tabs[spot].text, text);
	
	return 1;
}


// Dessin
void DrawTabs(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	TabLayoutArgs *args = w->args;
	args->nTabs = args->nWidgets;
	int x;
	int lx;
	int px = 1;
	Uint32 c1 = Darker(theme->color1, 50);
	Uint32 c2 = Darker(theme->color1, 60);
	Uint32 c3 = Darker(theme->color1, 25);
	struct TAB *tab;
	nSDL_Font *f1 = theme->tab_f1, *f2 = theme->tab_f2;
	SDL_Rect saveBounds = w->bounds;
	
	// Dessin du background
	w->bounds = (SDL_Rect) {w->bounds.x, w->bounds.y+1, w->bounds.w, 9};
	wDrawBackground(w);
	w->bounds = saveBounds;
	
	for (x=0; x < args->nWidgets; x++) {
		tab = &args->tabs[x];
		
		lx = nSDL_GetStringWidth(f1, tab->text);
		if (tab->canBeDeleted) lx += 7;
		if (px + lx + 4 > w->bounds.w) {
			args->nTabs = x;
			if (x && args->cWidget >= x) args->cWidget = x-1;
			if (!x) w->isDynamic = 0;
			break;
		}
		
		if (x == args->hWidget && K_SCRATCHPAD()) {
			DrawFillRectXY(scr, w->bounds.x + px, w->bounds.y+1, lx + 4, 10, theme->tab_color4);
			DrawClippedStr(scr, f2, w->bounds.x + px + 1, w->bounds.y + 2, tab->text);
		}
		else if (x == args->cWidget) {
			DrawFillRectXY(scr, w->bounds.x + px, w->bounds.y+1, lx + 4, 10, theme->tab_color3);
			DrawClippedStr(scr, f2, w->bounds.x + px + 1, w->bounds.y + 2, tab->text);
		}
		else {
			DrawFillRectXY(scr, w->bounds.x + px, w->bounds.y+1, lx + 4, 10, theme->tab_color2);
			DrawClippedStr(scr, f1, w->bounds.x + px + 1, w->bounds.y + 2, tab->text);
		}
		DrawRectXY(scr, w->bounds.x + px, w->bounds.y+1, lx + 4, 10, c1);
		DrawRectXY(scr, w->bounds.x + px + lx + 4, w->bounds.y + 2, 1, 9, c2);
		
		if (tab->canBeDeleted)
			DrawSurface(theme->tab_delete, NULL, scr,
			 &((SDL_Rect) {w->bounds.x+px+lx-4, w->bounds.y+3, 7, 7}));
		
		tab->px = w->bounds.x + px;
		px += lx + 7;
	}

	// Dessin de la bordure
	DrawRectXY(scr, w->bounds.x, w->bounds.y + 10, w->bounds.w-1, w->bounds.h - 11, c1);
	DrawRectXY(scr, w->bounds.x + w->bounds.w - 1, w->bounds.y+11, 1, w->bounds.h-11, c2);
	DrawRectXY(scr, w->bounds.x + 1, w->bounds.y+w->bounds.h-1, w->bounds.w-1, 1, c2);
	
	if (args->nTabs) {
		tab = &args->tabs[args->cWidget];
		lx = nSDL_GetStringWidth(f1, tab->text);
		if (tab->canBeDeleted) lx += 7;
		DrawRectXY(scr, tab->px + 1, w->bounds.y + 10, lx + 2, 1, c3);
	}
}


void DrawTabLayout(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	TabLayoutArgs *args = w->args;
	Uint32 c1 = Darker(theme->color1, 50);
	Uint32 c2 = Darker(theme->color1, 60);
	BOOL vcenter = 0;
	
	if (!args->nWidgets || w->bounds.w < 20 || w->bounds.h < 16) {
		if (w->background) {
			DrawRect(scr, &w->bounds, c1);
			DrawRectXY(scr, w->bounds.x + w->bounds.w, w->bounds.y + 1, 1,
							w->bounds.h, c2);
			DrawRectXY(scr, w->bounds.x + 1, w->bounds.y + w->bounds.h,
							w->bounds.w, 1, c2);
		}
		else {
			DrawRect(scr, &w->bounds, c1);
			DrawRectXY(scr, w->bounds.x + w->bounds.w, w->bounds.y + 1, 1,
							w->bounds.h, c2);
			DrawRectXY(scr, w->bounds.x + 1, w->bounds.y + w->bounds.h,
							w->bounds.w, 1, c2);
		}
		w->isDynamic = 0;
		return;
	}
	
	Widget *child;
	w->isDynamic = 1;
	
	// Dessin des onglets
	DrawTabs(w);	
	
	// Dessin du widget actif
	if (args->nTabs) { 	// si au moins un onglet est affichable
		
		child = args->widgets[args->cWidget];
		if (child) {
			if (child->type == WIDGET_PIXMAP)
				vcenter = 1;
			
			if (child->isLayout) {
				SDL_Rect r = (SDL_Rect) {w->bounds.x+1, w->bounds.y+11, w->bounds.w-3, w->bounds.h-13};
				wFindBounds(child, &r, 1, vcenter);
				child->displayBounds = 0;
				w->bounds.x-=2, w->bounds.y-=2, w->bounds.w+=3, w->bounds.h+=3;
			}
			
			else {
				SDL_Rect r = (SDL_Rect) {w->bounds.x+2, w->bounds.y+12, w->bounds.w-5, w->bounds.h-15};
				wFindBounds(child, &r, 1, vcenter);
				child->displayBounds = 0;
				w->bounds.x-=1, w->bounds.y-=1, w->bounds.w+=1, w->bounds.h+=1;
			}
			vcenter = 0;
			
			if (wIsActivable(child))
				wDrawWidget(child);
			
			if (child->isLayout)
				w->bounds.x+=2, w->bounds.y+=2, w->bounds.w-=3, w->bounds.h-=3;
			else
				w->bounds.x+=1, w->bounds.y+=1, w->bounds.w-=1, w->bounds.h-=1;
		}
	}
}


// Activation
int ActivateTabLayout(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	TabLayoutArgs *args = w->args;
	if (!args->nWidgets || !args->widgets) return 0;
	int ok;
	int x;
	Widget *child = args->widgets[args->cWidget];
	BOOL onlyScratch = 1;
	if (!child->isDynamic) SDL_Flip(scr);
	
	
	do {
		ok = wActivateWidget(child);
		if (child->isDynamic && ok < 2) return ok;
		onlyScratch = 1;
		args->hWidget = args->cWidget;
		
		if (!child->isDynamic) {
			wait_key_pressed();
			while (K_CTRL()) {
				if (K_SCRATCHPAD()) break;
			}
			if (!K_SCRATCHPAD()) {
				if (w->parent && (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()))
					return 2;
				if (K_ESC()) return 0;
				if (K_ENTER()) return 1;
			}
		}
		
		if (K_SCRATCHPAD() || K_VAR()) {
			args->hWidget = args->cWidget;
			while (K_SCRATCHPAD() || K_VAR()) {
				
				if (K_LEFT()) {
					args->hWidget--;
					if (args->hWidget < 0) args->hWidget = args->nTabs - 1;
					DrawTabs(w);
					SDL_Flip(scr);
					onlyScratch = 0;
					while(K_LEFT());
				}
				
				if (K_RIGHT()) {
					args->hWidget = (args->hWidget+1)%args->nTabs;
					DrawTabs(w);
					SDL_Flip(scr);
					onlyScratch = 0;
					while(K_RIGHT());
				}
								
				if (K_DEL()) {
					onlyScratch = 0;
					if (!args->tabs[args->hWidget].canBeDeleted) {
						while(K_DEL());
						continue;
					}
										
					
					if (args->nWidgets == 1) {
						free(args->widgets);
						args->widgets = NULL;
						free(args->tabs[0].text);
						free(args->tabs);
						args->tabs = NULL;
						args->widgets = NULL;
						w->isDynamic = 0;
						args->nWidgets = 0;
						args->nTabs = 0;
						if ((ok=wExecCallback(w, SIGNAL_ACTION2)) != ACTION_CONTINUE)
							return ok;  // SIGNAL_ACTION2 = delete a tab
						wDrawWidget(w);
						return 2;
					}
					
					// on supprime l'onglet
					free(args->tabs[args->hWidget].text);
					args->tabs[args->hWidget].text = NULL;
					wAddWidgetToConstruct(w->construct, args->widgets[args->hWidget]);
					
					for (x=args->hWidget; x < args->nWidgets-1; x++) {
						args->widgets[x] = args->widgets[x+1];
						args->tabs[x] = args->tabs[x+1];
					}
					args->nWidgets--;
					
					// pas  besoin de vérifier les realloc car tailles plus petites
					args->widgets = realloc(args->widgets, args->nWidgets * sizeof(Widget*));
					args->tabs = realloc(args->tabs, args->nWidgets * sizeof(struct TAB));
					
					// on redessine
					if (args->hWidget == args->cWidget) {  // si on a supprimé le tab courant
						if ((ok=wExecCallback(w, SIGNAL_ACTION2)) != ACTION_CONTINUE) {
							args->hWidget = -1;
							return ok;  // SIGNAL_ACTION2 = delete a tab
						}
						
						if (args->cWidget >= args->nWidgets)
							args->cWidget = args->nWidgets - 1;
						args->hWidget = args->cWidget;
						
						if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
							return ok;  // SIGNAL_ACTION = move current tab
						DrawTabLayout(w);
					}
					else {  // si on a supprimé un autre tab
						if ((ok=wExecCallback(w, SIGNAL_ACTION2)) != ACTION_CONTINUE) {
							args->hWidget = -1;
							return ok;  // SIGNAL_ACTION2 = delete a tab
						}
						if (args->hWidget >= args->nWidgets)
							args->hWidget = args->nWidgets - 1;
						if (args->cWidget > args->hWidget)
							args->cWidget--;
						DrawTabLayout(w);
					}
					SDL_Flip(scr);
					child = args->widgets[args->cWidget];
					
					while (K_DEL());
				}
			}
			
			if (onlyScratch) {  // alors on passe au tab suivant
				args->cWidget = (args->cWidget+1)%args->nTabs;
				child = args->widgets[args->cWidget];
				args->hWidget = -1;
				if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
					return ok;
				wDrawWidget(w);
				SDL_Flip(scr);
			}
			
			else if (args->hWidget == args->cWidget || args->hWidget == -1) {  // si on n'a pas changé d'onglet
				args->hWidget = -1;
				DrawTabs(w);
				SDL_Flip(scr);
			}
			
			else {  // si on a changé d'onglet
				args->cWidget = args->hWidget;
				child = args->widgets[args->cWidget];
				args->hWidget = -1;
				if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
					return ok;
				wDrawWidget(w);
				SDL_Flip(scr);
			}
		}
	
	} while (ok == 3);
	return 2;
}


// Fermeture
void CloseTabLayout(Widget *w)
{
	if (!w) return;
	TabLayoutArgs *args = w->args;
	int x;
	
	if (args) {
		if (args->widgets) {
			for (x=0; x < args->nWidgets; x++)
				wAddWidgetToConstruct(w->construct, args->widgets[x]);
			free(args->widgets);
		}
		
		if (args->tabs) {
			for (x=0; x<args->nWidgets; x++)
				free(args->tabs[x].text);
			free(args->tabs);
		}
	}
}


// Méthodes
Widget *wTab_GetCurrentWidget(Widget *tab)
{
	if (!tab) return NULL;
	TabLayoutArgs *args = tab->args;
	if (!args->nWidgets || !args->widgets) return NULL;
	return args->widgets[args->cWidget];
}


int wTab_GetCurrentTab(Widget *tab)
{
	if (!tab) return -1;
	TabLayoutArgs *args = tab->args;
	if (!args->nWidgets || !args->widgets) return -1;
	return args->cWidget;
}


char *wTab_GetCurrentTabTitle(Widget *tab)
{
	if (!tab) return NULL;
	TabLayoutArgs *args = tab->args;
	if (!args->nWidgets || !args->tabs) return NULL;
	return args->tabs[args->cWidget].text;
}

void wTab_SetTabTitle(Widget *tab, int ntab, const char *title)
{
	if (!tab || !title || ntab < 0) return;
	TabLayoutArgs *args = tab->args;
	if (ntab >= args->nWidgets) return;
	
	void *tmp = realloc(args->tabs[ntab].text, strlen(title)+1);
	if (!tmp) return;
	strcpy(tmp, title);
	args->tabs[ntab].text = tmp;
}

