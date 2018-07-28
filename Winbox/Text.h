#ifndef TEXT_H
#define TEXT_H

#include <os.h>
#include <SDL/SDL.h>

#include "structures.h"
#include "textinput.h"


// Arguments
typedef struct TEXT_ARGS {
	char *text;
	char *voidStr;
	int bufferSize;
	int dRows;		// displayed rows
	BOOL isEditable;
	nSDL_Font *font;
	
	TextBody body;
	int length;
	int cChar;
	int yRow;
	int cRow;  // ligne actuelle du curseur
	int cCharInRow;  // caractère actuel du curseur
	int select[3];  // base {row1,charInRow,char} du rectangle de sélection
	
	BOOL scrollbar;
	BOOL isActive;	// 0= non-actif, 1= survolé, 2= sélectionné
	BOOL isSelect; // indique si la sélection est active
	
	int nKeyWords;
	KEYWORD *keyWords;  // tableau de keywords
} TextArgs;

Widget *wText(const char *text, int dRows);
Widget *wExText(const char *text, int dRows, nSDL_Font *f, const char *voidStr);
void DrawText(Widget *w);
int  ActivateText(Widget *w);
void CloseText(Widget *w);

// Méthodes
void wText_SetVoidStr(Widget *w, const char *voidStr);
void wText_SetText(Widget *w, const char *t);
void wText_SetFont(Widget *w, nSDL_Font *f);
void wText_SetEditable(Widget *w);
void wText_SetUnEditable(Widget *w);
BOOL wText_IsEditable(Widget *w);
char *wText_GetText(Widget *w);
char *wText_GetAllocatedText(Widget *w);

void wText_AddKeyWord(Widget *w, char *str, nSDL_Font *font);
void wText_RemoveKeyWords(Widget *w);

#endif
