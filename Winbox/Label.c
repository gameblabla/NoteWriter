#include "structures.h"
#include "Label.h"
#include "general.h"
#include "widgets.h"



// Création
Widget *wLabel(const char *text, int alignment)
{
	return wExLabel(text, alignment, NULL);
}


Widget *wExLabel(const char *text, int alignment, nSDL_Font *font)
{
	Widget *w = malloc(sizeof(Widget));
	w->type = WIDGET_LABEL;
	w->displayBounds = 0;
	w->Hexpansive = 1;
	w->Vexpansive = 0;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;
	

	if (font)	w->bounds.w = 6 + nSDL_GetStringWidth(font, text);
	else 			w->bounds.w = 6 + 8*strlen(text);
	w->bounds.h = 14;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw	= DrawLabel;
	w->activate	= NULL;
	w->close		= CloseLabel;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(LabelArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	LabelArgs *args = w->args;
	args->text		= malloc(strlen(text)+1);
	strcpy(args->text, text);
	args->font		= font;
	args->alignment	= alignment;
	
	return w;
}






// Dessin
void DrawLabel(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	LabelArgs *args = w->args;
	int x = w->bounds.x + 3, y = w->bounds.y + 3,
		width = nSDL_GetStringWidth(args->font? args->font : theme->font, args->text),
		height = nSDL_GetStringHeight(args->font? args->font : theme->font, args->text);
	
	
	// on calcule l'alignement
	if 		(args->alignment%4 == 1)
		x = w->bounds.x + (w->bounds.w - width)/2;
	else if	(args->alignment%4 == 2)
		x = w->bounds.x + w->bounds.w - 3 - width;
	
	if 		(args->alignment/4 == 1)
		y = w->bounds.y + (w->bounds.h - height)/2;
	else if	(args->alignment/4 == 2)
		y = w->bounds.y + w->bounds.h - 3 - height;
	
	// on dessine
	DrawClippedStr(scr, args->font? args->font : theme->font, x, y, args->text);
}


// Activation


// Fermeture
void CloseLabel(Widget *w)
{
	if (!w) return;
	LabelArgs *args = w->args;
	free(args->text);
	if (wIsFreedArg(w, WFONT)) wAddFontToConstruct(w->construct, args->font);
}


// Ajout de widgets-enfants





// Setters & Getters

void wLabel_SetText(Widget *w, const char *text)
{
	if (!w || !text || w->type != WIDGET_LABEL) return;
	LabelArgs *args = w->args;
	void *tmp = realloc(args->text, strlen(text)+1);
	if (tmp) {
		args->text = tmp;
		strcpy(args->text, text);
	}
}

void wLabel_SetFont(Widget *w, nSDL_Font *font)
{
	if (!w || !font || w->type != WIDGET_LABEL) return;
	LabelArgs *args = w->args;
	args->font		= font;
}

void wLabel_SetAlignment(Widget *w, int alignment)
{
	if (!w || w->type != WIDGET_LABEL) return;
	LabelArgs *args = w->args;
	args->alignment	= alignment;
}



char *wLabel_GetText(Widget *w)
{
	if (!w || w->type != WIDGET_LABEL) return NULL;
	LabelArgs *args = w->args;
	return args->text;
}






