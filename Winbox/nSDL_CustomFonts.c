#include <os.h>
#include <SDL/SDL.h>




// Permet de charger une police personnalisée
nSDL_Font *nSDL_LoadCustomFont(unsigned char *fontChars, Uint8 r, Uint8 g, Uint8 b)
{
	nSDL_Font *font;
	int i, j, k;
	int char_width = fontChars[0], char_height = fontChars[1];
	int bytes_per_char = ((char_width-1)/8 + 1) * char_height;

	font = SDL_malloc(sizeof(*font));
	if ( font == NULL ) {
		SDL_OutOfMemory();
		return(NULL);
	}

	for (i = 0; i < NSP_FONT_NUMCHARS; i++) {  // i est le numéro du caractère ; NSP_FONT_NUMCHARS = 256
		int offset = 2 + bytes_per_char * i;
		int max_width = 0;
		Uint32 color;
		SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, char_width, char_height,
		16, 0, 0, 0, 0);
		if ( tmp == NULL ) {
			SDL_OutOfMemory();
			return(NULL);
		}
		color = SDL_MapRGB(tmp->format, r, g, b);
		SDL_FillRect(tmp, NULL, ! color);
		SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, ! color);
		font->char_width[i] = char_width;
		SDL_LockSurface(tmp);
		for ( j = 0; j < char_height; j++)  // j est la ligne du caractère i
		for ( k = 0; k < char_width; k++ ) {  // k est le k-ième bit de la ligne j du caractère i
			
			if ( fontChars[offset + j*((char_width-1)/8 + 1) + k/8]  &  (1 << (7 - k%8)) ) { /* "Pixel" set */
				
				if ( k >= max_width ) {
					font->char_width[i] = k + 1;
					max_width = k;
				}
				nSDL_SetPixel(tmp, k, j, color);
				
			}
		}
		SDL_UnlockSurface(tmp);
		font->chars[i] = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		font->hspacing = 0;
		font->vspacing = 1;
		font->monospaced = SDL_FALSE;
	}

	return(font);
}





// La version originale de cette fonction tronquait verticalement les trop grands caractères
int nSDL_DrawStringCF(SDL_Surface *surface, nSDL_Font *font,
int x, int y, const char *format, ...)
{
	char buf[2048];
	int length;
	SDL_Rect pos;
	va_list args;
	int i;
	int font_height = font->chars[0]->h;
	int max_width = 0;
	int n;
	
	if (font->monospaced) {
		for (n=0; n < 256; n++)
			if (font->char_width[n] > max_width) max_width = font->char_width[n];
	}

	va_start(args, format);
	if ( vsprintf(buf, format, args) < 0 )
	return(-1);
	va_end(args);
	length = (int)strlen(buf);
	pos.x = x;
	pos.y = y;
	NSP_DEBUG("\"%s\" at (%d, %d)", buf, pos.x, pos.y);

	for ( i = 0; i < length; ++i ) {
		int c = buf[i];
		if ( c == '\n' ) {
			pos.x = x;
			pos.y += font_height + font->vspacing;
		}
		else {
			SDL_Rect rect;
			rect.x = rect.y = 0;
			rect.w = font->char_width[c];
			rect.h = font_height;
			if (font->monospaced) pos.x += (max_width - font->char_width[c]) / 2;  // on centre le caractère
			if ( SDL_BlitSurface(font->chars[c], &rect, surface, &pos) == -1 )
				return(-1);
			
			if (font->monospaced)	pos.x += max_width + font->hspacing - (max_width - font->char_width[c]) / 2;
			else							pos.x += font->char_width[c] + font->hspacing;
		}
	}

	return(0);
}

// la fonction originale ne prenait pas en compte la possibilité que dans le cas
// monospaced, la largeur d'un caractère pouvait être différente de 8
int nSDL_GetStringWidthCF(nSDL_Font *font, const char *s)
{
	if (!s[0]) return 0;
	int width = 0;
	int max_width = 0;
	int max_char_width = 0;
	int x;
	if (font->monospaced) {
		for (x=0; x < 256; x++)
			if (font->char_width[x] > max_char_width) max_char_width = font->char_width[x];
	}
	
	
	do {
		x = *s;
		if ( x == '\n' || x == '\0' ) {
			if ( width > max_width )
			max_width = width;
			width = 0;
		}
		else {
			if (font->monospaced)	width += max_char_width + font->hspacing;
			else							width += font->char_width[x] + font->hspacing;
		}
	} while ( *s++ );
	return(max_width - font->hspacing);
}


// la fonction originale ne prenait pas en compte la hauteur de la police, qu'elle
// considérait toujours égale à 8
int nSDL_GetStringHeightCF(nSDL_Font *font, const char *s)
{
	int height = 0;
	int font_height = font->chars[0]->h;
	do {
		if ( *s == '\n' || *s == '\0' )
		height += font_height + font->vspacing;
	} while ( *s++ );
	return(height - font->vspacing);
}

