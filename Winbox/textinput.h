#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <SDL/SDL.h>
#include "structures.h"


typedef struct TEXT_ROW {
	char *row;
	int length;
} TextRow;

typedef struct TEXT_BODY {
	TextRow *rows;
	int nRows;
} TextBody;

TextBody getTextBody(nSDL_Font *f, int w, char *s);
void DrawTextRow(SDL_Surface *scr, nSDL_Font *f, int x, int y, TextRow *l, KEYWORD *kwords, int nkwords);
void DrawTextBody(SDL_Surface *scr, nSDL_Font *f, int x, int y, TextBody *b, int fRow, int nRows, KEYWORD *kwords, int nkwords);

char *strstr_(char *str1, int length, char *str2);

char enshift(char c);
char enctrl(wCONSTRUCT *ctr, char c);



#endif
