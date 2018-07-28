#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

typedef struct GRID_LAYOUT_ARGS {
	int nWidgets;
	Widget **widgets;
	int cWidget;
	int cols;
	int rows;
	int *sizeCols;
	int *sizeRows;
} GridLayoutArgs;


Widget *wGridLayout(int cols, int rows, int DisplayBounds);
int AddGridLayout(Widget *parent, Widget *child, int spot);

void DrawGridLayout(Widget *w);
int ActivateGridLayout(Widget *w);
void CloseGridLayout(Widget *w);

// Méthodes
void wGrid_SetColumnSize(Widget *w, int col, int size);
void wGrid_SetRowSize(Widget *w, int row, int size);


#endif

