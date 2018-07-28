#ifndef STRUCTURES_H
#define STRUCTURES_H


#include <SDL/SDL.h>
#include "theme.h"



// - AUTRES STRUCTURES-

enum ALIGNMENT {
	ALIGN_LEFT = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2,
	ALIGN_TOP = 0,
	ALIGN_MID = 4,
	ALIGN_BOT = 8
};

enum ARROWS {
	ARROW_LEFT	= 0,
	ARROW_RIGHT	= 1,
	ARROW_BOT	= 2,
	ARROW_TOP	= 3,
};

enum SIGNALS {
	SIGNAL_HOVER = 1,
	SIGNAL_ACTION,
	SIGNAL_CLICK,
	SIGNAL_KEY,
	SIGNAL_LEAVE,
	SIGNAL_ACTION2,
};

enum ACTIONS {
	ACTION_REFRESH = -2,
	ACTION_ERROR,
	ACTION_EXIT,
	ACTION_CONTINUE,
};

enum ARGS_TYPES {
	WFONT		= 1,
	WMENU		= 2,
	WSURFACE	= 4,
	WBACKG	= 8,
	WTHEME	= 16,
};


typedef struct KEYWORD {
	char str[32];
	nSDL_Font *font;
} KEYWORD;



// - BACKGROUND -
typedef struct BACKGROUND {
	Uint32 color1;
	Uint32 color2;
	SDL_Surface *image;
	short mode;
} wBACKGROUND;

enum BACKGROUND_MODES {
	BACKG_SOLID,
	BACKG_HGRAD,
	BACKG_VGRAD,
	BACKG_HALO,
	BACKG_TRICOLOR,
	BACKG_IMG,
	BACKG_MOSAIC,
};


// - STRUCTURES DE WIDGETS -
/* Les différentes structures utilisées par la
 gestion des Widgets
*/

enum WIDGET_TYPES {
	// Widget vide
	NOWIDGET,
	
	// Widgets statiques
	WIDGET_PANEL,				// FAIT
	WIDGET_LABEL,				// FAIT
	WIDGET_TABLE,				// FAIT
	WIDGET_PIXMAP,				// FAIT
	
	// Widgets layouts
	WIDGET_WINDOW,	
	WIDGET_BASIC_LAYOUT,		// FAIT
	WIDGET_CARDINAL_LAYOUT,	// FAIT
	WIDGET_GRID_LAYOUT,		// FAIT
	WIDGET_TAB_LAYOUT,		// FAIT
	WIDGET_HORIZONTAL_LAYOUT,// FAIT
	WIDGET_DIALOG,				// FAIT
	WIDGET_SCROLLED_LAYOUT,	//FAIT
	
	// Widgets dynamiques
	WIDGET_CHECK,				// FAIT
	WIDGET_BUTTON,				// FAIT
	WIDGET_REQUEST,			// FAIT
	WIDGET_TEXT,				// FAIT
	WIDGET_LIST,				// FAIT
	WIDGET_BOXLIST,			// FAIT
	WIDGET_EXPLORER,			// FAIT
	WIDGET_MAP,					// FAIT
	WIDGET_PROGRESS_BAR,		// FAIT
	WIDGET_PROGRESS_BAR_V,	// FAIT
	WIDGET_WINDOW_LIST,		// FAIT
	
	WIDGET_CUSTOM,
};

struct WIDGET {
	SDL_Rect bounds;
	int displayBounds;  // -1 = jamais, 0 = si actif, 1 = toujours
	int Hexpansive;
	int Vexpansive;
	wBACKGROUND *background;
	int isDynamic;
	int isLayout;
	int (*callBack) (struct WIDGET *, int);
	void 	*customArgs;
	void (*freeCustomArgs) (void *);
	int freeArgsType;
	
	int					type;
	struct CONSTRUCT	*construct;
	struct WIDGET		*parent;
	void					*args;
	
	void 	(*draw)		(struct WIDGET *);
	int	(*activate)	(struct WIDGET *);
	void 	(*close)		(struct WIDGET *);
	int	(*add)		(struct WIDGET *, struct WIDGET *, int spot); // uniquement pour les widgets qui peuvent en contenir d'autres
};

typedef struct WIDGET Widget;



// - CONSTRUCT -
/* Le Construct représente l'ensemble des widgets d'une
construction particulière
*/


typedef struct CONSTRUCT {
	SDL_Surface *scr;
	wTHEME *theme;
	BOOL saveImage;
	SDL_Surface *image;
	BOOL onlyOneDynamic;
	
	Widget **widgets;
	int nWidgets;
	nSDL_Font **fonts;
	int nFonts;
	SDL_Surface **surfaces;
	int nSurfaces;
	void **elts;
	int nElts;
	
	int key;
	SDL_Rect previousWidgetBounds;
	
} wCONSTRUCT;



#endif

