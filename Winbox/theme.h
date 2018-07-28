#ifndef wTHEME_H
#define wTHEME_H

#include <SDL/SDL.h>

typedef struct wTHEME {
	struct BACKGROUND *defbg;  // background de base
	Uint32 color1;  // couleur principale 1 - background (obsolète?)
	Uint32 color2;  // couleur principale 2 - exterieure
	Uint32 color3;  // couleur - intérieur
	Uint32 color4;  // couleur de la police
	int font_index;	// type de la police
	nSDL_Font *font;	// police principale
	
	
	struct BACKGROUND *button_bg;
	struct BACKGROUND *button_bg_selected;
	nSDL_Font *button_font;
	nSDL_Font *button_font_selected;
	
	SDL_Surface *img_check_0; // 125
	SDL_Surface *img_check_1;
	
	SDL_Surface *img_scrollb_up;	// 53
	SDL_Surface *img_scrollb_down;
	SDL_Surface *img_scrollb_right;
	SDL_Surface *img_scrollb_left;
	
	Uint32 tab_color2;	// couleur-background des tabs défaut
	Uint32 tab_color3;	// coueur-background du tab sélectionné
	Uint32 tab_color4;	// couleur-background du tab hovered
	SDL_Surface *tab_delete;
	nSDL_Font *tab_f1;	// police du tab normal
	nSDL_Font *tab_f2;	// police du tab sélectionné/survolé
	
	Uint32		 request_c1;	// Couleur du background - sélectionné
	Uint32		 request_c2;	// Couleur du background - non-sélectionné
	Uint32		 request_c3;	// couleur du curseur '|'
	nSDL_Font	*request_f1;	// Police de base
	nSDL_Font	*request_f2;	// Police de la VoidStr
	SDL_Surface	*request_right;
	SDL_Surface	*request_left;
	
	Uint32 text_c1;				// couleur intermédiaire - hover
	
	Uint32		menus_title_c1;	// dégradé background titre
	Uint32		menus_title_c2;
	nSDL_Font	*menus_title_font;	// police titre
	nSDL_Font	*menus_font1;		// police non-sélectionné
	Uint32		menus_c1;			// couleur d'arrière-plan
	nSDL_Font	*menus_font2;		// police sélectionné
	SDL_Surface	*menus_right;
	
	Uint32	map_c1;	// couleur de l'aura lors du survol
	
	SDL_Surface	*pbar_cursor;
	SDL_Surface	*pbar_cursor_v;
	
	struct BACKGROUND *window_title_bg;
	nSDL_Font	*window_title_font;
	SDL_Surface *window_icon;
	Uint32		window_c1;
	
	struct BACKGROUND *dialog_bg1;	// background du bouton 'OK'
	struct BACKGROUND *dialog_bg2;	// background du bouton 'pas OK'
	struct BACKGROUND *dialog_bg_selected;	// background des deux boutons sélectionnés
	
} wTHEME;

void SetColorKey(SDL_Surface *s);
void wInitTheme(wTHEME *theme);
void wSetTheme(wTHEME *theme);
wTHEME *wDefaultTheme();
void wCloseTheme();

#endif

