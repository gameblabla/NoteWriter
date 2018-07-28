#include <os.h>
#include <SDL/SDL.h>

#include "Winbox.h"

extern wTHEME *Theme;


void wShowMsg(const char *title, const char *msg)
{
	Widget *dialog = wDialog1(title, "OK");
	Widget *text = wText(msg, 4);
	wText_SetUnEditable(text);
	wSetHeight(dialog, 92);
	wAddWidget(dialog, text);
	wExecConstruct(dialog);
}


int wInputMsg(const char *title, const char *msg)
{
	Widget *dialog = wDialog2(title, "Yes", "No");
	Widget *text = wText(msg, 4);
	wText_SetUnEditable(text);
	wSetHeight(dialog, 92);
	wAddWidget(dialog, text);
	
	return wExecConstruct(dialog);
}



void wFastMsg(const char *msg, ...)
{
	char *p1 = (char *) msg;
	char *p2 = p1;
	int bufferSize = 512;
	int msgSize = 0;
	char *buffer = malloc(bufferSize);
	buffer[0] = 0;
	va_list v;
	va_start(v, msg);
	int W = 220;
	SDL_Surface *scr = SDL_GetVideoSurface();

	
	/* 1-- On remplace les %u, %i, %c et %s */
	while ((p2 = strchr(p1, '%'))) {
		*p2 = 0;
		msgSize += strlen(p1);
		if (msgSize > bufferSize) {
			bufferSize = msgSize+64;
			buffer = realloc(buffer, bufferSize);
		}
		strcat(buffer, p1);
		*p2++ = '%';
		
		// on ajoute l'élément suivant
		if (*p2 == 'u') {
			unsigned int u = va_arg(v, unsigned int);
			msgSize += 12;  // 12 chiffres majore largement tout entier
			if (msgSize > bufferSize) {
				bufferSize = msgSize+64;
				buffer = realloc(buffer, bufferSize);
			}
			sprintf(buffer + strlen(buffer), "%u", u);
		}
		
		else if (*p2 == 'i') {
			int i = va_arg(v, int);
			msgSize += 12;  // 12 chiffres majore largement tout entier
			if (msgSize > bufferSize) {
				bufferSize = msgSize+64;
				buffer = realloc(buffer, bufferSize);
			}
			sprintf(buffer + strlen(buffer), "%i", i);
		}
		
		else if (*p2 == 'c') {
			msgSize++;
			if (msgSize > bufferSize) {
				bufferSize = msgSize+64;
				buffer = realloc(buffer, bufferSize);
			}
			buffer[strlen(buffer)+1] = 0;
			buffer[strlen(buffer)] = (char) va_arg(v, int);
		}
		
		else if (*p2 == 's') {
			char *s = va_arg(v, char *);
			msgSize += strlen(s);
			if (msgSize > bufferSize) {
				bufferSize = msgSize+64;
				buffer = realloc(buffer, bufferSize);
			}
			strcat(buffer, s);
		}
		
		else {
			// on ajoute le symbole % et on continue à lire
			msgSize++;
			if (msgSize > bufferSize) {
				bufferSize = msgSize+64;
				buffer = realloc(buffer, bufferSize);
			}
			buffer[strlen(buffer)+1] = 0;
			buffer[strlen(buffer)] = '%';
			p2--;
		}
		
		p1 = p2+1;
	}
	va_end(v);
	
	// on rajoute la fin de la chaîne
	msgSize += strlen(p1);
	if (msgSize > bufferSize) {
		bufferSize = msgSize+64;
		buffer = realloc(buffer, bufferSize);
	}
	strcat(buffer, p1);
	
	
	
	
	
	/* 2-- On calcule les lignes */
	int nLines = 0;
	char *p0 = buffer;
	char *p3;
	p1 = p0-1;
	p2 = p0;
	char c;
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 35,0,0);
	
	while (p2) {
		p2 = strchr(p1+1, ' ');
		p3 = strchr(p1+1, '\n');
		if (!p2 && !p3) break;
		if (!p2) p2 = p3;
		else if (p3) p2 = min(p2, p3);
		// p2 est à présent le prochain 'séparateur'
		
		
		// on vérifie qu'il n'y ait pas de '\n'
		c = *p2;
		*p2 = 0;
// DrawFillRect(scr, NULL, RGB(255, 200, 200));
// nSDL_DrawString(scr, font, 0, 0, "nLines = %i\nwidth = %i", nLines, nSDL_GetStringWidth(font, p0));
// SDL_Flip(scr);
// wait_key_pressed();
		
		if (nSDL_GetStringWidth(font, p0) > W) { // si [p0->p2] dépasse, alors [p0->p1] forme une nouvelle ligne (<=> *p1 = 0)
			nLines++;
			
			if (p0 > p1) { // [p0->p2] forme une ligne trop grande
				p1 = p2+1;
				p0 = p1;
			}
			
			else {  // [p0->p1] forme une ligne de bonne taille
				// on pose p1 à 0
				*p1 = 0;
				
				// si p2 est un espace
				if (c == ' ') {
					*p2 = ' ';
					p0 = p1+1;
					p1 = p2;
				}
				
				// sinon
				else {
					nLines++;
					p1 = p2;
					p0 = p1+1;
				}
			}
		}
		
		else {
			if (c == ' ') {
				*p2 = ' ';
				p1 = p2;
			}
			
			else {
				nLines++;
				p1 = p2;
				p0 = p1+1;
			}
		}
	}
	
	// on ajoute la dernière ligne
	if (p0 > p1 || nSDL_GetStringWidth(font, p0) <= W) nLines++;
	else {
		nLines += 2;
		*p1 = 0;
	}
	
	
	/* 3-- On affiche le message */
	int x;
	SDL_Rect r = {0, 0, 226, 6+10*nLines};
	SDL_Rect clip;
	r.x = (scr->w - r.w) / 2;
	r.y = (scr->h - r.h) / 2;
	SDL_Surface *saveImage = NewSurface(r.w, r.h);
	DrawSurface(scr, &r, saveImage, NULL);
	SDL_GetClipRect(scr, &clip);
	SDL_SetClipRect(scr, &r);
	
	// on dessine le rectangle
	DrawFillRect(scr, &r, RGB(219,215,232));
	DrawRect(scr, &r, RGB(43,45,57));
	DrawRectXY(scr, r.x+1, r.y+1, r.w-2, r.h-2, RGB(100,103,130));
	
	
	// on affiche les lignes de texte
	p0 = buffer;
	for (x=0; x < nLines; x++) {
		DrawClippedStr(scr, font, (scr->w - nSDL_GetStringWidth(font,p0))/2, r.y+4+10*x, p0);
		p0 += strlen(p0)+1;
	}
	
	// on affiche et attend l'appui d'une touche
	SDL_SetClipRect(scr, &clip);
	SDL_Flip(scr);
	
	DrawSurface(saveImage, NULL, scr, &r);
	SDL_FreeSurface(saveImage);
	free(buffer);
	nSDL_FreeFont(font);
	
	wait_no_key_pressed();
	wait_key_pressed();
	wait_no_key_pressed();
	SDL_Flip(scr);
}





wMENU *wPopup(wMENU *m, int *n)
{
	return wPopupXY(m, -1, -1, n);
}


wMENU *wPopupXY(wMENU *m, int x, int y, int *n)
{
	*n = -1;
	if (!m) return NULL;
	int ok;
	int b = 0;
	wMENU *r = NULL;
	
	Widget *list = wBoxList(m, min(8, m->nItems), 0);
	if (x >= 0 && y >= 0) wSetSpot(list, x, y);
	wSetWidth(list, 180);
	wSetFreedArgs(list, WMENU);  // c'est à l'utilisateur de fermer le menu
	
	
	int CB_Click(Widget *w, int signal)
	{
		if (w && signal == SIGNAL_CLICK) {
			b=1;
			return ACTION_EXIT;
		}
		return ACTION_CONTINUE;
	}
	
	wConnect(list, CB_Click);
	
	ok = wActivateConstruct(list)-1;
	if (!ok || b) r = wList_GetCurrentItem(list, n);
	wCloseConstruct(list);
	return r;
}





static int wOpenBox_(const char *path, const char *pattern, char *fileName_output, int mode)
{
	int ok;
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_VGA, Red(Theme->color4), Green(Theme->color4), Blue(Theme->color4));
	BOOL b = pattern && pattern[0] && strcmp(pattern, "*");
	char fileName[512];
	strcpy(fileName, fileName_output);
		
	// Déclaration des widgets
	Widget *mainWidget = wDialog2("Open", "Open", "Cancel");
	Widget *body = wBasicLayout(0);
	Widget *label = wExLabel("", ALIGN_LEFT, font);
	Widget *explorer = wBoxExplorer((char *) path, (char *) pattern, b? 10:12);
	Widget *typeFile = NULL;
	
	wAddWidget(mainWidget, body);
	wAddWidget(body, label);
	wAddWidget(body, explorer);
	if (b) {
		typeFile = wRequest("Files :", pattern, REQUEST_ALL, 32);
		wAddWidget(body, typeFile);
	}

	
	wSetWidth(mainWidget, 300);
	wSetHeight(mainWidget, 210);
	wSetDynamism(typeFile, 0);
	
	// Fonctions Callbacks
	int CB_ChangeLabel(Widget *w, int signal)
	{
		if (w == explorer && signal == SIGNAL_ACTION) {
			wList_GetCurrentPath(explorer, fileName);
			if (!fileName[0]) strcpy(fileName, "/");
			char *p = fileName;
			
			// on vérifie que fileName n'est pas trop grand
			if (nSDL_GetStringWidth(font, fileName) > label->bounds.w-4) {
				do p = strchr(++p, '/');
				while (p && nSDL_GetStringWidth(font, p)+nSDL_GetStringWidth(font, "... ") > label->bounds.w-4);
				
				if (p) {
					memmove(fileName+4, p, strlen(p)+1);
					fileName[0] = '.', fileName[1] = '.', fileName[2] = '.', fileName[3] = ' ';
				}
			}
			
			// on change le texte du label
			wLabel_SetText(label, fileName);
			wDrawWidget(label);
		}
		return ACTION_CONTINUE;
	}
	
	wConnect(explorer, CB_ChangeLabel);
	
	
	// on active la construction
	wDrawConstruct(mainWidget);
	CB_ChangeLabel(explorer, SIGNAL_ACTION);
  ACTIVATE:
	ok = wActivateConstruct(mainWidget);
	
	if (ok) {
		wMENU *menu = wList_GetCurrentItem(explorer, NULL);
		if (!menu || !menu->nItems) {
			wFastMsg("No path selected.");
			goto ACTIVATE;
		}
		
		wList_GetCurrentPath(explorer, fileName);
		if (fileName[0] && fileName[strlen(fileName)-1] != '/') strcat(fileName, "/");
		strcat(fileName, wList_GetCurrentStr(explorer));
		
		if (isFile(fileName) && mode == 1) {
			wFastMsg("A folder path is expected.");
			goto ACTIVATE;
		}
		if (isFolder(fileName) && mode == 0) {
			wFastMsg("A file path is expected.");
			goto ACTIVATE;
		}
		
		strcpy(fileName_output, fileName);
	}
	
	wCloseConstruct(mainWidget);
	return ok;
}


// ouvre uniquement un fichier
int wOpenBox(const char *path, const char *pattern, char *fileName_output)
{
	return wOpenBox_(path, pattern, fileName_output, 0);
}

// ouvre uniquement un dossier
int wOpenBox2(const char *path, const char *pattern, char *fileName_output)
{
	return wOpenBox_(path, pattern, fileName_output, 1);
}

// ouvre un fichier ou un dossier
int wOpenBox3(const char *path, const char *pattern, char *fileName_output)
{
	return wOpenBox_(path, pattern, fileName_output, 2);
}



int wSaveBox(const char *path, char *fileName_output)
{
	int ok;
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_VGA, Red(Theme->color4), Green(Theme->color4), Blue(Theme->color4));
	char fileName[512];
	strcpy(fileName, fileName_output);
	
	// Déclaration des widgets
	Widget *mainWidget = wDialog2("Save", "Save", "Cancel");
	Widget *body = wBasicLayout(0);
	Widget *label = wExLabel("", ALIGN_LEFT, font);
	Widget *explorer = wBoxExplorer((char *) path, NULL, 10);
	Widget *req = wRequest(NULL, fileName, REQUEST_VAR, 31);
	
	wSetWidth(mainWidget, 300);
	wSetHeight(mainWidget, 210);
	
	wAddWidget(mainWidget, body);
	wAddWidget(body, label);
	wAddWidget(body, explorer);
	wAddWidget(body, req);
	wSetActiveWidget(req);
	
	
	// Fonctions Callback
	int CB_Explorer(Widget *w, int signal)
	{
		if (w != explorer) return ACTION_CONTINUE;
		
		if (signal == SIGNAL_CLICK) {
			// on modifie req
			wRequest_SetText(req, wList_GetCurrentStr(explorer));
			wDrawWidget(req);
		}
		
		else if (signal == SIGNAL_ACTION) {
			// on modifie label
			wList_GetCurrentPath(explorer, fileName);
			if (!fileName[0]) strcpy(fileName, "/");
			char *p = fileName;
			
			// on vérifie que fileName n'est pas trop grand
			if (nSDL_GetStringWidth(font, fileName) > label->bounds.w-4) {
				do {
					p++;
					p = strchr(p, '/');
				} while (p && nSDL_GetStringWidth(font, p)+nSDL_GetStringWidth(font, "... ") > label->bounds.w-4);
				
				if (p) {
					memmove(fileName+4, p, strlen(p)+1);
					fileName[0] = '.', fileName[1] = '.', fileName[2] = '.', fileName[3] = ' ';
				}
			}
			
			// on change le texte du label
			wLabel_SetText(label, fileName);
			fileName[0] = 0;
			wDrawWidget(label);
		}
		
		return ACTION_CONTINUE;
	}
	
	wConnect(explorer, CB_Explorer);
	
	
	// on active le construct
	wDrawConstruct(mainWidget);
	CB_Explorer(explorer, SIGNAL_ACTION);  // on définit le texte du label
	ok = wActivateConstruct(mainWidget);
	if (ok) {
		wList_GetCurrentPath(explorer, fileName);
		if (fileName[0] && fileName[strlen(fileName)-1] != '/') strcat(fileName, "/");
		strcat(fileName, wRequest_GetText(req));
		
		strcpy(fileName_output, fileName);
	}
	
	wCloseConstruct(mainWidget);
	return ok;
}












