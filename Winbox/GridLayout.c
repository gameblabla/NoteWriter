#include "structures.h"
#include "GridLayout.h"
#include "general.h"
#include "theme.h"
#include "keys.h"
#include "widgets.h"



// Création
Widget *wGridLayout(int cols, int rows, int displayBounds)
{
	if (rows <= 0 || cols <= 0) return NULL;
	
	int x;
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_GRID_LAYOUT;
	w->Hexpansive = 1;
	w->Vexpansive = 1;
	w->parent = NULL;
	w->construct = NULL;
	w->displayBounds = displayBounds;
	w->background = NULL;
	w->isDynamic = 0;
	w->isLayout = 1;
	w->callBack = NULL;
	w->freeArgsType = 0;
	
	w->bounds.w = 160;
	w->bounds.h = 190;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw		= DrawGridLayout;
	w->activate	= ActivateGridLayout;
	w->close		= CloseGridLayout;
	w->add		= AddGridLayout;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(GridLayoutArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	GridLayoutArgs *args = w->args;
	args->nWidgets	= 0;
	args->widgets	= NULL;
	args->cWidget = 0;
	args->cols = cols;
	args->rows = rows;
	args->sizeCols = malloc(cols*sizeof(int));
	args->sizeRows = malloc(rows*sizeof(int));
	
	if (!args->sizeCols || !args->sizeRows) {
		free(args->sizeCols);
		free(args->sizeRows);
		free(w->args);
		free(w);
		return NULL;
	}
	
	for (x=0; x < cols; x++)
		args->sizeCols[x] = 0;
	for (x=0; x < rows; x++)
		args->sizeRows[x] = 0;
	
	return w;
}


// Ajout de Widgets
int AddGridLayout(Widget *parent, Widget *child, int spot)
{
	if (!parent || !child) return 0;
	int x;
	GridLayoutArgs *args = parent->args;
	
	// on vérifie que l'utilisateur a donné une valeur sensée
	if (spot == -1) spot = args->nWidgets;
	if (spot < 0 || spot >= args->cols*args->rows)
		return 0;
	
	// on alloue la mémoire
	if (spot >= args->nWidgets) {
		void *tmp = realloc(args->widgets, (spot+1) * sizeof(Widget *));
		if (!tmp) return 0;
		args->widgets = tmp;
			
		for (x=args->nWidgets; x<spot; x++)
			args->widgets[x] = NULL;
		
		args->nWidgets = spot+1;
	}
	else
		wAddWidgetToConstruct(parent->construct, args->widgets[spot]);

	
	args->widgets[spot] = child;
	return 1;
}


// Dessin
void DrawGridLayout(Widget *w)
{
	GridLayoutArgs *args = w->args;
	int x, nDynamics=0;
	Widget *child = NULL;
	int colSum = 0, rowSum = 0;
	int col, row;
	int cols = args->cols, rows = args->rows;
	int px = 0, py = 0, nx = 1, ny = 1;
	SDL_Rect r;
	w->isDynamic = 0;
	
	// Calcul de DefaultColumnSize et DefaultRowSize 
	for (x=0; x < args->cols; x++) {
		colSum += args->sizeCols[x];
		if (args->sizeCols[x]) cols--;
	}
	colSum = w->bounds.w - colSum - 7 - 2*(args->cols-1);
	
	for (x=0; x < args->rows; x++) {
		rowSum += args->sizeRows[x];
		if (args->sizeRows[x]) rows--;
	}
	rowSum = w->bounds.h - rowSum - 7 - 2*(args->rows-1);
	
	
	// Calcul des coordonnées de chaque widget par rapport à l'autre et dessin
	for (x=0; x < args->nWidgets; x++) {
		child = args->widgets[x];
		
		
		col = x % args->cols;
		row = x / args->cols;
		
		// Calcul des positions initiales
		if (col == 0) {
			px = 0, nx = 1;
			if (row) {
				if (args->sizeRows[row-1])
					py += args->sizeRows[row-1] + 2;
				else {
					py += (rowSum*ny)/rows - (rowSum*(ny - 1))/rows + 2;
					ny++;
				}
			}
		}
		else {
			if (args->sizeCols[col-1])
				px += args->sizeCols[col-1] + 2;
			else {
				px += (colSum*nx)/cols - (colSum*(nx - 1))/cols + 2;
				nx++;
			}
		}
		
		if (!child) continue;
		r.x = w->bounds.x + 3 + px;
		r.y = w->bounds.y + 3 + py;
		
		// calcul de la largeur		
		if (args->sizeCols[col])
			r.w = args->sizeCols[col];
		else
			r.w = (colSum*nx)/cols - (colSum*(nx - 1))/cols + 1;
		
		// calcul de la hauteur
		if (args->sizeRows[row])
			r.h = args->sizeRows[row];
		else
			r.h = (rowSum*ny)/rows - (rowSum*(ny - 1))/rows + 1;
		
		wFindBounds(child, &r, 1, 1);
		
		
		// on vérifie que le widget est affichable
		if (!wIsActivable(child))
			continue;
		
		// dessin-- la taille des bounds peut encore être modifiée ici
		wDrawWidget(child);
		
		if (child->isDynamic && w->construct->onlyOneDynamic && nDynamics < 2) {
			nDynamics++;
			if (nDynamics == 2) w->construct->onlyOneDynamic = 0;
		}
		w->isDynamic |= child->isDynamic;
	}
}




// Activation

int ActivateGridLayout(Widget *w)
{
	GridLayoutArgs *args = w->args;
	int ok;
	int nWidget = args->cWidget;
	
	
	if (K_DOWN())			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
	else if (K_UP())		nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
	else if (K_RIGHT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
	else if (K_LEFT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
	else if (K_TAB() || !args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])) {
		nWidget = 0;
		while (nWidget < args->nWidgets && (!args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])))
			nWidget++;
		if (nWidget == args->nWidgets) nWidget = -1;
	}


	
	
	// Début de la boucle
	do {
		if (nWidget > -1 && nWidget < args->nWidgets) args->cWidget = nWidget;
		ok = wActivateWidget(args->widgets[args->cWidget]);
		if (ok < 2) return ok;
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT()) {
			if (K_DOWN())			nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_BOT, w->construct->previousWidgetBounds);
			else if (K_UP())		nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_TOP, w->construct->previousWidgetBounds);
			else if (K_RIGHT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_RIGHT, w->construct->previousWidgetBounds);
			else if (K_LEFT())	nWidget = wFindNextWidget(args->widgets, args->nWidgets, ARROW_LEFT, w->construct->previousWidgetBounds);
			
			if (nWidget == -1) break;
			ok = 3;
		}
		
		else if (K_TAB()) {
			nWidget = args->cWidget;
			do {
				++nWidget;
			} while (nWidget < args->nWidgets && (!args->widgets[nWidget]->isDynamic || !wIsActivable(args->widgets[nWidget])));
			if (nWidget == args->nWidgets) nWidget = -1;
			
			if (nWidget == -1) break;
			ok = 3;
		}
				
	} while (ok == 3);
	return 2;
}





// Fermeture
void CloseGridLayout(Widget *w)
{
	if (!w) return;
	GridLayoutArgs *args = w->args;
	
	if (args) {
		int x;
		if (args->widgets) {
			for (x=0; x < args->nWidgets; x++)
				wAddWidgetToConstruct(w->construct, args->widgets[x]);
			free(args->widgets);
		}
		if (args->sizeCols) free(args->sizeCols);
		if (args->sizeRows) free(args->sizeRows);
	}
}



// Méthodes

void wGrid_SetColumnSize(Widget *w, int col, int size)
{
	if (!w || col < 0 || size < 0) return;
	if (w->type != WIDGET_GRID_LAYOUT) return;
	GridLayoutArgs *args = w->args;
	if (col > args->cols-1) return;
	
	args->sizeCols[col] = size;
}

void wGrid_SetRowSize(Widget *w, int row, int size)
{
	if (!w || row < 0 || size < 0) return;
	if (w->type != WIDGET_GRID_LAYOUT) return;
	GridLayoutArgs *args = w->args;
	if (row > args->rows-1) return;
	
	args->sizeRows[row] = size;
}
