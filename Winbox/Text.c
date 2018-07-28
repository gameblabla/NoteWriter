#include <os.h>
#include <SDL/SDL.h>
#include "structures.h"
#include "keys.h"
#include "general.h"
#include "widgets.h"
#include "Text.h"
#include "textinput.h"
#include "scrollbar.h"
#include "nSDL_CustomFonts.h"

#include "dialogs.h"


extern char Clipboard[512];


// Création
Widget *wText(const char *text, int dRows)
{
	return wExText(text, dRows, NULL, NULL);
}

Widget *wExText(const char *text, int dRows, nSDL_Font *f, const char *voidStr)
{
	Widget *w = malloc(sizeof(Widget));
	if (!w) return NULL;
	w->type = WIDGET_TEXT;
	w->displayBounds = 0;
	w->Hexpansive = 1;
	w->Vexpansive = 0;
	w->parent = NULL;
	w->construct = NULL;
	w->background = NULL;
	w->isDynamic = 1;
	w->isLayout = 0;
	w->callBack = NULL;
	w->freeArgsType = 0;

	w->bounds.w = 160;
	w->bounds.h = 10*dRows+6;
	w->bounds.x = (320-w->bounds.w)/2;
	w->bounds.y = (240-w->bounds.h)/2;
	
	w->draw	= DrawText;
	w->activate	= ActivateText;
	w->close		= CloseText;
	w->add = NULL;
	
	w->customArgs = NULL;
	w->freeCustomArgs = NULL;
	
	w->args = malloc(sizeof(TextArgs));
	if (!w->args) {
		free(w);
		return NULL;
	}
	TextArgs *args	 = w->args;
	args->voidStr		= (char *) voidStr;
	args->font			= f;
	args->dRows			= dRows;
	args->cRow 			= 0;
	args->yRow			= 0;
	args->cCharInRow	= 0;
	args->isEditable	= 1;
	args->isActive		= 0;
	args->isSelect		= 0;
	args->scrollbar	= 0;
	args->bufferSize	= 1024;
	args->length		= strlen(text);
	args->cChar			= 0;
	args->body			= (TextBody) {NULL, 0};
	args->nKeyWords	= 0;
	args->keyWords		= NULL;
	
	// on ajuste la taille du buffersize
	while (args->bufferSize < args->length+16)  // +16 par mesure de précaution
		args->bufferSize += 1024;
	
	// on copie le texte
	args->text = malloc(args->bufferSize);
	if (!text)
		args->text[0] = 0;
	else 
		strcpy(args->text, text);
	
	return w;
}



// Dessin
void TextingProcess(Widget *w)
{
	TextArgs *args = w->args;
	wTHEME *theme = w->construct->theme;
	nSDL_Font *f = args->font? args->font : theme->font;
	int x=0, y=-1;
	char *c = NULL;
	args->length = strlen(args->text);
		
	if (!args->text || !args->text[0]) {
		args->body.rows = NULL;
		args->body.nRows = 0;
		args->cCharInRow = 0;
		args->cChar = 0;
		args->cRow = 0;
		args->yRow = 0;
		args->scrollbar = 0;
		if (!args->isEditable) w->isDynamic = 0;
		return;
	}
	
	
	free(args->body.rows);
	
	// on calcule les lignes du texte
	if (args->dRows * (w->bounds.w/5 + 1) < args->length) {
		// on est sûr qu'il y a besoin de scrollbar
		args->scrollbar = 1;
		args->body = getTextBody(f, w->bounds.w - 15, args->text);
	}
	else {
		args->scrollbar = 0;
		args->body = getTextBody(f, w->bounds.w - 8, args->text);
		if (args->body.nRows > args->dRows) {
			// il y a besoin de scrollbar
			free(args->body.rows);
			args->scrollbar = 1;
			args->body = getTextBody(f, w->bounds.w - 15, args->text);
		}
	}
	
	// on calcule la cCharInRow en fonction de cChar
	// et yRow en fonction de cRow
	if (args->body.nRows && args->body.rows) {
		// si on a une chaîne non-vide
		
		if (args->cChar == args->length) {
			// si on est au dernier caractère
			if (args->body.nRows < args->dRows) {
				args->yRow = 0;
				args->cRow = args->body.nRows - 1;
			}
			else {
				args->yRow = args->body.nRows - args->dRows;
				args->cRow = args->dRows - 1;
			}
			args->cCharInRow = args->body.rows[args->body.nRows-1].length;
		}
		
		else {
			c = NULL;
			x = 0;
			y = -1;
			while (x < args->cChar) {  // on cherche y le numéro de la ligne où est situé le curseur
				if (c) x++;
				x += args->body.rows[++y].length;
				if (x < args->length) {
					c = args->body.rows[y].row + args->body.rows[y].length;
					if (c == args->body.rows[y+1].row) c = NULL;
				}
				else
					c = NULL;
			}
			
			if (y == -1) {
				args->yRow = 0;
				args->cRow = 0;
				args->cCharInRow = args->cChar;
			}
			
			else if (args->body.nRows <= args->dRows) {  // si on a pas de scrollbar
				x -= args->body.rows[y].length;
				args->cCharInRow = args->cChar - x;  // on trouve cCharInRow
				
				args->yRow = 0;
				args->cRow = y;
			}
			
			else {  // si on a des scrollbars, il faut également trouver yRow
				x -= args->body.rows[y].length;
				args->cCharInRow = args->cChar - x;  // on trouve cCharInRow
				
				args->cRow = y - args->yRow;
				if (args->cRow < 0) {
					args->yRow = y;
					args->cRow = 0;
				}
				else if (args->cRow >= args->dRows) {
					args->yRow = y - args->dRows + 1;
					args->cRow = args->dRows-1;
				}
			}
		}
	}
	else {
		args->cCharInRow = 0;
		args->cChar = 0;
		args->cRow = 0;
		args->yRow = 0;
	}
	if (!args->isEditable && args->body.nRows <= args->dRows)
		w->isDynamic = 0;
}


void DrawingProcess(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	wTHEME *theme = w->construct->theme;
	TextArgs *args = w->args;
	Uint32 c1 = theme->request_c1? theme->request_c1 : theme->color1;
	Uint32 c2 = theme->request_c2? theme->request_c2 : theme->color1;
	Uint32 c3 = theme->request_c3? theme->request_c3 : theme->color4;
	nSDL_Font *f = args->font? args->font : theme->request_f1;
	if (args->isActive == 1) c1 = theme->text_c1? theme->text_c1 : theme->color1;
	
	if (args->isActive || !args->isEditable)
		DrawFillRectXY(scr, w->bounds.x+2, w->bounds.y+2, w->bounds.w-4, w->bounds.h-4, c1);
	else
		DrawFillRect(scr, &w->bounds, c2);
	
	if (args->scrollbar) {
		// dessin des scrollbars
		DrawVScrollBar(scr, theme, &w->bounds, 10*max(args->body.nRows, args->yRow + args->dRows)+6, 10*args->yRow);
		if (args->isActive < 2) {
			// on dessine en transparent
			SDL_Surface *s_im;
			s_im = SDL_CreateRGBSurface(SDL_SWSURFACE, w->bounds.w, w->bounds.h, 16, 0,0,0,0);
			DrawFillRect(s_im, NULL, args->isActive? c1 : c2);
			SDL_SetAlpha(s_im, SDL_SRCALPHA, 128);
			DrawSurface(s_im, NULL, scr, &w->bounds);
			SDL_FreeSurface(s_im);
			if (args->isActive) wDrawBounds(w);
		}
	}
	
	
	
	// on dessine la sélection
	if (args->isSelect) {
		int y;
		int row1, row2, char1, char2;
		if (args->select[0] < args->cRow+args->yRow || (args->select[0] == args->cRow+args->yRow && args->select[1] < args->cCharInRow)) {
			row1 = args->select[0];
			char1 = args->select[1];
			row2 = args->cRow+args->yRow;
			char2 = args->cCharInRow;
		}
		else {
			row1 = args->cRow+args->yRow;
			char1 = args->cCharInRow;
			row2 = args->select[0];
			char2 = args->select[1];
		}
		int px, py, pw, ph;  // position x,y
		char *s, c;
		int z;
		
		// on dessine la sélection ligne après ligne
		for (z=0; z < 2; z++) {  // z=0 : on dessine les rectangles vides  ;  z=1 : on dessine les rectangles pleins
			for (y=row1; y <= row2; y++) {
				if (y < args->yRow) continue;  // ligne trop "haute"
				if (y - args->yRow >= args->dRows) continue;  // ligne trop "basse"
				
				
				// on trouve px et py
				if (y == row1) {
					s = args->body.rows[row1].row;  // s est le texte de la ligne y
					c = *(s + char1);  // c  est la valeur du caractère sélectionné
					*(s + char1) = 0;
					px = w->bounds.x + 2 + nSDL_GetStringWidthCF(args->font? args->font:theme->font, s);  // on obtient px
					*(s + char1) = c;
					
					py = w->bounds.y + 2 + 10 * (row1 - args->yRow);
				}
				else {
					px = w->bounds.x + 2;
					py = w->bounds.y + 2 + 10 * (y - args->yRow);
				}
				
				
				// on trouve pw et ph
				if (y == row2) {
					s = args->body.rows[row2].row;  // s est le texte de la ligne y
					c = *(s + char2);  // c  est la valeur du caractère sélectionné
					*(s + char2) = 0;
					pw = nSDL_GetStringWidthCF(args->font? args->font:theme->font, s);
					pw -= px - w->bounds.x - 4;
					*(s + char2) = c;
					
					ph = 12;
				}
				else {
					TextRow *Row = &args->body.rows[y];
					int l = Row->length;
					c = Row->row[l];
					Row->row[l] = 0;
					pw = nSDL_GetStringWidthCF(args->font? args->font:theme->font, Row->row);
					pw -= px - w->bounds.x - 4;
					Row->row[l] = c;
					
					ph = 12;
				}
				
				px += z, py += z, pw -= 2*z, ph -= 2*z;
				if (!z) DrawRectXY(scr, px, py, pw, ph, Contrasted(c1, 30));
				else DrawFillRectXY(scr, px, py, pw, ph, Contrasted(c1, 15));
			}
		}
	}
	
	
	// on dessine le texte
	if (!args->isActive) DrawRectXY(scr, w->bounds.x+1, w->bounds.y+1, w->bounds.w-2, w->bounds.h-2, theme->color2);
	if (args->text[0])
		DrawTextBody(scr, f, w->bounds.x+3, w->bounds.y+4, &args->body, args->yRow, args->dRows, args->keyWords, args->nKeyWords);
	else if (args->voidStr)
		DrawClippedStr(scr, theme->request_f2, w->bounds.x+3, w->bounds.y+4, args->voidStr);
	
	
	// on dessine le curseur
	if (args->isActive == 2 && args->isEditable) {
		char *s = args->body.rows? args->body.rows[args->yRow + args->cRow].row : args->text;
		char c = *(s + args->cCharInRow);
		*(s + args->cCharInRow) = 0;
		int cw = nSDL_GetStringWidthCF(f, s);
		*(s + args->cCharInRow) = c;
		DrawFillRectXY(scr, w->bounds.x+cw+3, w->bounds.y+3+10*args->cRow, 1, 9, c3);
	}
}


void DrawText(Widget *w)
{
	TextingProcess(w);
	DrawingProcess(w);
}




// Activation
int ActivateText(Widget *w)
{
	SDL_Surface *scr = w->construct->scr;
	TextArgs *args = w->args;
	args->isActive = 1;
	char K=0, KD=0, KV=0, K1=0, K2=0, K3=0, K4=0;
	BOOL ood = w->construct->onlyOneDynamic;
	int x = 0, y;
	char *s;
	int ok = ACTION_CONTINUE;
	BOOL B=1;
	char klist1[4], klist2[4]={};
	
	
	if (ood) goto INLOOP;
	
	DrawingProcess(w);
	SDL_Flip(scr);
	
	while (!K_ESC()) {
		wait_key_pressed();
		
		
		if (K_UP() || K_DOWN() || K_LEFT() || K_RIGHT() || K_MENU() || K_TAB()
			|| K_SCRATCHPAD() || K_ENTER() || K_ESC()) {
				break;
			}
	
		else if (K_CLICK()) {
			
		  INLOOP:
			args->isActive = 2;
			if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
				return ok;
			else
				TextingProcess(w);
			
			do {
				DrawingProcess(w);
				SDL_Flip(scr);
				while (K_CLICK());
				
				
/// -- ON CHOPE UNE TOUCHE DU KEYBOARD !!!!!!! -------------------------------				
				B = 1;
				while (B) {
									
					/* -- I -- On vérifie qu'une nouvelle touche caractère ne soit pas pressée */
					K = 0;
					K_getChar(klist1);
					
					// on cherche K
					for (x=0; x < 4; x++) {
						if (klist1[x]) {
							for (y=0; y<4; y++) if (klist1[x] == klist2[y])
								break;
							
							if (y==4) {  // alors on a un nouveau caractère !!
								K = klist1[x];
								break;
							}
						}
						else break;
					}
					
					// on actualise la valeur de klist2, et si une touche a été pressée, on la prend en compte
					memcpy(klist2, klist1, 4);
					if (K) {
						B = 0;
						continue;
					}//*/
					
					
					/* -- II -- On vérifie que K_DEL ne soit pas pressée */
					if (K_DEL()) {
						if (!KD || K_CTRL()) {
							B = 0;
							KD = 1;
							continue;
						}
					}
					else KD = 0;//*/
					
					/* -- III -- On vérifie que K_VAR ne soit pas pressée */
					if (K_VAR()) {
						if (!KV) {
							B = 0;
							KV = 1;
							continue;
						}
					}
					else KV = 0;//*/
					
					
					/* -- IV -- On vérifie qu'une touche directionnelle ne soit pas pressée */
					if (K_UP()) {
						if ((!K1&&!K2&&!K3&&!K4) || K_CTRL()) {
							B = 0;
							K1=1;
							continue;
						}
					}
					else K1 = 0;
					
					if (K_RIGHT()) {
						if ((!K1&&!K2&&!K3&&!K4) || K_CTRL()) {
							B = 0;
							K2=1;
							continue;
						}
					}
					else K2 = 0;
					
					if (K_DOWN()) {
						if ((!K1&&!K2&&!K3&&!K4) || K_CTRL()) {
							B = 0;
							K3=1;
							continue;
						}
					}
					else K3 = 0;
					
					if (K_LEFT()) {
						if ((!K1&&!K2&&!K3&&!K4) || K_CTRL()) {
							B = 0;
							K4=1;
							continue;
						}
					}
					else K4 = 0;
					
					
					/* -- V -- On vérifie qu'une touche qui fasse sortir du widget ne soit pas pressée */
					if ((K_ENTER() || K_CLICK())) {
						if ((ok=wExecCallback(w, SIGNAL_CLICK)) != ACTION_CONTINUE)
							return ok;
						
						if (ood) {
							if (K_ENTER()) goto QUIT;
						}
						
						else {
							TextingProcess(w);
							break;
						}
					}
					
					else if (K_MENU() || K_TAB() || K_SCRATCHPAD() || K_ESC()) {
						if (ood) goto QUIT;
						break;
					}//*/
					
					/* -- VI -- On active le callback pour K_DOC */
					else if ((K_DOC()) && !K_CTRL() && !K_SHIFT()) {
						if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
							return ok;
						TextingProcess(w);
						wait_no_key_pressed();
					}//*/
				}
				// on a chopé !!! €-}
				if (B) break;  // on doit sortir du widget
				
				
				
/// -- A PRESENT QU'ON A CHOPE, ON TRAITE LA TOUCHE PRESSEE !!!!!!! -------------------------------				
			
				/* I -- Si une touche caractère a été pressée */
				if (K) {
					if (!args->isEditable) continue;
					
					if (K_SHIFT())
						K = enshift(K);
					if (K_CTRL())
						K = enctrl(w->construct, K);
					
					// on vérifie que le buffersize est assez grand pour accueillir le nouveau caractère
					if (args->bufferSize < args->length+4) {  // +4 par mesure de précaution
						while (args->bufferSize < args->length+4) args->bufferSize += 1024;
						args->text = realloc(args->text, args->bufferSize);
					}
					
					
					// s'il y a une sélection, on remplace la sélection par le caractère
					if (args->isSelect) {
						// on ajoute le caractère
						char *p = args->text + min(args->cChar, args->select[2]);
						*p = K;
						
						// on supprime le reste de la sélection
						int x;
						int d = max(args->cChar,args->select[2]) - min(args->cChar,args->select[2]);
						int m = args->length - max(args->cChar,args->select[2]);
						for (x=1; x <= m; x++) p[x] = p[x+d-1];
						p[x] = 0;
						if (args->select[2] < args->cChar) args->cChar = args->select[2]+1;
						else args->cChar++;
						args->isSelect = 0;
						args->length -= d-1;
					}
					
					// sinon, on insère le nouveau caractère dans le buffer texte
					else {
						for (x = args->length-1; x >= args->cChar; x--)
							args->text[x+1] = args->text[x];
						args->text[args->cChar++]	= K;
						args->text[++args->length]	= 0;
					}
					
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
					TextingProcess(w);  // on modifie l'affichage
				}
				
				
				
				/* II -- Si DEL a été pressée */
				else if (KD) {
					if ((!args->cChar && !args->isSelect) || !args->body.nRows) continue;
					if (!args->isEditable) continue;
					
					if (args->isSelect) {
						// on supprime la sélection
						char *p = args->text + min(args->cChar, args->select[2]);
						int x;
						int d = max(args->cChar,args->select[2]) - min(args->cChar,args->select[2]);
						int m = args->length - max(args->cChar,args->select[2]);
						for (x=0; x <= m; x++) p[x] = p[x+d];
						p[x] = 0;
						if (args->select[2] < args->cChar) args->cChar = args->select[2];
						args->isSelect = 0;
						args->length -= d;
					}
					
					else {
						if (args->length > 1) {
							for (x = args->cChar-1; x < args->length; x++)
								args->text[x] = args->text[x+1];
						}
						else {
							args->text[0] = 0;
						}
						if (args->cChar) args->cChar--;
						args->length--;
					}
											
					if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
						return ok;
					TextingProcess(w);
				}
				
				
				
				/* III -- Si VAR a été pressée */
				else if (KV) {
					if (K_CTRL()) {
						// on enregistre dans le clipboard
						if (!args->isSelect) continue;
						char *p = &args->text[min(args->cChar, args->select[2])];
						int l = max(args->cChar, args->select[2]) - min(args->cChar, args->select[2]);
						strncpy(Clipboard, p, min(l, 1023));
						Clipboard[min(l,1023)] = 0;
					}
					
					else {
						// on copie le clipboard
						if (!Clipboard[0]) continue;
						
						// on supprime la sélection
						if (args->isSelect) {
							char *p = args->text + min(args->cChar, args->select[2]);
							int x;
							int d = max(args->cChar,args->select[2]) - min(args->cChar,args->select[2]);
							int m = args->length - max(args->cChar,args->select[2]);
							for (x=0; x <= m; x++) p[x] = p[x+d];
							p[x] = 0;
							if (args->select[2] < args->cChar) args->cChar = args->select[2];
							args->isSelect = 0;
							args->length -= d;
						}
						
						// on obtient les valeurs nécessaires
						int x;
						int t = strlen(Clipboard);  // <t> est la "distance de translation", et le nombre de caractères du clipboard qui seront copiés
						int l = strlen(args->text + args->cChar);  // <l> est le nombre de caractères qui seront translatés de <t> octets vers la droite
						
						
						// on vérifie que le buffersize est assez grand pour accueillir les nouveaux caractères
						if (args->bufferSize < args->length+t+4) {  // +4 par mesure de précaution
							while (args->bufferSize < args->length+t+4) args->bufferSize += 1024;
							args->text = realloc(args->text, args->bufferSize);
						}
						
						// on insère le clipboard
						for (x=l; x >=0; x--)  // 1. on translate <l> octets de <t> vers la droite
							args->text[args->cChar + t + x] = args->text[args->cChar + x];
						for (x=0; x < t; x++)  // 2. on copie les <t> caractères du clipboard
							args->text[args->cChar + x] = Clipboard[x];
						
						args->cChar += t;
						args->length += t;
						if ((ok=wExecCallback(w, SIGNAL_ACTION)) != ACTION_CONTINUE)
							return ok;
						TextingProcess(w);  // on modifie l'affichage
					}
				}
				
				
				
				/* IV -- Si une touche directionnelle a été pressée */
				else if (K1||K2||K3||K4) {
				
					if (K_SHIFT()) {
						if (args->isEditable && args->body.rows && !args->isSelect) {
							args->select[0] = args->cRow + args->yRow;
							args->select[1] = args->cCharInRow;
							args->select[2] = args->cChar;
							args->isSelect = 1;
						}
					}
					else if (args->isSelect) {
						args->isSelect = 0;
						if (K2) {  // si Droite
							if (args->select[2] > args->cChar) {
								args->cRow			= args->select[0] - args->yRow;
								args->cCharInRow	= args->select[1];
								args->cChar			= args->select[2];
								if (args->cRow >= args->dRows) {
									args->yRow = args->select[0] - args->dRows + 1;
									args->cRow = args->dRows-1;
								}
							}
							continue;
						}
						else if (K4) {  // si Gauche
							if (args->select[2] < args->cChar) {
								args->cRow			= args->select[0] - args->yRow;
								args->cCharInRow	= args->select[1];
								args->cChar			= args->select[2];
								if (args->cRow < 0) {
									args->yRow = args->select[0];
									args->cRow = 0;
								}
							}
							continue;
						}
					}
					
					
					if (K1) {  /* K_UP */
						if (!args->body.rows) continue;
						if (!args->isEditable && !args->yRow) continue;
						if (args->yRow + args->cRow == 0) {
							if (!args->isEditable) continue;
							// si on est à la première ligne, on va au premier caractère
							args->cChar = 0;
							args->cCharInRow = 0;
						}
						else {
							// sinon...
							if (!args->isEditable || !args->cRow)	args->yRow--;
							else												args->cRow--;
							x = args->body.rows[args->yRow + args->cRow].length;
							args->cChar -= x + 1;
							if (x < args->cCharInRow) {
								args->cChar -= args->cCharInRow - x;
								args->cCharInRow = x;
							}
							
							// on vérifie qu'on a bien modifié args->cChar
							y = args->yRow + args->cRow;
							s = args->body.rows[y].row + args->body.rows[y].length;
							if (s == args->body.rows[y+1].row) args->cChar++;
						}
					}
					
					
					else if (K3 && args->body.rows) {  /* K_DOWN */
						if (!args->isEditable && args->body.nRows <= args->dRows)
							continue;
						if (!args->isEditable && args->yRow + args->dRows >= args->body.nRows)
							continue;
						
						if (args->yRow + args->cRow == args->body.nRows - 1) {
							// si on est à la dernière ligne, on va au dernier caractère
							x = args->body.rows[args->yRow + args->cRow].length;
							args->cChar += x - args->cCharInRow;
							args->cCharInRow = x;
						}
						else {
							// sinon...
							x = args->body.rows[args->yRow + args->cRow].length;
							args->cChar += x + 1;
							if (!args->isEditable || args->cRow == args->dRows-1)	args->yRow++;
							else																	args->cRow++;
							x = args->body.rows[args->yRow + args->cRow].length;
							if (x < args->cCharInRow) {
								args->cChar -= args->cCharInRow - x;
								args->cCharInRow = x;
							}
							
							// on vérifie qu'on a bien modifié args->cChar
							y = args->yRow + args->cRow - 1;
							s = args->body.rows[y].row + args->body.rows[y].length;
							if (s == args->body.rows[y+1].row) args->cChar--;
						}
					}
					
					
					else if (K4 && args->body.rows) {  /* K_LEFT */
						if (!args->isEditable) continue;
						if (!args->cChar) continue;
						args->cChar--;
						if (!args->cCharInRow) {
							// 1. on vérifie qu'il faille bien modifier args->cChar
							y = args->yRow + args->cRow - 1;
							s = args->body.rows[y].row + args->body.rows[y].length;
							if (s == args->body.rows[y+1].row) args->cChar++;
							
							// 2. on change les lignes et la position du curseur dans la ligne
							if (!args->cRow)	args->yRow--;
							else					args->cRow--;
							args->cCharInRow = args->body.rows[args->yRow + args->cRow].length;
						}
						else
							args->cCharInRow--;
					}
					
					else if (K2 && args->body.rows) {  /* K_RIGHT */
					 // RIGHT:
						if (!args->isEditable) continue;
						if (args->cChar == args->length) continue;
						args->cChar++; 
						if (args->cCharInRow == args->body.rows[args->yRow + args->cRow].length) {
							// 1. on vérifie qu'il faille bien modifier args->cChar
							y = args->yRow + args->cRow;
							s = args->body.rows[y].row + args->body.rows[y].length;
							if (s == args->body.rows[y+1].row) args->cChar--;
							
							// 2. on change les lignes et la position du curseur dans la ligne
							if (args->cRow == args->dRows-1)	args->yRow++;
							else										args->cRow++;
							args->cCharInRow = 0;
						}
						else
							args->cCharInRow++;
					}
					
					
					// on vérifie à présent que la sélection n'est pas elle-même
					if (args->isSelect && args->select[0] == args->cRow+args->yRow && args->select[1] == args->cCharInRow)
						args->isSelect = 0;
				}
				
				else if (!K_CTRL() && !K_SHIFT()) {
					if ((ok=wExecCallback(w, K_CLICK()||K_ENTER()? SIGNAL_CLICK : SIGNAL_KEY)) != ACTION_CONTINUE)
						return ok;
					TextingProcess(w);
					wait_no_key_pressed();
					continue;
				}
				
			} while (!K_ESC());
			
			
			args->isActive = 1;
			args->isSelect = 0;
			DrawingProcess(w);
			SDL_Flip(scr);
			while(K_ESC() || K_CLICK());
		}
		
		else if (any_key_pressed()) {
			if ((ok=wExecCallback(w, SIGNAL_KEY)) != ACTION_CONTINUE)
				return ok;
			DrawingProcess(w);
			SDL_Flip(scr);
			wait_no_key_pressed();
		}
	}
	
  QUIT:
	args->isActive = 0;
	return ok;
}


// Fermeture
void CloseText(Widget *w)
{
	if (!w) return;
	if (w->args) {
		TextArgs *args = w->args;
		if (args->text) free(args->text);
		if (args->body.rows) free(args->body.rows);
		if (args->font && wIsFreedArg(w, WFONT)) wAddFontToConstruct(w->construct, args->font);
		
		if (args->keyWords) {
			int x;
			for (x=0; x < args->nKeyWords; x++) {
				if (args->keyWords[x].font && wIsFreedArg(w, WFONT))
					wAddFontToConstruct(w->construct, args->keyWords[x].font);
			}
			free(args->keyWords);
		}
	}
}


// Méthodes
void wText_SetVoidStr(Widget *w, const char *voidStr)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	args->voidStr = (char *) voidStr;
}

void wText_SetText(Widget *w, const char *t)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	
	args->length = strlen(t);
	if (args->bufferSize < args->length+16) {
		while (args->bufferSize < args->length+16) args->bufferSize += 1024;
		args->text = realloc(args->text, args->bufferSize);
	}
	
	strcpy(args->text, t);
}

char *wText_GetText(Widget *w)
{
	if (!w) return NULL;
	if (w->type != WIDGET_TEXT) return NULL;
	TextArgs *args = w->args;
	return args->text;
}

char *wText_GetAllocatedText(Widget *w)
{
	if (!w) return NULL;
	if (w->type != WIDGET_TEXT) return NULL;
	TextArgs *args = w->args;
	char *t = malloc(strlen(args->text)+1);
	if (!t) return NULL;
	strcpy(t, args->text);
	
	return t;
}

void wText_SetFont(Widget *w, nSDL_Font *f)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	args->font = f;
}

void wText_SetEditable(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	args->isEditable = 1;
	w->isDynamic = 1;
}

void wText_SetUnEditable(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	args->isEditable = 0;
}

BOOL wText_IsEditable(Widget *w)
{
	if (!w) return 0;
	if (w->type != WIDGET_TEXT) return 0;
	TextArgs *args = w->args;
	return args->isEditable;
}


void wText_AddKeyWord(Widget *w, char *str, nSDL_Font *font)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	
	args->keyWords = realloc(args->keyWords, (args->nKeyWords+1) * sizeof(KEYWORD));
	strncpy(args->keyWords[args->nKeyWords].str, str, 31);
	args->keyWords[args->nKeyWords].str[31] = 0;
	args->keyWords[args->nKeyWords].font = font;
	args->nKeyWords++;
}




void wText_RemoveKeyWords(Widget *w)
{
	if (!w) return;
	if (w->type != WIDGET_TEXT) return;
	TextArgs *args = w->args;
	args->nKeyWords = 0;
}






