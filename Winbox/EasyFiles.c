#include <os.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <dirent.h>
#include "EasyFiles.h"
#include "dialogs.h"



// cette fonction interne est utilisée pour celle qui suit.
// Diviser pour conquérir !!
static BOOL matchSequence(char **str, char **pattern)
{
	char *s=*str, *p=*pattern;
	int x;
	while (*p && (*p == '*' || *p == '?')) {
		if (*p == '?' && *s) s++;
		else if (*p == '?') return 0;
		p++;
	}
	
	if (*p == 0) {
		*str = s;
		*pattern = p;
		return 1;
	}
	
	
	// on arrive au début de la séquence
	while (*s) {
		// on cherche tout d'abord le premier élément du pattern
		while (*s && *s != *p) s++;
		if (*s == 0) return 0;
		
		// ici on a *s == *p
		// on cherche si à partir de s toute la séquence est vérifiée
		x = 0;
		while (s[x] && p[x] && p[x] != '*') {
			if (p[x] != '?' && p[x] != s[x]) break;
			x++;
		}
		
		// si on est arrivé au bout du pattern on a fini
		if ((!p[x] && !s[x]) || p[x] == '*') {
			*str = s+x;
			*pattern = p+x;
			return 1;
		}
		
		// si on est arrivé au bout de la chaîne sans être à la fin du pattern
		// ou à la fin du pattern sans la fin de la chaîne, alors pas de match
		if (!p[x] || !s[x]) return 0;
		
		// sinon, on cherche un autre premier caractère
		s++;
	}
	
	return 0;
}



BOOL matchPattern(const char *str, const char *pattern)
{
	if (str == pattern || !pattern) return 1;
	if (!str) return 0;
	char *s = (char *) str;
	char *p = (char *) pattern;
	
	while (*p != '*' && *p && *s) {
		if (*p != '?' && *p != *s) return 0;
		p++, s++;
	}
	if (*p == 0) return 1;
	if (*s == 0) return 0;
	
	
	// on arrive à la première étoile
	while (matchSequence(&s, &p))
		if (*p == 0) return 1;
	
	return 0;
}





BOOL isFile(const char *path)
{
	struct stat st;
	if (stat(path, &st)) return 0;
	return S_ISREG(st.st_mode);
}


BOOL isFolder(const char *path)
{
	struct stat st;
	if (stat(path, &st)) return 0;
	return S_ISDIR(st.st_mode);
}




int file_GetSize(const char *path)
{
	struct stat st;
	if (stat(path, &st)) return -1;
	return st.st_size;
}



int file_GetaTime(const char *path)
{
	struct stat st;
	if (stat(path, &st)) return -1;
	return st.st_atime;
}


int file_GetmTime(const char *path)
{
	struct stat st;
	if (stat(path, &st)) return -1;
	return st.st_mtime;
}


int file_Remove(const char *path)
{
	if (path && isFile(path)) return remove(path);
	return -1;
}

int folder_Remove(const char *path)  // Remove a folder and all his files
{
	if (!path || strlen(path) < 1 || !isFolder(path)) return -1;
	
	char *elements[512];
	char subPath[128];
	int n = folder_GetElements(path, NULL, elements);
	int x;
	int r = 0;
	
	// on supprime tous les sous-éléments
	for (x=0; x < n; x++) {
		// on obtient le sous-élément
		strcpy(subPath, path);
		if (path[strlen(path)] != '/') strcat(subPath, "/");
		strcat(subPath, elements[x]);
		free(elements[x]);
		
		// on le supprime
		if (isFile(subPath)) r |= file_Remove(subPath);
		else if (isFolder(subPath)) r |= folder_Remove(subPath);
	}
	
	// on supprime le dossier
	rmdir(path);
	return r;
}


int folder_GetSize(const char *path)
{
	int size = 0;
	DIR *folder;
	struct dirent *elt;
	char p[512];
	int r = 0;
	
	folder = opendir(path);
	if (!folder) return -1;
	
	readdir(folder),readdir(folder);  // on enlève les deux premiers cas
	
	while ((elt = readdir(folder))) {
		strcpy(p, path);
		if (!p[0] || p[strlen(p)-1] != '/') strcat(p, "/");
		strcat(p, elt->d_name);
		
		if (isFolder(p))	r = folder_GetSize(p);
		else					r = file_GetSize(p);
		if (r < 0) break;
		size += r;
	}
	
	closedir(folder);
	return r? r : size;
}




int folder_GetNumberOfElements(const char *path)
{
	int n = -2;
	DIR *folder = opendir(path);
	if (!folder) return -1;
	
	while (readdir(folder)) n++;
	
	closedir(folder);
	return n;
}




int folder_GetElements(const char *path, const char *pattern, char **result)
{
	if (!path || !result) return -1;
	int x, n = 0;
	DIR *folder = opendir(path);
	struct dirent *elt;
	if (!folder) return -1;
	char pathElt[256];
	char *pname;
	int isCapped;
	
	// on passe les éléments "." et ".."
	readdir(folder);
	readdir(folder);
	
	// pour trier dans l'ordre alphabétique, on va créer une liste chaînée des élements !
	struct CHAIN_LIST {
		char *name;
		struct CHAIN_LIST *previous;
		struct CHAIN_LIST *next;
		int isCapped;
	};
	
	struct CHAIN_LIST *first = NULL;
	struct CHAIN_LIST *cl;
	struct CHAIN_LIST *newcl;
	
	
	// on trouve les éléments
	while ((elt = readdir(folder))) {
		// on obtient le chemin complet de l'élément
		strcpy(pathElt, path);
		if (!path[0] || path[strlen(path)-1] != '/') strcat(pathElt, "/");
		strcat(pathElt, elt->d_name);
		
		// si c'est un fichier, on vérifie que le pattern match avec le nom du fichier
		if (isFolder(pathElt) || matchPattern(elt->d_name, pattern)) {
			pname = malloc(strlen(elt->d_name)+1);
			strcpy(pname, elt->d_name);
			n++;
			if (pname[0] >= 'A' && pname[0] <= 'Z') {
				isCapped = 1;
				pname[0] += 'a' - 'A';
			}
			else isCapped = 0;

// wFastMsg("n = %i\npname = %s", n, pname);			
			// puis on enregistre le résultat dans la liste chaînée
			if (!first) {
				first = malloc(sizeof(struct CHAIN_LIST));
				first->name = pname;
				first->previous = NULL;
				first->next = NULL;
				first->isCapped = isCapped;
			}
			
			else {
				cl = first;
				while (cl->next && strcmp(pname, cl->name) > 0)  // tant que pname est alphabétiquement 'plus grand'
					cl = cl->next;
				
				if (!cl->next && strcmp(pname, cl->name) > 0) {
					// alors pname est la plus 'grande' des chaînes
					newcl = malloc(sizeof(struct CHAIN_LIST));
					newcl->name = pname;
					newcl->previous = cl;
					newcl->next = NULL;
					newcl->isCapped = isCapped;
					cl->next = newcl;
				}
				
				else  {
					// alors pname se situe juste avant cl
					newcl = malloc(sizeof(struct CHAIN_LIST));
					newcl->name = pname;
					newcl->previous = cl->previous;
					if (newcl->previous) newcl->previous->next = newcl;
					newcl->next = cl;
					newcl->isCapped = isCapped;
					cl->previous = newcl;
					if (first == cl) first = newcl;
				}
			}
		}
	}
	closedir(folder);
	
	
	// on les stocke ensuite dans result ; ils sont alors classés alphabétiquement
	newcl = first;
	for (x=0; x < n; x++) {
		result[x] = newcl->name;  // on stocke le résultat
		if (newcl->isCapped) (result[x])[0] -= 'a' - 'A';
		cl = newcl;
		newcl = newcl->next;  // on passe au prochain path
		free(cl);  // on libère la CHAIN_LIST
	}
	
	return n;
}









// Fonctions pour accéder aux données d'un fichier de configuration
/* Exemple de contenu d'un fichier config :
debug=1
nbOfBytes=4
trustOS=2
OSname=HenriIV
*/
int cfgFile_GetInt(FILE *f, const char *field)
{
	char str[128], rfield[32];
	char *p;
	sprintf(rfield, "%s=", field);
	fseek(f, 0, SEEK_SET);
	
	do {
		if (!fgets(str, 128, f)) return -1;
		p = strstr(str, rfield);

		
		if (p == str) {
			int x=strlen(rfield), n=0;
			while (str[x]>='0' && str[x]<='9') n = 10*n+str[x++]-'0';
			
			return n;
		}
	} while (!feof(f));
	
	return -1;
}




char *cfgFile_GetStr(FILE *f, const char *field)
{
	char str[512], rfield[32];
	char *p;
	sprintf(rfield, "%s=", field);
	fseek(f, 0, SEEK_SET);
	char c;
	
	do {
		if (!fgets(str, 512, f)) return NULL;
		p = strstr(str, rfield);

		
		if (p == str) {
			int x=strlen(rfield), y=0, z;
			
			do {
				c = str[x+(y++)];
			} while (c>10 && c<255);
			
			char *result = malloc(y);

			for (z=0; z<y; z++) result[z] = str[x+z];
			if (y) result[y-1] = 0;
			return result;
		}
	} while (!feof(f));
	
	return NULL;
}



