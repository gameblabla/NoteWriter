#include <os.h>
#include <SDL/SDL.h>

#include "general.h"
#include "keys.h"
#include "structures.h"
#include "textInput.h"
#include "colored_strings.h"
#include "nSDL_CustomFonts.h"

#include "dialogs.h"


TextBody getTextBody(nSDL_Font *f, int W, char *s)
{
	TextBody text = {NULL, 0};
	if (!s || !s[0]) return text;
	TextRow *rows = NULL;
	char *p0 = s;
	char *p1, *p2, *p3, *p4;
	p1 = p0-1;
	p2 = p0;
	char *pmax = s + strlen(s);
	char c;
	
	while (p0 <= pmax) {  // condition théoriquement inutile ; que je mets par sécurité
		p2 = strchr(p1+1, ' ');
		p3 = strchr(p1+1, '\n');
		p4 = strchr(p1+1, '\25');
		
		if (!p3) p3 = p4;
		else if (p4) p3 = min(p3, p4);
		// p3 est à présent le prochain 'saut de ligne'
		
		if (!p2 && !p3) { // s'il n'y a plus de séparateurs après p1
			
			if (nSDL_GetStringWidthCF(f, p0) <= W) {  // pas besoin de tronquer la ligne ! C'est donc la dernière ligne !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = strlen(p0);
				break;  // on a terminé
			}
			
			if (p1 > p0) {  // si p1 est bien défini, alors [p0->p1] forme une ligne de bonne taille !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = p1-p0;
				p0 = p1+1;
				continue;
			}
			
			else {  // sinon, on est au début d'une nouvelle ligne
				// on tronque la ligne
				p1 = p0;  // la ligne doit faire minimum 1 caractère, sinon on rentre dans une boucle infinie
				c = *p1;
				
				do {
					*p1 = c;
					p1++;
					c = *p1;
					*p1 = 0;
				} while (nSDL_GetStringWidthCF(f, p0) <= W);
				*p1 = c;
				
				// [p0->p1] forme à présent une ligne !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = (--p1)-p0;
				p0 = p1--;
			}
			
			continue;
		}
		
		if (!p2) p2 = p3;
		else if (p3) p2 = min(p2, p3);
		// p2 est à présent le prochain 'séparateur'
		
		
		// on vérifie qu'il n'y ait pas de '\n'
		c = *p2;
		*p2 = 0;
		
		if (nSDL_GetStringWidthCF(f, p0) > W) { // si [p0->p2] dépasse, alors [p0->p1] forme une nouvelle ligne
			
			if (p0 > p1) { // p1 n'est pas défini (on vient de commencer une nouvelle ligne)
				// alors [p0->p2] forme une ligne sans séparateurs qui dépasse !!
				// on tronque la ligne
				*p2 = c;
				p1 = p0;  // la ligne doit faire minimum 1 caractère, sinon on rentre dans une boucle infinie
				c = *p1;
				
				do {
					*p1 = c;
					p1++;
					c = *p1;
					*p1 = 0;
				} while (nSDL_GetStringWidthCF(f, p0) <= W);
				*p1 = c;
				
				// [p0->p1] forme à présent une ligne !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = (--p1)-p0;
				p0 = p1--;
				continue;
			}
			
			else {  // [p0->p1] forme une ligne de bonne taille !
				// on ajoute la ligne !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = p1 - p0;
				
				p0 = p1+1;
			}
		}
		
		else {  // sinon, [p0->p2] est de bonne taille, donc : on continue si (c = ' '), ou on passe à la ligne si <c> est un saut de ligne
			
			if (c == ' ') {
				p1 = p2;
			}
			
			else {
				// on ajoute la ligne !
				text.nRows++;
				rows = realloc(rows, sizeof(TextRow) * text.nRows);
				rows[text.nRows-1].row = p0;
				rows[text.nRows-1].length = p2 - p0;
				
				p1 = p2;
				p0 = p1+1;
			}
		}
		
		
		// on remet sa valeur à p2
		*p2 = c;
	}

	
	text.rows = rows;
	return text;
}




void DrawTextRow(SDL_Surface *scr, nSDL_Font *f, int x, int y, TextRow *l, KEYWORD *kwords, int nkwords)
{
	if (!l->length) return;
	char c = l->row[l->length];
	l->row[l->length] = 0;
	int t;
	char *p;
	C_WORD *cwords = malloc(64 * sizeof(C_WORD));  // 64 mots colorés par ligne semble être une limite (plus que) raisonnable
	int ncw = 0;  // compteur de mots colorés
	int isSym;  // un symbole est un caractère simple différent d'une lettre
	char tc;
	int b;  // boolean
	
	
	for (t=0; t < nkwords; t++) {  // on check s'il y a des mots à colorer
		// on vérifie si le mot-clé est un symbole
		isSym = 0;
		if ((tc=strlen(kwords[t].str)) == 1) {
			if ((tc<'a' || tc>'z') && (tc<'A' || tc>'Z'))
				isSym = 1;
		}
		
		p = strstr(l->row, kwords[t].str);
		
		while (p) {  // si on en trouve un, on l'ajoute aux CWORDS
			// on vérifie tout d'abord, si on n'a pas un symbole, que le mot n'est pas
			// inclus dans un mot plus grand (ex:  on cherche 'int' mais on ne va pas colorer
			// les trois premiers caractères de 'integer')
			b = 1;
			if (!isSym) {
				if (p > l->row) {
					tc = *(p-1);
					if ((tc>='a' && tc<='z') || (tc>='A' && tc<='Z') || (tc>='0' && tc<='9')) b = 0;
				}
				// si kwords[t].str n'est pas tout au bout de la ligne
				tc = p[strlen(kwords[t].str)];
				if ((tc>='a' && tc<='z') || (tc>='A' && tc<='Z') || (tc>='0' && tc<='9')) b = 0;
			}
			
			if (b) {
				cwords[ncw].offset = p - l->row;
				cwords[ncw].length = strlen(kwords[t].str);
				cwords[ncw++].font = kwords[t].font;
			}
			
			p = strstr(p+strlen(kwords[t].str), kwords[t].str);  // on cherche ensuite la prochaine occurence du mot
		}
	}
	DrawColoredString(scr, f, x, y, l->row, (S_COLORS) {ncw, cwords});
	
	l->row[l->length] = c;
	free(cwords);
}



void DrawTextBody(SDL_Surface *scr, nSDL_Font *f, int x, int y, TextBody *b, int fRow, int nRows, KEYWORD *kwords, int nkwords)
{
	if (nRows <= 0 || b->nRows <= 0 || fRow < 0 || fRow > b->nRows) return;
	int h = 10;
	int z;
	
	for (z = 0; z < min(nRows, b->nRows - fRow); z++)
		DrawTextRow(scr, f, x, y+h*z, &(b->rows[z+fRow]), kwords, nkwords);
}



char enshift(char c)
{
	if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
	if (c == ',') return 39;
	if (c == '(') return '{';
	if (c == ')') return '}';
	if (c == '1') return '<';
	if (c == '3') return '>';
	if (c == '2') return '_';
	if (c == 250) return 168;
	if (c == '0') return 13;
	if (c == '.') return 14;
	if (c == '4') return 3;
	if (c == '5') return 4;
	if (c == '7') return 5;
	if (c == '8') return 6;
	if (c == '6') return 11;
	if (c == '9') return 12;
	
	return c;
}



char enctrl(wCONSTRUCT *ctr, char c)
{
	if (c == '0') return ':';
	if (c == '.') return '!';
	if (c == 250) return '?';
	if (c == '1') return 174;
	if (c == '2') return '"';
	if (c == '3') return 175;
	if (c == '4') return '~';
	if (c == '5') return '\\';
	if (c == '6') return '&';
	if (c == '7') return '#';
	if (c == '8') return '@';
	if (c == '9') return 2;
	if (c == ',') return ';';
	if (c == '(') return '[';
	if (c == ')') return ']';
	if (c == 'l') return 156;
	if (c == 'c') return 135;
	if (c == 'n') return 164;
	if (c == 'f') return 159;
	
	char chars[30];
	int nchars;
	int x;
	
	if (c == 'a') {
		char tmp[6] = {133, 131, 132, 134, 160, 145};
		nchars = 6;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 'e') {
		char tmp[4] = {130, 138, 136, 137};
		nchars = 4;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 'i') {
		char tmp[5] = {140, 139, 141, 161, 173};
		nchars = 5;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 'o') {
		char tmp[4] = {147, 148, 149, 162};
		nchars = 4;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 'u') {
		char tmp[3] = {150, 151, 163};
		nchars = 3;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 'y') {
		char tmp[2] = {152, 157};
		nchars = 2;
		for (x = 0; x < nchars; x++) chars[x] = tmp[x];
	}
	else if (c == 20) {
		nchars = 27;
		for (x = 0; x < nchars; x++) chars[x] = 224+x;
	}
	else
		return c;
	
	SDL_Surface *scr = ctr->scr;
	wTHEME *theme = ctr->theme;
	int z;
	x = nchars-1;
	SDL_Rect r = {160-5*nchars, 225, 10*nchars+2, 13};
	SDL_Rect tr = {0, 226, 10, 11};
	SDL_Surface *save = SDL_CreateRGBSurface(SDL_SWSURFACE, r.w, r.h, 16, 0, 0, 0, 0);
	SDL_BlitSurface(scr, &r, save, NULL);
	Uint32 black = SDL_MapRGB(scr->format, 0, 0, 0);
	Uint32 white = SDL_MapRGB(scr->format, 255, 255, 255);
	Uint32 bg = Darker(theme->color1, 10);
	nSDL_Font *f = nSDL_LoadFont(theme->font_index, 0, 0, 0);
	char k = 0;
	char ch[2] = "0";
	
	
	
	SDL_FillRect(scr, &r, white);
	DrawRect(scr, &r, black);
	
	for (z=0; z < nchars; z++) {
		ch[0]= chars[z];
		DrawClippedStr(scr, f, 162-5*nchars+10*z, tr.y + 2, ch);
	}
	
	SDL_Flip(scr);
	
	
	while (K_CTRL()) {
		k = K_getAlphaKey();
		if (!k) k = K_getNumericKey();
		if (!k) k = K_getPuncKey();
		if (K_PUNC()) k = 255;
		
		if (k == c) {
			// on redessine
			tr.x = 161-5*nchars+10*x;
			SDL_FillRect(scr, &tr, white);
			ch[0]= chars[x];
			DrawClippedStr(scr, f, tr.x+1, tr.y + 2, ch);
			
			x = (x+1)%nchars;
			tr.x = 161-5*nchars+10*x;
			SDL_FillRect(scr, &tr, bg);
			ch[0]= chars[x];
			DrawClippedStr(scr, f, tr.x+1, tr.y + 2, ch);
			
			SDL_Flip(scr);
			
			// on attend que la touche pressée soit relachée
			while (k == c) {
				k = K_getAlphaKey();
				if (!k) k = K_getNumericKey();
				if (!k) k = K_getPuncKey();
				if (K_PUNC()) k = 255;
			}
		}
	}
	
	SDL_BlitSurface(save, NULL, scr, &r);
	SDL_FreeSurface(save);
	nSDL_FreeFont(f);
	SDL_Flip(scr);
	
	return chars[x];
}




