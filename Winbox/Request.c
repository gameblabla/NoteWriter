#include <os.h>
#include <SDL/SDL.h>
#include "structures.h"
#include "keys.h"
#include "general.h"
#include "widgets.h"
#include "Request.h"
#include "textInput.h"

// Création
Widget *wRequest(const char *str, const char *text, int type, int max)
{
	return wExRequest(str, text, type, max, NULL);
}

Widget *wExRequest(const char *str, const char *text, int type, int max, const char *voidStr)
{
	if (max < 0 && type != REQUEST_INT) return NULL;	
	
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_REQUEST;
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
	
	w->draw	= DrawRequest;
	w->activate	= ActivateRequest;
	w->close		= CloseRequest;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(RequestArgs));
	RequestArgs *args = w->args;
	args->str		= str? malloc(strlen(str)+1) : NULL;
	args->type		= type;
	args->min		= (max<0? -2147483647:0);
	args->max		= max;
	args->voidStr	= voidStr ? malloc(strlen(voidStr)+1) : NULL;
	if (str) strcpy(args->str, str);
	if (voidStr) strcpy(args->voidStr, voidStr);
	
	
	int x = max;
	if (type == REQUEST_INT) {
		x = 0;
		while (max) x++, max /= 10;
	}
	args->text = malloc(x+1);
	if (!args->text) {
		free(w);
		return NULL;
	}
	
	args->text[0] = 0;
	if (text) strncpy(args->text, text, x);
	args->text[x] = 0;
	
	return w;
}


// Dessin
void DrawRequest(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	RequestArgs *args = w->args;
	int c = Darker(theme->color1, 60);
	
	int ws = nSDL_GetStringWidth(theme->request_f1, args->text);
	int xw = (w->bounds.w*3)/5;
	if (!args->str)
		xw = w->bounds.w - 6;
	else if (xw + nSDL_GetStringWidth(theme->font, args->str) + 9 > w->bounds.w)
		xw = w->bounds.w - nSDL_GetStringWidth(theme->font, args->str) - 9;
	int x = w->bounds.x + w->bounds.w - xw - 3;
	
	
	if (args->str)
		DrawClippedStr(scr, theme->font, w->bounds.x + 3, w->bounds.y + 6, args->str);
	DrawFillRectXY(scr, x, w->bounds.y + 3, xw, 14, theme->request_c2? theme->request_c2 : theme->color1);
	DrawRectXY(scr, x, w->bounds.y + 3, xw, 14, c);
	
	if ((args->text == NULL || args->text[0] == 0) && args->voidStr && args->voidStr[0]) {
		if (nSDL_GetStringWidth(theme->request_f2, args->voidStr) < xw - 4)
			DrawClippedStr(scr, theme->request_f2, x+2, w->bounds.y + 6, args->voidStr);
	}
	else if (ws < xw - 4)
		DrawClippedStr(scr, theme->request_f1, x+2, w->bounds.y + 6, args->text);
	else {
		DrawSurface(theme->request_right, NULL, scr, &((SDL_Rect) {x+xw-7, w->bounds.y+6, 5, 7}));
		
		int y = 1;
		char c = args->text[y];
		do {
			if (y > 1) args->text[y-1] = c;
			c = args->text[y];
			args->text[y++] = 0;
		} while (nSDL_GetStringWidth(theme->request_f1, args->text) < xw - 16);
		
		DrawClippedStr(scr, theme->request_f1, x+2, w->bounds.y + 6, args->text);
		args->text[y-1] = c;
	}
}


// Activation
BOOL isCompatible(Widget *w, char *pc)
{
	RequestArgs *args = w->args;
	char c = *pc;
	
	
	if (args->type == REQUEST_ALL) {
		if (strlen(args->text) >= (int) args->max) return 0;
		return 1;
	}
	
	
	if (args->type == REQUEST_INT) {
		if (c == '-' && !strlen(args->text) && args->min < 0) return 1;
		
		int x=0, n = 0, s=1;
		if (args->text[0] == '-') x++, s = -1;
		
		// on trouve la valeur qu'aurait le nombre si on ajoutait le caractère
		while (x < strlen(args->text))
			n = 10*n + args->text[x++] - '0';
		n = s * (10*n + c - '0');
		
		// s'il est plus grand que la valeur max, on définit la valeur max comme valeur actuelle
		if (n > args->max) {
			wRequest_SetInt(w, args->max);
			return 0;
		}
		
		// s'il est plus petit que la valeur min, on définit la valeur min comme valeur actuelle
		if (n < args->min) {
			wRequest_SetInt(w, args->min);
			return 0;
		}
		if (c >= '1' && c <= '9') return 1;
		if (c == '0' && (strlen(args->text) != 1 || args->text[0] != '0')) return 1;
	}
	
	
	else if (args->type == REQUEST_ID) {
		if (strlen(args->text) >= (int) args->max) return 0;
		if (c >= '0' && c <= '9') return 1;
		if (c >= 'a' && c <= 'z') return 1;
		if (c >= 'A' && c <= 'Z') return 1;
	}

	
	else if (args->type == REQUEST_VAR) {
		if (strlen(args->text) >= (int) args->max) return 0;
		if (c >= 'a' && c <= 'z') return 1;
		if (c >= 'A' && c <= 'Z') return 1;
		if (strlen(args->text) && c >= '0' && c <= '9') return 1;
		if (strlen(args->text) && (c == ' ' || c == '_' || c == '-' || c == '.')) return 1;
	}
	
	
	else if (args->type == REQUEST_NAME) {
		if (strlen(args->text) >= (int) args->max) return 0;
		if (c >= 'A' && c <= 'Z') return 1;
		if (c >= 'a' && c <= 'z') {
			if (!strlen(args->text)) *pc += 'A' - 'a';
			return 1;
		}
		if (strlen(args->text)) {
			if (c == '-' || c == ' ') return 1;
			if ((c>128 && c<155) || (c>159 && c<165)) return 1;
			if (c == 39) return 1;
		}
	}
	
	return 0;
}

int ActivateRequest(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	RequestArgs *args = w->args;
	wTHEME *theme = w->construct->theme;
	
	int ws;
	int xw = (w->bounds.w*3)/5;
	if (!args->str)
		xw = w->bounds.w - 6;
	else if (xw + nSDL_GetStringWidth(theme->font, args->str) + 9 > w->bounds.w)
		xw = w->bounds.w - nSDL_GetStringWidth(theme->font, args->str) - 9;
	int x = w->bounds.x + w->bounds.w - xw - 3;
	int c1 = theme->request_c1? theme->request_c1 : theme->color1;
	int c3 = theme->request_c3? theme->request_c3 : theme->color4;
	char k1=0, k2=0, k3=0, k4, k5;
	int ok = ACTION_CONTINUE;
	char c = 0, kc=0;
	
	
	do {
		
		ws = nSDL_GetStringWidth(theme->request_f1, args->text);
		
		// on redessine
		DrawFillRectXY(scr, x+1, w->bounds.y+4, xw-2, 12, c1);
		if (ws < xw - 6) {
			DrawClippedStr(scr, theme->request_f1, x+2, w->bounds.y + 6, args->text);
			DrawFillRectXY(scr, x+ws + (ws? 3:2), w->bounds.y+5, 1, 9, c3);
		}
		else {
			DrawSurface(theme->request_left, NULL, scr, &((SDL_Rect) {x+2, w->bounds.y+6, 5, 7}));
			int y = strlen(args->text)-1;
			while (nSDL_GetStringWidth(theme->request_f1, args->text + y) < xw - 12) y--;
			ws = nSDL_GetStringWidth(theme->request_f1, args->text + (++y));
		

			DrawClippedStr(scr, theme->request_f1, x+7, w->bounds.y + 6, args->text+y);
			DrawFillRectXY(scr, x+ws+8, w->bounds.y+5, 1, 9, c3);
		}
		SDL_Flip(scr);
		// on a redessiné $-)
		
		if (kc && (K_CTRL() || K_SHIFT())) {
			if (k1) while (K_getAlphaKey() == kc);
			if (k2) while (K_getNumericKey() == kc);
			if (k3) while (K_getPuncKey() == kc);
		}
		
		// on chope une touche du keyboard
		if (!K_CTRL() && !K_SHIFT()) while(any_key_pressed());
		while (!any_key_pressed());
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			 || K_SCRATCHPAD() || K_ENTER() || K_ESC())
			goto END;
		// on a chopé €-}
		
		
		k1 = K_getAlphaKey();
		k2 = K_getNumericKey();
		k3 = K_getPuncKey();
		k4 = K_DEL();
		k5 = K_PUNC() && K_CTRL();
		
		
		
		if (k1 || k2 || k3 || k5) {
			
			if			(k1) c = k1;
			else if	(k2) c = k2;
			else if	(k3) c = k3;
			else if	(k5) c = 255;
			kc = c;
			
			if (K_SHIFT())
				c = enshift(c);
			if (K_CTRL())
				c = enctrl(w->construct, c);
			
			if (isCompatible(w, &c)) {
				if (args->type == REQUEST_INT && args->text[0] == '0')
					args->text[0] = 0;
				args->text[strlen(args->text)+1]	= 0;
				args->text[strlen(args->text)]	= c;
				if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
					goto END;
			}
		}
		else if (k4 && args->text[0]) {
			args->text[strlen(args->text)-1] = 0;
			if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
				goto END;
		}
		else if (K_CLICK()) {
			if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
				goto END;
		}
		else if (!K_CTRL() && !K_SHIFT()) {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				goto END;
		}
		
		
	} while (!K_ESC());
	
 END:
	if (K_ENTER()) ok = wExecCallback(w, SIGNAL_CLICK);
	return ok;
}


// Fermeture
void CloseRequest(Widget *w)
{
	if (!w) return;
	if (w->args) {
		RequestArgs *args = w->args;
		if (args) {
			free(args->text);
			free(args->str);
			free(args->voidStr);
		}
	}
}


// Méthodes
void wRequest_SetVoidStr(Widget *w, const char *voidStr)
{
	if (!w) return;
	RequestArgs *args = w->args;
	args->voidStr = realloc(args->voidStr, strlen(voidStr)+1);
	strcpy(args->voidStr, voidStr);
}

void wRequest_SetStr(Widget *w, const char *str)
{
	if (!w) return;
	RequestArgs *args = w->args;
	args->str = realloc(args->str, strlen(str)+1);
	strcpy(args->str, str);
}


void wRequest_SetText(Widget *w, const char *t)
{
	if (!w) return;
	RequestArgs *args = w->args;
	if (w->type == REQUEST_INT) return;
	int x = args->max;
	
	strncpy(args->text, t, x);
}

char *wRequest_GetText(Widget *w)
{
	if (!w) return NULL;
	RequestArgs *args = w->args;
	return args->text;
}

char *wRequest_GetAllocatedText(Widget *w)
{
	if (!w) return NULL;
	RequestArgs *args = w->args;
	char *t = malloc(strlen(args->text)+1);
	if (!t) return NULL;
	strcpy(t, args->text);
	t[strlen(args->text)] = 0;
	
	return t;
}

int wRequest_GetInt(Widget *w)
{
	if (!w) return 0;
	RequestArgs *args = w->args;
	if (args->type != REQUEST_INT) return 0;
	int x=0, s = 1;
	if (args->text[0] == '-') {
		s = -1;
		x++;
	}
	
	int n = 0;
	while (x < strlen(args->text))
		n = 10*n + args->text[x++] - '0';
	
	return s*n;
}

void wRequest_SetInt(Widget *w, int n)
{
	if (!w) return;
	RequestArgs *args = w->args;
	if (args->type != REQUEST_INT) return;
	
	if (n > args->max) n = args->max;
	sprintf(args->text, "%i", n);
	
	return;
}


void wRequest_SetMax(Widget *w, int max)
{
	if (!w) return;
	RequestArgs *args = w->args;
	
	if (max < args->min) return;
	args->max = max;
}


void wRequest_SetMin(Widget *w, int min)
{
	if (!w) return;
	RequestArgs *args = w->args;
	
	if (args->type != REQUEST_INT || min > args->max) return;
	args->min = min;
}


char *wRequest_GetTitle(Widget *w)
{
	if (!w) return NULL;
	RequestArgs *args = w->args;
	return args->str;
}





