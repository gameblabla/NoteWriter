#ifndef CARDINALLAYOUT_H
#define CARDINALLAYOUT_H

typedef struct CARDINAL_LAYOUT_ARGS {
	int nWidgets;		// doit être égal à 5
	Widget **widgets; // dans l'ordre : HAUT-BAS-GAUCHE-DROITE-CENTRE
	int cWidget;
	int sideSize[4];  // les différentes tailles des layouts extremes (HAUT-BAS-GAUCHE-DROITE)
} CardinalLayoutArgs;

enum CARDINAL_POSITIONS {
	CARDINAL_NORTH,
	CARDINAL_SOUTH,
	CARDINAL_WEST,
	CARDINAL_EAST,
	CARDINAL_CENTER
};


Widget *wCardinalLayout(int DisplayBounds);
int AddCardinalLayout(Widget *parent, Widget *child, int spot);

void DrawCardinalLayout(Widget *w);
int ActivateCardinalLayout(Widget *w);
void CloseCardinalLayout(Widget *w);


#endif

