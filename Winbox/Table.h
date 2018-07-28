#ifndef TABLE_H
#define TABLE_H

#include "os.h"

// Arguments
enum ELEMENT_TYPE {
	ELEMENT_INT,
	ELEMENT_FLOAT,
	ELEMENT_STRING
};

typedef struct TABLE_EL {
	int type; // entier / flottant / string
	char value[4]; // les 4 octets où sont enregistré l'élément
	int alignment;
	nSDL_Font *font;
	wBACKGROUND *background;
} TABLE_EL;

typedef struct TABLE_ARGS {
	int numCols;
	int numRows;
	int maxRows;
	int cRow;
	int alignment;
	nSDL_Font *font;
	int nElements;
	TABLE_EL **elements;
} TableArgs;

void DrawTable(Widget *w);
int ActivateTable(Widget *w);
void CloseTable(Widget *w);
Widget *wExTable(int numCols, int maxRows, int alignment, nSDL_Font *font, const char *elements, ...);
Widget *wTable(int numCols, int maxRows, const char *elements, ...);

int wTable_AddInt(Widget *table, int value);
int wTable_AddFloat(Widget *table, float value);
int wTable_AddString(Widget *table, const char *value);
int wTable_AddElement(Widget *table, int type, char value[], int alignment, nSDL_Font *font, wBACKGROUND *background);


int wTable_AddExInt(Widget *table, int value, int alignment, nSDL_Font *font, wBACKGROUND *background);
int wTable_AddExFloat(Widget *table, float value, int alignment, nSDL_Font *font, wBACKGROUND *background);
int wTable_AddExString(Widget *table, const char *value, int alignment, nSDL_Font *font, wBACKGROUND *background);


#endif

