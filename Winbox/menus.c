#include <os.h>
#include <SDL/SDL.h>
#include <Winbox.h>

#include "menus.h"
#include "structures.h"
#include "scrollbar.h"
#include "EasyFiles.h"
#include "general.h"
#include "dialogs.h"



// ------- Méthodes de CREATION / DESTRUCTION


wMENU *wMenu(const char *title)
{
	wMENU *m = malloc(sizeof(wMENU));
	m->magicNumber = 1;
	m->nItems = 0;
	m->items = NULL;
	m->nSelected = 0;
	m->parent = NULL;
	if (title) {
		m->title = malloc(strlen(title)+1);
		strcpy(m->title, title);
	}
	else m->title = NULL;
	
	return m;
}

wMENU *wMenu_AddItemList(wMENU *m, const char *str)
{
	if (!m) m = wMenu(NULL);
	if (!str) return m;
	
	char *p;
	char ts[128];
	ts[127] = 0;
	
	while ((p = strchr(str, '|'))) {
		strncpy(ts, str, min(127, p-str));
		ts[p-str] = 0;
		wMenu_AddItem(m, ts);
		str = p+1;
	}
	wMenu_AddItem(m, str);
	
	return m;
}


wMENU *wMenu_AddItem(wMENU *m, const char *str)
{
	if (!m) m = wMenu(NULL);
	if (!str) return m;
	
	char *itemStr = malloc(strlen(str)+1);
	if (!itemStr) return m;
	strcpy(itemStr, str);
	
	// on créé l'espace associé au nouvel item
	wITEM *itm = malloc(sizeof(wITEM));
	if (!itm) {
		free(itemStr);
		return m;
	}
	
	// et on le remplit
	itm->magicNumber = 2;
	itm->str = itemStr;
	itm->isSelected = 0;
	
	
	// on rajoute le nouveau slot dans m->items
	void *tmp = realloc(m->items, sizeof(void *)*(m->nItems+1));
	if (!tmp) {
		free(itemStr);
		free(itm);
		return m;
	}
	m->items = tmp;
	m->items[m->nItems] = itm;
	m->nItems++; // opération réussie : on incrémente le nombre d'items !
	
	return m;
}

void wMenu_SetItem(wMENU *m, const char *str, int nItem)
{
	if (!m || !str) return;
	if (nItem >= m->nItems) wMenu_AddItem(m, str);
	else {
		wITEM *itm = m->items[nItem];
		
		char *itemStr = malloc(strlen(str)+1);
		if (!itemStr) return;
		strcpy(itemStr, str);
		
		free(itm->str);
		itm->str = itemStr;
	}
}


void wMenu_SetTitle(wMENU *m, const char *str)
{
	free(m->title);
	m->title = malloc(strlen(str)+1);
	strcpy(m->title, str);
}


wMENU *wMenu_AddMenu(wMENU *parent, wMENU *m)
{
	if (!parent) parent = wMenu(NULL);
	if (!m) return parent;
	
	void *tmp = realloc(parent->items, sizeof(void *)*(parent->nItems+1));
	if (!tmp) return parent;
	
	// on ajoute le menu
	parent->items = tmp;
	parent->items[parent->nItems] = m;
	parent->nItems++;
	m->parent = parent;
	
	return parent;
}



static void AddFolderToMenu(wMENU *m, const char *folder, const char *pattern, BOOL onlyFolders, int cDepth, int depth)
{
	if (depth && cDepth == depth) return;
	char path[512];
	int x;
	wMENU *sm;
	char *files[512];
	int numFiles = folder_GetElements(folder, pattern, files);
	// wFastMsg("folder = %s\nnumFiles = %i", folder, numFiles);
	
	
	// on ajoute les items au menu
	for (x=0; x<numFiles; x++) {
		// on obtient le chemin complet de l'élément
		strcpy(path, folder);
		if (!path[0] || path[strlen(path)-1] != '/') strcat(path, "/");
		strcat(path, files[x]);
		
		// on l'ajoute
		if (isFile(path)) { // si on a un fichier
			if (!onlyFolders) wMenu_AddItem(m, files[x]);
		}
		
		else if (isFolder(path)) {  // si on a un dossier
			if (onlyFolders)
				wMenu_AddItem(m, files[x]);
			
			else if (depth <= 0 || cDepth+1 < depth) {  // on n'affiche pas les dossiers qui mèneraient vers des fichiers trop profonds
					sm = wMenu(files[x]);
					AddFolderToMenu(sm, path, pattern, onlyFolders, cDepth+1, depth);
					wMenu_AddMenu(m, sm);
			}
		}
		
		else wFastMsg("ERREUR\nL'element n'est ni un fichier ni un dossier :\n%s", path);
		
		
		
		free(files[x]);
	}
}


wMENU *wMenu_FileList(const char *title, const char *path, const char *pattern, int depth)
{
	wMENU *m = wMenu(title);
	if (!m) return NULL;
	
	char folder[512] = "/documents";
	if (path && path[0]) strcpy(folder, path);
	
	AddFolderToMenu(m, folder, pattern, 0, 0, depth);
	return m;
}



wMENU *wMenu_FolderList(const char *title, const char *path, const char *pattern)
{
	wMENU *m = wMenu(title);
	if (!m) return NULL;
	
	char folder[512] = "/documents";
	if (path && path[0]) strcpy(folder, path);
	
	AddFolderToMenu(m, folder, pattern, 1, 0, 1);
	return m;
}


wMENU *wMenu_NewItemList(const char *title, const char *str)
{
	wMENU *m = wMenu(title);
	return wMenu_AddItemList(m, str);
}


// Fermeture
void wMenu_Close(wMENU *m)
{
	if (!m) return;
	if (m->items && m->nItems) {
		int x;
		for (x=0; x < m->nItems; x++) {
			wMENU *sm = m->items[x];
			if (sm->magicNumber == 1) {
				free(sm->title);
				wMenu_Close(sm);
			}
			else {
				free(((wITEM *) sm)->str);
				free(sm);
			}
		}
		free(m->items);
	}
	free(m);
}



// Setters / Getters
int wMenu_GetSelectedItem(wMENU *m)
{
	if (!m) return -1;
	wITEM *itm;
	int x;
	
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 2)
			if (itm->isSelected) return x;
	}
	
	return -1;
}


char *wMenu_GetSelectedStr(wMENU *m)
{
	if (!m) return NULL;
	int x;
	wITEM *itm;
	wMENU *menu;
	
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 2)
			if (itm->isSelected) return itm->str;
	}
	
	// si on n'a pas d'item sélectionné, on regarde les sous-menus
	char *s;
	for (x=0; x < m->nItems; x++) {
		menu = m->items[x];
		if (menu->magicNumber == 1) {
			s = wMenu_GetSelectedStr(menu);
			if (s) return s;
		}
	}
	
	return NULL;
}

int wMenu_GetItemFromStr(wMENU *m, const char *itemStr)
{
	if (!m) return -1;
	int x = 0;
	wITEM *itm;
	
	for (x=0; x < m->nItems; x++) {
		itm = m->items[x];
		if (itm->magicNumber == 2 && !strcmp(itm->str, itemStr)) return x;
	}
	return -1;
}



void wMenu_SelectItem(wMENU *m, int item)
{
	if (!m) return;
	if (item < 0 || item > m->nItems) return;
	wITEM *itm = m->items[item];
	if (itm->magicNumber == 2)
		itm->isSelected = !itm->isSelected;
	
	if (itm->isSelected) m->nSelected++;
	else m->nSelected--;
}


BOOL wMenu_IsSelected(wMENU *m, int item)
{
	if (!m) return 0;
	if (item < 0 || item > m->nItems) return 0;
	wITEM *itm = m->items[item];
	if (itm->magicNumber == 1) return 0;
	return itm->isSelected;
}


void wMenu_DeselectAll(wMENU *m)
{
	if (!m) return;
	int x;
	wMENU *sm;
	wITEM *itm;
	m->nSelected = 0;
	
	// on déselectionne les sous-menus
	for (x=0; x < m->nItems; x++) {
		sm = m->items[x];
		if (sm->magicNumber == 1)
			wMenu_DeselectAll(sm);
		else {
			itm = (wITEM *) sm;
			itm->isSelected = 0;
		}
	}
}


BOOL wMenu_HasSelectedItemOrMenu(wMENU *m)
{
	if (!m) return 0;
	if (m->nSelected) return 1;
	int x;
	wMENU *menu;
	BOOL b;
	
	for (x=0; x < m->nItems; x++) {
		menu = m->items[x];
		if (menu->magicNumber == 1) {
			b = wMenu_HasSelectedItemOrMenu(menu);
			if (b) return b;
		}
	}
	
	return 0;
}



