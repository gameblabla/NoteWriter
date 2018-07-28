#include <os.h>
#include <SDL/SDL.h>
#include "common.h"
#include "structures.h"
#include "general.h"

void DrawVScrollBar(SDL_Surface *scr, wTHEME *t, SDL_Rect *bounds, int totalSize, int ypos)
{
	SDL_Rect up, down, bar, inbar, cursor;
	
	// --- UP !!!
	up.x = bounds->x + bounds->w - 10;
	up.y = bounds->y + 3;
	up.h = 7;
	up.w = 7;
	
	// --- DOWN !!!
	down.x = up.x;
	down.y = bounds->y + bounds->h - 10;
	down.h = 7;
	down.w = 7;
	
	// --- BAR !!!
	bar.x = up.x;
	bar.y = up.y + 7;
	bar.w = 7;
	bar.h = bounds->h - 20;
	
	// --- INBAR !!!
	inbar.x = bar.x + 1;
	inbar.y = bar.y + 1;
	inbar.w = bar.w - 2;
	inbar.h = bar.h - 2;

	// --- CURSOR !!!
	cursor.x = up.x+1;
	cursor.y = bar.y + 1 + (ypos * (bar.h - 2)) / totalSize;
	cursor.w = 5;
	cursor.h = (bounds->h * (bar.h - 2)) / totalSize + 1;
	
	// --- Vérification de l'intégrité des résultats...
	if (cursor.y + cursor.h > inbar.y + inbar.h)
		cursor.h = inbar.y - cursor.y + inbar.h;
	
	SDL_Surface *i_up = t->img_scrollb_up;
	SDL_Surface *i_down = t->img_scrollb_down;
	
	// on commence à dessiner
	SDL_BlitSurface(i_up, NULL, scr, &up);
	SDL_BlitSurface(i_down, NULL, scr, &down);
	if (bounds->h > 22) {
		SDL_FillRect(scr, &bar, t->color2);
		SDL_FillRect(scr, &inbar, t->color4);
		FillVGradientRect(scr, cursor, t->color3, t->color1);
	}
}



void DrawHScrollBar(SDL_Surface *scr, wTHEME *t, SDL_Rect *bounds, int totalSize, int xpos)
{
	SDL_Rect left, right, bar, inbar, cursor;
	
	// --- LEFT !!!
	left.x = bounds->x + 3;
	left.y = bounds->y + bounds->h - 10;
	left.h = 7;
	left.w = 7;
	
	// --- RIGHT !!!
	right.x = bounds->x + bounds->w - 10;
	right.y = left.y;
	right.h = 7;
	right.w = 7;
	
	// --- BAR !!!
	bar.x = left.x + 7;
	bar.y = left.y;
	bar.w = bounds->w - 20;
	bar.h = 7;
	
	// --- INBAR !!!
	inbar.x = bar.x + 1;
	inbar.y = bar.y + 1;
	inbar.w = bar.w - 2;
	inbar.h = bar.h - 2;

	// --- CURSOR !!!
	cursor.x = bar.x + 1 + (xpos * (bar.w - 2)) / totalSize;
	cursor.y = left.y+1;
	cursor.w = (bounds->w * (bar.w - 2)) / totalSize + 1;
	cursor.h = 5;
	
	if (cursor.x + cursor.w > inbar.x + inbar.w)
		cursor.w = inbar.x - cursor.x + inbar.w;

	
	SDL_Surface *i_left = t->img_scrollb_left;
	SDL_Surface *i_right = t->img_scrollb_right;
	
	
	// on commence à dessiner
	SDL_BlitSurface(i_left, NULL, scr, &left);
	SDL_BlitSurface(i_right, NULL, scr, &right);
	if (bounds->w > 22) {
		SDL_FillRect(scr, &bar, t->color2);
		SDL_FillRect(scr, &inbar, t->color4);
		FillHGradientRect(scr, cursor, t->color3, t->color1);
	}
}
