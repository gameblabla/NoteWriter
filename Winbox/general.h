#ifndef GENERAL_H
#define GENERAL_H


// Fonctions sur les couleurs
Uint32 Lighter(Uint32 color, Uint8 value);
Uint32 Darker(Uint32 color, Uint8 value);
Uint32 Contrasted(Uint32 color, Uint8 value);
Uint32 ColorToGray(Uint32 color);
Uint32 RGB(Uint8 r, Uint8 g, Uint8 b);
Uint8 Red(Uint32 c);
Uint8 Green(Uint32 c);
Uint8 Blue(Uint32 c);


// Fonctions de dessin
SDL_Surface *NewSurface(int width, int height);
void DrawSurface(SDL_Surface *src, SDL_Rect *srcRect, SDL_Surface *dest, SDL_Rect *dstRect);
void DrawRect(SDL_Surface *scr, SDL_Rect *rect, Uint32 color);
void DrawRectXY(SDL_Surface *scr, int x, int y, int w, int h, Uint32 color);
void DrawFillRectXY(SDL_Surface *scr, int x, int y, int w, int h, Uint32 color);
void DrawFillRect(SDL_Surface *scr, SDL_Rect *rect, Uint32 color);
void FillHGradientRect(SDL_Surface *scr, SDL_Rect rect, Uint32 color1, Uint32 color2);
void FillVGradientRect(SDL_Surface *scr, SDL_Rect rect, Uint32 color1, Uint32 color2);
void GradientHalo(SDL_Surface *scr, SDL_Rect *r, int w, Uint32 c1, Uint32 c2);
void DrawUnBoundedRectXY(SDL_Surface *scr, int x, int y, int w, int h, Uint32 color);
void DrawUnBoundedRect(SDL_Surface *scr, SDL_Rect *r, Uint32 color);
void DrawClippedStr(SDL_Surface *scr, nSDL_Font *f, int x, int y, const char *str);

// Autres
Uint32 Rand(Uint32 n);
SDL_Rect IntersectRects(SDL_Rect *s1, SDL_Rect *s2);



#endif