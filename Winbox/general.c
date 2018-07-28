#include <os.h>
#include <SDL/SDL.h>
#include "Winbox/Winbox.h"
#include "nSDL_CustomFonts.h"


// TRANSFORMATIONS DE COULEURS --
Uint32 RGB(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(SDL_GetVideoSurface()->format, r, g, b);
}

Uint8 Red(Uint32 color)
{
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	return c[0];
}

Uint8 Green(Uint32 color)
{
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	return c[1];
}

Uint8 Blue(Uint32 color)
{
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	return c[2];
}



Uint32 Lighter(Uint32 color, Uint8 value)
{
	if (value > 100) value = 100;
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	return RGB(c[0] + ((255-c[0])*value)/100,
				  c[1] + ((255-c[1])*value)/100,
				  c[2] + ((255-c[2])*value)/100);
}



Uint32 Darker(Uint32 color, Uint8 value)
{
	if (value > 100) value = 100;
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	return RGB(	c[0] + ((0-c[0])*value)/100,
					c[1] + ((0-c[1])*value)/100,
					c[2] + ((0-c[2])*value)/100);
}



Uint32 ColorToGray(Uint32 color)
{
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
	c[0] = (c[0]+c[1]+c[2])/3;
	return RGB(c[0], c[0], c[0]);
}


Uint32 Contrasted(Uint32 color, Uint8 value)
{
	Uint8 c[3];
	SDL_GetRGB(color, SDL_GetVideoSurface()->format, c, c+1, c+2);
/*  WHAT IS THIS ???????
	for (x=1; x<3; x++)
		c[x] = c[x] < 128? c[x]-127 : c[x]+127;
*/
		if ((c[0]+c[1]+c[2])/3 > 127)
		return Darker(color, value);
	return Lighter(color, value);
}





// DESSIN -- 
SDL_Surface *NewSurface(int width, int height)
{
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16, 0,0,0,0);
}

void DrawSurface(SDL_Surface *src, SDL_Rect *srcRect, SDL_Surface *dest, SDL_Rect *dstRect)
{
	if (!dstRect) SDL_BlitSurface(src, srcRect, dest, NULL);
	else {
		SDL_Rect r = *dstRect;
		SDL_BlitSurface(src, srcRect, dest, &r);
	}
}

void DrawRect(SDL_Surface *s, SDL_Rect *rect, Uint32 color)
{
	if (rect)	DrawRectXY(s, rect->x, rect->y, rect->w, rect->h, color);
	else			DrawRectXY(s, 0, 0, s->w, s->h, color);
}

void DrawRectXY(SDL_Surface *s, int x, int y, int w, int h, Uint32 color)
{
	SDL_FillRect(s, &((SDL_Rect) {x, y, w, 1}), color);
	SDL_FillRect(s, &((SDL_Rect) {x, y, 1, h}), color);
	SDL_FillRect(s, &((SDL_Rect) {x+w-1, y, 1, h}), color);
	SDL_FillRect(s, &((SDL_Rect) {x, y+h-1, w, 1}), color);
}

void DrawFillRect(SDL_Surface *s, SDL_Rect *rect, Uint32 color)
{
	if (!rect) SDL_FillRect(s, NULL, color);
	else {
		SDL_Rect tmp = *rect;
		SDL_FillRect(s, &tmp, color);
	}
}

void DrawFillRectXY(SDL_Surface *s, int x, int y, int w, int h, Uint32 color)
{
	SDL_FillRect(s, &((SDL_Rect) {x, y, w, h}), color);
}

void FillHGradientRect(SDL_Surface *s, SDL_Rect rect, Uint32 color1, Uint32 color2)
{
	Uint8 c1[3];
	Uint8 c2[3];
	int y, h = rect.h;
	rect.h = 1;
	Uint32 color;
	
	SDL_GetRGB(color1, s->format, c1, c1+1, c1+2);
	SDL_GetRGB(color2, s->format, c2, c2+1, c2+2);

	for (y=0; y < h; y++) {
		color = SDL_MapRGB(s->format,
								 c1[0] + ((c2[0]-c1[0])*y)/h,
								 c1[1] + ((c2[1]-c1[1])*y)/h,
								 c1[2] + ((c2[2]-c1[2])*y)/h);
		DrawFillRect(s, &rect, color);
		rect.y++;
	}
}

void FillVGradientRect(SDL_Surface *s, SDL_Rect rect, Uint32 color1, Uint32 color2)
{
	Uint8 c1[3];
	Uint8 c2[3];
	int x, w = rect.w;
	rect.w = 1;
	Uint32 color;
	
	SDL_GetRGB(color1, s->format, c1, c1+1, c1+2);
	SDL_GetRGB(color2, s->format, c2, c2+1, c2+2);
	
	for (x=0; x < w; x++) {
		color = SDL_MapRGB(s->format,
								 c1[0] + ((c2[0]-c1[0])*x)/w,
								 c1[1] + ((c2[1]-c1[1])*x)/w,
								 c1[2] + ((c2[2]-c1[2])*x)/w);
		DrawFillRect(s, &rect, color);
		rect.x++;
	}
}

void DrawUnBoundedRectXY(SDL_Surface *s, int x, int y, int w, int h, Uint32 color)
{
	SDL_FillRect(s, &((SDL_Rect) {x+1, y, w-2, 1}), color);
	SDL_FillRect(s, &((SDL_Rect) {x, y+1, 1, h-2}), color);
	SDL_FillRect(s, &((SDL_Rect) {x+w-1, y+1, 1, h-2}), color);
	SDL_FillRect(s, &((SDL_Rect) {x+1, y+h-1, w-2, 1}), color);
}

void DrawUnBoundedRect(SDL_Surface *s, SDL_Rect *r, Uint32 color)
{
	DrawUnBoundedRectXY(s, r->x, r->y, r->w, r->h, color);
}

void GradientHalo(SDL_Surface *s, SDL_Rect *r, int w, Uint32 color1, Uint32 color2)
{
	Uint8 c1[3];
	Uint8 c2[3];
	int x;
	Uint32 color;
	
	SDL_GetRGB(color1, s->format, c1, c1+1, c1+2);
	SDL_GetRGB(color2, s->format, c2, c2+1, c2+2);
	
	color = SDL_MapRGB(s->format, c1[0], c1[1], c1[2]);
	DrawUnBoundedRect(s, r, color);
	
	for (x=0; x <= w; x++) {
		color = SDL_MapRGB(s->format,
								 c1[0] + ((c2[0]-c1[0])*x)/w,
								 c1[1] + ((c2[1]-c1[1])*x)/w,
								 c1[2] + ((c2[2]-c1[2])*x)/w);
		DrawRectXY(s, r->x+x, r->y+x, r->w-2*x, r->h-2*x, color);
	}
}



void DrawClippedStr(SDL_Surface *s, nSDL_Font *f, int x, int y, const char *str)
{
	SDL_Rect clip;
	SDL_GetClipRect(s, &clip);
	
	if (x >= clip.x && y >= clip.y) {
		nSDL_DrawStringCF(s, f, x, y, str);
		return;
	}
	
	int w = nSDL_GetStringWidth(f, str);
	int h = nSDL_GetStringHeight(f, str);
	SDL_Surface *s2 = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 0, 0, 0, 0);
	
	DrawFillRect(s2, NULL, SDL_MapRGB(s->format, 255, 0, 0));
	nSDL_DrawStringCF(s2, f, 0, 0, str);
	SDL_SetColorKey(s2, SDL_SRCCOLORKEY, SDL_MapRGB(s->format, 255, 0, 0));
	DrawSurface(s2, NULL, s, &(SDL_Rect) {x, y, w, h});
	SDL_FreeSurface(s2);
}






Uint32 Rand(Uint32 n)
{
	static unsigned int r = 0;
	if (!r) r = SDL_GetTicks();
	r = (16807 * r) % 2147483647;
	return r%n;
}


SDL_Rect IntersectRects(SDL_Rect *s1, SDL_Rect *s2)
{
	SDL_Rect s;
	s.x = max(s1->x, s2->x);
	s.y = max(s1->y, s2->y);
	s.w = min(s1->w + s1->x, s2->w + s2->x) - s.x;
	s.h = min(s1->h + s1->y, s2->h + s2->y) - s.y;
	
	return s;
}


