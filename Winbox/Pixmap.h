#ifndef PIXMAP_H
#define PIXMAP_H

// Arguments
typedef struct PIXMAP_ARGS {
	SDL_Surface	*img;
	Uint32 colorKey;
	Uint8 alpha;
	int xpos;
	int ypos;
} PixmapArgs;

Widget *wPixmapBMP(const char *file);
Widget *wPixmapNTI( Uint16 *data);
Widget *wPixmapSurface(SDL_Surface *s);
Widget *wExPixmapBMP(const char *file, Uint32 colorKey, Uint8 alpha);
Widget *wExPixmapNTI(Uint16 *data, Uint32 colorKey, Uint8 alpha);
void DrawPixmap(Widget *w);
int ActivatePixmap(Widget *w);
void ClosePixmap(Widget *w);

// Méthodes
void wPixmap_SetImageBMP(Widget *w, const char *file);
void wPixmap_SetImageNTI(Widget *w, Uint16 *data);
void wPixmap_SetImageSurface(Widget *w, SDL_Surface *img);
void wPixmap_SetColorKey(Widget *w, Uint32 colorKey);
void wPixmap_SetAlpha(Widget *w, Uint8 alpha);

#endif
