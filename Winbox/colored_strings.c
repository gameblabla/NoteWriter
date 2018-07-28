#include <os.h>
#include <SDL/SDL.h>
#include "colored_strings.h"
#include "general.h"
#include "nSDL_CustomFonts.h"

#include "dialogs.h"

void DrawColoredString(SDL_Surface *scr, nSDL_Font *f, int x, int y, const char *str, S_COLORS colors)
{
	if (colors.nCwords <= 0) {
		nSDL_DrawStringCF(scr, f, x, y, str);
		return;
	}
	
	int offset = 0;
	char c = 1;
	char *p;
	int l;
	int t;
	nSDL_Font *font;
	C_WORD *cw;
	int next;
	int slen = strlen(str);
	
	while (c) {
		
		// on trouve la police à utiliser, et la longueur du mot
		font = NULL;
		next = 0;
		for (t=0; t < colors.nCwords; t++) {
			cw = &colors.cwords[t];
			if (cw->offset <= offset && (cw->offset + cw->length > offset)) {
				font = cw->font;
				l = cw->length - (offset - cw->offset);
				if (offset + l > slen) l = slen - offset;
				break;
			}
			
			else if (cw->offset > offset) {
				// s'il n'y a aucune police qui contienne l'offset
				// alors on en profite pour chercher le prochain changement de police
				if (!next) next = cw->offset;
				else if (cw->offset < next)
					next = cw->offset;
			}
		}
		
		if (!font) {
			font = f;
			if (next) l = next - offset;
			else l = strlen(str+offset);
		}
				
		
		
		// on dessine le mot
		p = (char *) &str[offset + l];
		c=*p, *p=0;
		nSDL_DrawStringCF(scr, font, x, y, str+offset);
		x += nSDL_GetStringWidthCF(font, str+offset) + font->hspacing;
		*p=c;
		
		
		// on incrémente offset
		offset += l;
	}
	
}








// Cette fonction utilise le même algorithme que précédemment, mais au lieu de dessiner les phrases,
// elle calcule leur taille
int GetColoredStringWidth(nSDL_Font *f, const char *str, S_COLORS colors)
{
	if (colors.nCwords <= 0)
		return nSDL_GetStringWidthCF(f, str);
	if (!str[0]) return 0;
	
	int offset = 0;
	char c = 1;
	char *p;
	int l;
	int t;
	nSDL_Font *font;
	C_WORD *cw;
	int next;
	int slen = strlen(str);
	int x=0;
	
	while (c) {
		
		// on trouve la police à utiliser, et la longueur du mot
		font = NULL;
		next = 0;
		for (t=0; t < colors.nCwords; t++) {
			cw = &colors.cwords[t];
			if (cw->offset <= offset && (cw->offset + cw->length > offset)) {
				font = cw->font;
				l = cw->length - (offset - cw->offset);
				if (offset + l > slen) l = slen - offset;
				break;
			}
			
			else if (cw->offset > offset) {
				// s'il n'y a aucune police qui contienne l'offset
				// alors on en profite pour chercher le prochain changement de police
				if (!next) next = cw->offset;
				else if (cw->offset < next)
					next = cw->offset;
			}
		}
		
		if (!font) {
			font = f;
			if (next) l = next - offset;
			else l = strlen(str+offset);
		}
				
		
		
		// on dessine le mot
		p = (char *) &str[offset + l];
		c=*p, *p=0;
		x += nSDL_GetStringWidthCF(font, str+offset) + font->hspacing;
		*p=c;
		
		
		// on incrémente offset
		offset += l;
	}
	
	
	return x - font->hspacing;
}















void CS_FreeFonts(S_COLORS colors)
{
	int x=0;
	for (x=0; x < colors.nCwords; x++)
		nSDL_FreeFont(colors.cwords[x].font);
}



