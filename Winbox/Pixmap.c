#include "structures.h"
#include "common.h"
#include "keys.h"
#include "scrollbar.h"
#include "Pixmap.h"
#include "widgets.h"
#include "general.h"

void DrawScrollBars(Widget *w);


// Création
Widget *CreatePixmap(void)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_PIXMAP;
	w->displayBounds = 0;
	w->Hexpansive = 2;
	w->Vexpansive = 2;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->draw	= DrawPixmap;
	w->activate	= ActivatePixmap;
	w->close		= ClosePixmap;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;

	w->args = malloc(sizeof(PixmapArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	PixmapArgs *args = w->args;
	args->xpos = 0;
	args->ypos = 0;
	
	return w;
}

Widget *wPixmapBMP(const char *file)
{
	return wExPixmapBMP(file, 0, 255);
}

Widget *wPixmapNTI(Uint16 *data)
{
	return wExPixmapNTI(data, 0, 255);
}

Widget *wPixmapSurface(SDL_Surface *img)
{
	Widget *w = CreatePixmap();
	if (!w) return NULL;
	PixmapArgs *args = w->args;
	
	w->bounds.w = img->w;
	w->bounds.h = img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;

	args->img = img;
	args->colorKey = 0;
	args->alpha = 255;
	
	return w;
}



Widget *wExPixmapBMP(const char *file, Uint32 colorKey, Uint8 alpha)
{
	if (!file) return NULL;
	SDL_Surface *img = SDL_LoadBMP(file);
	if (!img) return NULL;
	Widget *w = CreatePixmap();
	if (!w) return NULL;
	
	w->bounds.w = img->w;
	w->bounds.h = img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	PixmapArgs *args = w->args;
	args->img = img;
	args->colorKey = colorKey;
	if (colorKey)
		SDL_SetColorKey(args->img, SDL_SRCCOLORKEY, colorKey);
	args->alpha = alpha;
	if (args->alpha < 255)
		SDL_SetAlpha(args->img, SDL_SRCALPHA, args->alpha);
	
	return w;
}

Widget *wExPixmapNTI(Uint16 *data, Uint32 colorKey, Uint8 alpha)
{
	if (!data) return NULL;
	SDL_Surface *img = nSDL_LoadImage(data);
	if (!img) return NULL;
	Widget *w = CreatePixmap();
	if (!w) return NULL;
	
	w->bounds.w = img->w;
	w->bounds.h = img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	PixmapArgs *args = w->args;
	args->img = img;
	args->colorKey = colorKey;
	if (colorKey)
		SDL_SetColorKey(args->img, SDL_SRCCOLORKEY, colorKey);
	args->alpha = alpha;
	if (args->alpha < 255)
		SDL_SetAlpha(args->img, SDL_SRCALPHA, args->alpha);
	
	return w;
}


// Dessin
void DrawPixmap(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	PixmapArgs *args = w->args;
	
	DrawSurface(args->img, &((SDL_Rect) {args->xpos, args->ypos, w->bounds.w, w->bounds.h}), scr, &w->bounds);
	
	if (args->img->w > w->bounds.w  ||  args->img->h > w->bounds.h)
		w->isDynamic = 1;
}



// Activation
int ActivatePixmap(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	PixmapArgs *args = w->args;
	BOOL vscroll = args->img->h > w->bounds.h;
	BOOL hscroll = args->img->w > w->bounds.w;
	BOOL ood = w->construct->onlyOneDynamic;
	SDL_Surface *s_im;
//	SDL_Rect r = {args->xpos+2, args->ypos+2, w->bounds.w-4, w->bounds.h-4};	
	SDL_Rect in = {w->bounds.x+2, w->bounds.y+2, w->bounds.w-4, w->bounds.h-4};
	BOOL draw = 0;
	int ok = ACTION_CONTINUE;
	int dx = min(3, args->img->w / 10);
	int dy = min(3, args->img->h / 10);
	
	
	// on affiche les scrollbars en transparent
	s_im = SDL_CreateRGBSurface(SDL_SWSURFACE, in.w, in.h, 16, 0,0,0,0);
	DrawSurface(scr, &in, s_im, NULL);
	SDL_SetAlpha(s_im, SDL_SRCALPHA, 128);

	DrawScrollBars(w);
	DrawSurface(s_im, NULL, scr, &in);
	SDL_Flip(scr);
	if (ood && (vscroll || hscroll)) goto INLOOP;

	// début de la boucle
	while (!K_ESC()) {
		wait_key_pressed();
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB() || K_SCRATCHPAD() || K_ENTER() || K_ESC())
			break;

		if (K_CLICK() && (vscroll || hscroll)) {
		  INLOOP:
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				break;
			DrawScrollBars(w);
			SDL_Flip(scr);
			while (K_CLICK());
			
			while (!K_TAB() && !K_SCRATCHPAD() && !K_MENU()) {
				
				if (K_CLICK() || K_ESC() || K_ENTER()) {
					if (ood) goto QUIT;
					else break;
				}
				
				if (K_UP() && vscroll && args->ypos) {
					if (args->ypos < dy) args->ypos = 0;
					else args->ypos -= dy;
					draw = 1;
				}
				
				if (K_DOWN() && vscroll && args->ypos + w->bounds.h < args->img->h) {
					args->ypos += dy;
					if (args->ypos + w->bounds.h > args->img->h)
						args->ypos = args->img->h - w->bounds.h;
					draw = 1;
				}

				if (K_LEFT() && hscroll && args->xpos) {
					args->xpos-=dx;
					if (args->xpos < 0) args->xpos = 0;
					draw = 1;
				}

				if (K_RIGHT() && hscroll && args->xpos + w->bounds.w < args->img->w) {
					args->xpos += dx;
					if (args->xpos + w->bounds.w > args->img->w)
						args->xpos = args->img->w - w->bounds.w;
					draw = 1;
				}
				
				if (!draw && any_key_pressed()) {
					if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
						goto QUIT;
					draw = 1;
				}
				
				if (draw) {
					DrawFillRect(scr, &in, theme->color1);
					DrawSurface(args->img, &((SDL_Rect) {args->xpos+2, args->ypos+2, in.w, in.h}), scr, &in);
					DrawScrollBars(w);
					SDL_Flip(scr);
					draw = 0;
				}
			}
			if (ood) goto QUIT;
			
			DrawSurface(args->img, &((SDL_Rect) {args->xpos+2, args->ypos+2, in.w, in.h}), s_im, NULL);
			DrawSurface(s_im, NULL, scr, &in);
			SDL_Flip(scr);
			
			if (K_CLICK() || K_ESC() || K_ENTER())
				wait_no_key_pressed();
		}
		
		else if (any_key_pressed()) {
			if ((ok=wExecCallback(w, K_CLICK()? SIGNAL_CLICK : SIGNAL_KEY)) != ACTION_CONTINUE)
				break;
			DrawScrollBars(w);
			SDL_Flip(scr);
			wait_no_key_pressed();
		}
	}
	
	if (K_ENTER() && !wHasDialogParent(w)) ok = wExecCallback(w, SIGNAL_CLICK);
	
	// on supprime les scrollbars
  QUIT:
	SDL_FreeSurface(s_im);
	return ok;
}


// Fermeture
void ClosePixmap(Widget *w)
{
	if (!w) return;
	if (w->args) {
		PixmapArgs *args = w->args;
		if (wIsFreedArg(w, WSURFACE)) SDL_FreeSurface(args->img);
	}
}




// Autres
void DrawScrollBars(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	PixmapArgs *args = w->args;
	BOOL vscroll = args->img->h > w->bounds.h;
	BOOL hscroll = args->img->w > w->bounds.w;
	
	if (vscroll && hscroll) {
		w->bounds.h -= 7;
		DrawVScrollBar(scr, theme, &w->bounds, args->img->h-7, args->ypos);
		w->bounds.h += 7;
		w->bounds.w -= 7;
		DrawHScrollBar(scr, theme, &w->bounds, args->img->w-7, args->xpos);
		w->bounds.w += 7;
	}
	else if (vscroll)
		DrawVScrollBar(scr, theme, &w->bounds, args->img->h, args->ypos);
	else if (hscroll)
		DrawHScrollBar(scr, theme, &w->bounds, args->img->w, args->xpos);
		
}





// Méthodes
void wPixmap_SetImageBMP(Widget *w, const char *file)
{
	if (!w || !file) return;
	if (w->type != WIDGET_PIXMAP) return;
	PixmapArgs *args = w->args;
	
	void *tmp = SDL_LoadBMP(file);
	if (!tmp) return;
	if (wIsFreedArg(w, WSURFACE))
		SDL_FreeSurface(args->img);
	args->img = tmp;
	args->xpos = 0;
	args->ypos = 0;
	w->bounds.w = args->img->w;
	w->bounds.h = args->img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	if (args->colorKey)
		SDL_SetColorKey(args->img, SDL_SRCCOLORKEY, args->colorKey);
	if (args->alpha < 255)
		SDL_SetAlpha(args->img, SDL_SRCALPHA, args->alpha);
}


void wPixmap_SetImageNTI(Widget *w, Uint16 *data)
{
	if (!w || !data) return;
	if (w->type != WIDGET_PIXMAP) return;
	PixmapArgs *args = w->args;
	
	void *tmp = nSDL_LoadImage(data);
	if (!tmp) return;
	if (wIsFreedArg(w, WSURFACE))
		SDL_FreeSurface(args->img);
	args->img = tmp;
	args->xpos = 0;
	args->ypos = 0;
	w->bounds.w = args->img->w;
	w->bounds.h = args->img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	if (args->colorKey)
		SDL_SetColorKey(args->img, SDL_SRCCOLORKEY, args->colorKey);
	if (args->alpha < 255)
		SDL_SetAlpha(args->img, SDL_SRCALPHA, args->alpha);
}


void wPixmap_SetImageSurface(Widget *w, SDL_Surface *img)
{
	if (!w || !img) return;
	if (w->type != WIDGET_PIXMAP) return;
	PixmapArgs *args = w->args;
	
	if (wIsFreedArg(w, WSURFACE))
		SDL_FreeSurface(args->img);
	args->img = img;
	args->xpos = 0;
	args->ypos = 0;
	args->colorKey = 0;
	args->alpha = 255;
	w->bounds.w = args->img->w;
	w->bounds.h = args->img->h;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
}


void wPixmap_SetColorKey(Widget *w, Uint32 colorKey)
{
	if (!w) return;
	if (w->type != WIDGET_PIXMAP) return;
	PixmapArgs *args = w->args;
	
	args->colorKey = colorKey;
	if (colorKey) 
		SDL_SetColorKey(args->img, SDL_SRCCOLORKEY, colorKey);
	else
		SDL_SetColorKey(args->img, 0, 0);
}


void wPixmap_SetAlpha(Widget *w, Uint8 alpha)
{
	if (!w) return;
	if (w->type != WIDGET_PIXMAP) return;
	PixmapArgs *args = w->args;
	
	args->alpha = alpha;
	SDL_SetColorKey(args->img, SDL_SRCALPHA, alpha);
}

















