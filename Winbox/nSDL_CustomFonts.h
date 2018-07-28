#ifndef NSDL_CUSTOM_FONTS_H
#define NSDL_CUSTOM_FONTS_H

#include <SDL/SDL.h>



nSDL_Font *nSDL_LoadCustomFont(unsigned char *fontChars, Uint8 r, Uint8 g, Uint8 b);  // à utiliser pour charger n'importe quelle police personnalisée
int nSDL_DrawStringCF(SDL_Surface *surface, nSDL_Font *font, int x, int y, const char *format, ...);  // à utiliser si la hauteur de votre police est différente de 8
int nSDL_GetStringHeightCF(nSDL_Font *font, const char *s);  // à utiliser si la hauteur de votre police est différente de 8
int nSDL_GetStringWidthCF(nSDL_Font *font, const char *s);  // à utiliser dans le cas de monospaced fonts



#endif
