#include "structures.h"
#include "keys.h"
#include "scrollbar.h"
#include "general.h"
#include "Table.h"
#include "widgets.h"

#define W_SCROLL 10

void WriteCell(Widget *table, int cell);




// Création
Widget *wTable(int numCols, int maxRows, const char *elements, ...)
{
	if (numCols <= 0 || maxRows <= 0) return NULL;
	
	Widget *w;
	w = wExTable(numCols, maxRows, ALIGN_LEFT, NULL, NULL);
	
	if (elements) {
		int x = 0;
		char c;
		va_list v1;
		va_start(v1, elements);
		
		while ((c=elements[x++])) {
			if (c == '-') {
				if (!(c=elements[x++]))
					goto ENDFOR;
				if			(c == 'i') wTable_AddInt(w, va_arg(v1, int));
				else if	(c == 'f') wTable_AddFloat(w, (float) va_arg(v1, double));
				else if	(c == 's') wTable_AddString(w, va_arg(v1, char *));
				else x--;
			}
		}ENDFOR:;
		
		va_end(v1);
	}

	TableArgs *args = w->args;
	if (args->numRows <= maxRows)	w->bounds.h = 12 * args->numRows + 7;
	else {
		w->bounds.w += W_SCROLL;
		w->bounds.h = 12 * maxRows + 7;
	}
	w->bounds.y = (240-w->bounds.h)/2;
	
	return w;
}


Widget *wExTable(int numCols, int maxRows, int alignment, nSDL_Font *font, const char *elements, ...)
{
	if (numCols <= 0 || maxRows <= 0) return NULL;
	
	Widget *w = malloc(sizeof(Widget));
	w->type = WIDGET_TABLE;
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
	
	
	w->draw	= DrawTable;
	w->activate	= ActivateTable;
	w->close		= CloseTable;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(TableArgs));
	TableArgs *args = w->args;
	args->numCols	= numCols;
	args->numRows	= 0;
	args->maxRows	= maxRows;
	args->alignment	= alignment;
	args->font		= font;
	args->nElements = 0;
	args->elements = NULL;
	
		
	if (elements) {
		int x = 0;
		char c;
		va_list v1;
		va_start(v1, elements);
		
		while ((c=elements[x++])) {
			if (c == '-') {
				if (!(c=elements[x++]))
					break;
				if			(c == 'i') wTable_AddInt(w, va_arg(v1, int));
				else if	(c == 'f') wTable_AddFloat(w, (float) va_arg(v1, double));
				else if	(c == 's') wTable_AddString(w, va_arg(v1, char *));
			}
		}
		
		va_end(v1);
	}
	
	w->bounds.w = 60 * numCols + 5;
	if (args->numRows <= maxRows)	w->bounds.h = 12 * args->numRows + 7;
	else {
		w->bounds.w += W_SCROLL;
		w->bounds.h = 12 * maxRows + 7;
	}
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	return w;
}



int wTable_AddInt(Widget *table, int value)
{
	return wTable_AddExInt(table, value, -1, NULL, NULL);
}

int wTable_AddFloat(Widget *table, float value)
{
	return wTable_AddExFloat(table, value, -1, NULL, NULL);
}

int wTable_AddString(Widget *table, const char *value)
{
	return wTable_AddExString(table, value, -1, NULL, NULL);
}

int wTable_AddExInt(Widget *table, int value, int alignment, nSDL_Font *font, wBACKGROUND *bg)
{
	char val[4];
	memcpy(val, &value, 4);
	return wTable_AddElement(table, ELEMENT_INT, val, alignment, font, bg);
}

int wTable_AddExFloat(Widget *table, float value, int alignment, nSDL_Font *font, wBACKGROUND *bg)
{
	char val[4];
	memcpy(val, &value, 4);
	return wTable_AddElement(table, ELEMENT_FLOAT, val, alignment, font, bg);
}

int wTable_AddExString(Widget *table, const char *value, int alignment, nSDL_Font *font, wBACKGROUND *bg)
{
	char val[4];
	memcpy(val, &value, 4);
	return wTable_AddElement(table, ELEMENT_STRING, val, alignment, font, bg);
}


int wTable_AddElement(Widget *table, int type, char value[], int alignment, nSDL_Font *font, wBACKGROUND *bg)
{
	TableArgs *args = table->args;
	
	TABLE_EL *newElement = malloc(sizeof(TABLE_EL));
	if (!newElement) return 0;
	newElement->type = type;
	memcpy(newElement->value, value, 4);
	newElement->alignment = alignment;
	newElement->font = font? font : args->font;
	newElement->background = bg;
	
	
	// on alloue la mémoire pour le nouvel élément
	if (args->nElements == 0 || args->elements == NULL) {
		if (args->nElements || args->elements) return 0;
		
		args->elements = malloc(sizeof(TABLE_EL *));
		if (!args->elements) {
			free(newElement);
			return 0;
		}
	}
	else {
		void *tmp = realloc(args->elements, (args->nElements+1)*sizeof(TABLE_EL *));
		if (!tmp) {
			free(newElement);
			return 0;
		}
		args->elements = tmp;
	}
	
	args->elements[args->nElements++] = newElement;
	
	if (args->nElements > (args->numRows * args->numCols)) {
		args->numRows++;
		if (args->numRows <= args->maxRows)
			table->bounds.h += 12;
		if (!table->isDynamic && args->numRows > args->maxRows)
			table->isDynamic = 1;
	}
	
	return 1;
}




// Dessin
void DrawTable(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	TableArgs *args = w->args;
	args->cRow = 0;
	BOOL vscroll = w->isDynamic;
	int rows = vscroll? args->maxRows : args->numRows, cols = args->numCols;
	int x, y;
	SDL_Rect table = w->bounds;
				table.x += 3;
				table.y += 3;
				table.w -= vscroll? 6 + W_SCROLL : 6;
				table.h -= 6;
	SDL_Rect rect;
	Uint32 lcolor = Contrasted(theme->color4, 40);
	
	// Dessin des bordures
	DrawRect(scr, &table, theme->color4);
	
	// Dessin des cellules
	for (y=0; y < rows; y++) {
		for (x=0; x < cols; x++) {
			if (cols*y + x+1 > args->nElements) goto ENDFOR;
			WriteCell(w, x + cols*y);
		}
	}ENDFOR:;
	
	// dessin des interlignes
	rect.y = table.y + 1;
	rect.w = 1;
	rect.h = table.h - 2;
	for (x=0; x < cols-1; x++) {
		rect.x = table.x + ((table.w-1) * (x+1))/cols;
		DrawFillRect(scr, &rect, lcolor);
	}
	
	rect.x = table.x + 1;
	rect.y = table.y + 12;
	rect.w = table.w - 2;
	rect.h = 1;
	for(y=0; y < rows-1; y++) {
		DrawFillRect(scr, &rect, lcolor);
		rect.y += 12;
	}
	
	
	
	// dessin de la scrollbar si besoin
	if (vscroll) {
		SDL_Surface *s=SDL_CreateRGBSurface(SDL_SWSURFACE, 8, table.h, 16, 0,0,0,0);
		DrawSurface(scr, &(SDL_Rect) {w->bounds.x+w->bounds.w-11, w->bounds.y+2, s->w, s->h}, s, NULL);
		SDL_SetAlpha(s, SDL_SRCALPHA, 128);
		rect.x = table.x + table.w + 2;
		rect.y = table.y;
		rect.w = 8;
		rect.h = table.h;
		
		DrawVScrollBar(scr, theme, &w->bounds, w->bounds.h + 12*(args->numRows - rows), 0);
		DrawSurface(s, NULL, scr, &rect);
		SDL_FreeSurface(s);
	}
	
}



// Activation
int ActivateTable(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	int x;
	TableArgs *args = w->args;
	int rows = args->maxRows, cols = args->numCols;
	BOOL ood = w->construct->onlyOneDynamic;
	SDL_Rect table = w->bounds;
			table.x += 3;
			table.y += 3;
			table.w -= 6 + W_SCROLL;
			table.h -= 6;
	SDL_Surface *s=SDL_CreateRGBSurface(SDL_SWSURFACE, 8, table.h, 16, 0,0,0,0);
	
	
	DrawFillRect(s, NULL, theme->color1);
	SDL_SetAlpha(s, SDL_SRCALPHA, 128);
	int ok = ACTION_CONTINUE;
	
	if (ood) goto INLOOP;
	
	SDL_Flip(scr);
	
	while (!K_ESC()) {

		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			 || K_SCRATCHPAD() || K_ENTER() || K_ESC())
			break;
	
		if (K_CLICK()) {
		  INLOOP:
			
			DrawVScrollBar(scr, theme, &w->bounds, w->bounds.h + 12*(args->numRows - rows), 12*args->cRow);
			SDL_Flip(scr);
			while (K_CLICK());
			
			while (!K_TAB() && !K_SCRATCHPAD() && !K_MENU()) {
				
				if (K_CLICK() || K_ESC() || K_ENTER()) {
					if (ood) goto QUIT;
					else break;
				}
				
				if (K_UP() && args->cRow) {
					if (rows > 1)
						DrawSurface(scr, &((SDL_Rect) {table.x+1, table.y+1,  table.w-2, 12*(rows-1)-1}),
											 scr, &((SDL_Rect) {table.x+1, table.y+13, table.w-2, 12*(rows-1)-1}));
					args->cRow--;
					for (x=0; x < cols; x++)
						WriteCell(w, cols * args->cRow + x);
					DrawVScrollBar(scr, theme, &w->bounds, w->bounds.h + 12*(args->numRows - rows), 12*args->cRow);
					SDL_Flip(scr);
					wait_no_key_pressed();
				}
				
				else if (K_DOWN() && args->cRow+rows < args->numRows) {
					if (rows > 1)
						DrawSurface(scr, &((SDL_Rect) {table.x+1, table.y+13, table.w-2, 12*(rows-1)-1}),
											 scr, &((SDL_Rect) {table.x+1, table.y+1,  table.w-2, 12*(rows-1)-1}));
					args->cRow++;
					for (x=0; x < cols; x++)
						WriteCell(w, cols*(args->cRow + rows-1) + x);
					DrawVScrollBar(scr, theme, &w->bounds, w->bounds.h + 12*(args->numRows - rows), 12*args->cRow);
					SDL_Flip(scr);
					wait_no_key_pressed();
				}
			}
			if (ood) break;
			
			wDrawWidget(w);
			wDrawBounds(w);
			if (K_CLICK() || K_ENTER() || K_ESC()) SDL_Flip(scr);
			while (K_CLICK() || K_ENTER() || K_ESC());
		}
	}
	
  QUIT:
	SDL_FreeSurface(s);
	return ok;
}




// Fermeture
void CloseTable(Widget *w)
{
	if (!w) return;
	if (w->args) {
		TableArgs *args = w->args;
		
		if (w->args) {
			if (args->elements) {
				int x;
				
				for (x=0; x < args->nElements; x++) {
					if (args->elements[x]) {
						TABLE_EL *tel = args->elements[x];
						if (wIsFreedArg(w, WBACKG)) wAddEltToConstruct(w->construct, tel->background);
						if (wIsFreedArg(w, WFONT)) wAddFontToConstruct(w->construct, tel->font);
						free(tel);
					}
				}
				
				free(args->elements);
			}
			
			if (wIsFreedArg(w, WFONT)) wAddFontToConstruct(w->construct, args->font);
		}
	}
}






// Autres
void WriteCell(Widget *w, int n)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	TableArgs *args = w->args;
	int cols = args->numCols;
	int x = n%cols, y = n/cols;
	TABLE_EL *el = NULL;
	if (n < args->nElements) el = args->elements[n];
	SDL_Rect table = w->bounds;
				table.x += 3;
				table.y += 3;
				table.w -= w->isDynamic? 6 + W_SCROLL : 6;;
				table.h -= 6;
	SDL_Rect cell = {table.x+1, table.y+1+12*(y-args->cRow), 0, 11};
	cell.x += ((table.w-1) * x)/cols;
	cell.w = ((table.w-1) * (x+1))/cols - 1 - ((table.w-1) * x)/cols;

	// 1-- on dessine le background
	if (el && el->background)	wBG_Draw(el->background, &cell);
	else 								DrawFillRect(scr, &cell, theme->color1);
	
	if (el) {
		void *val = el->value;
		char str[100];
		str[99] = 0;
		int el_w;
		int str_x;
		int alignment;
		
		// 2-- on trouve l'élément
		if (el->type == ELEMENT_INT) {
			int n = *((int *) val);
			int x = 1;
			int m = 0;
			if (n < 0) {
				m++;
				n = -n;
			}
			
			while (n) {
				m++;
				n /= 10;
			}
			
			n = *((int *) val);
			if (n < 0) n = -n;
			
			while (x <= m) {
				str[m - (x++)] = n%10 + '0';
				n /= 10;
			}
			if (*((int *) val) < 0)
				str[0] = '-';
			str[m] = 0;
		}
		else if (el->type == ELEMENT_STRING)
			strncpy(str, *((const char **) val), 98);
		
		el_w = nSDL_GetStringWidth(el->font? el->font : theme->font, str);
		alignment = el->alignment==-1? args->alignment : el->alignment;
		
		if 		(alignment%4 == 1)
			str_x = cell.x + 1 + (cell.w - el_w)/2;
		else if	(alignment%4 == 2)
			str_x = cell.x + cell.w - 2 - el_w;
		else
			str_x = cell.x + 2;
		
		// 3-- on le dessine
		DrawClippedStr(scr, el->font? el->font : theme->font, str_x, cell.y + 2, str);
	}
}

