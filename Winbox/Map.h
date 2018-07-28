#ifndef MAP_H
#define MAP_H


// Arguments
typedef struct MAP_ELEMENT {
	SDL_Surface *pic0;	// image de base
	SDL_Surface *pic1;	// image sélectionnée (optionnel)
	int isSelected;
} MapElement;

typedef struct MAP_ARGS {
	int dCols, nCols;
	int dRows, nRows;
	int minSpace;
	int nElements;
	MapElement *elements;
	int mode;	// 0 == pressable, 1 = sélectionnable
	int w;	// largeur d'une image
	int h;	// hauteur d'une image
	
	int x, y, cx, cy;	// position actuelle du curseur
	int state;
} MapArgs;

// Création
Widget *wMap(int dCols, int nCols, int dRows, int nRows, int minSpace, int mode);
void DrawMap(Widget *w);
int ActivateMap(Widget *w);
void CloseMap(Widget *w);
void wMap_AddElement(Widget *w, SDL_Surface *pic0, SDL_Surface *pic1);


// Méthodes
void wMap_DrawElement(Widget *w, int elt);
BOOL wMap_IsElementSelected(Widget *w, int elt);
void wMap_SelectElement(Widget *w, int elt);
void wMap_UnselectElement(Widget *w, int elt);
void wMap_SelectAll(Widget *w);
void wMap_UnselectAll(Widget *w);
void wMap_SetElementImage(Widget *w, int elt, SDL_Surface *pic0, SDL_Surface *pic1);
MapElement *wMap_GetElementStruct(Widget *w, int elt);

#endif