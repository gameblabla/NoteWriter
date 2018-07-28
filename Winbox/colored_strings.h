#ifndef COLORED_STRINGS_H
#define COLORED_STRINGS_H




typedef struct COLORED_WORD {
	int offset;  // position du mot dans la chaîne
	int length;  // longueur du mot
	nSDL_Font *font;  // police du mot
} C_WORD;

typedef struct STRING_COLORS {
	int nCwords;
	C_WORD *cwords;
} S_COLORS;





void DrawColoredString(SDL_Surface *scr, nSDL_Font *f, int x, int y, const char *str, S_COLORS colors);
int GetColoredStringWidth(nSDL_Font *f, const char *str, S_COLORS colors);
void CS_FreeFonts(S_COLORS colors);









#endif


