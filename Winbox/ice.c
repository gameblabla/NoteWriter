#include <os.h>
#include "EasyFiles.h"



int ice_addElement(FILE *f_output, char *elementPath);



int ice_addFile(FILE *f_output, char *filePath)
{
	// pas besoin de checker f_output et filePath
	FILE *f_input = fopen(filePath, "rb");
	if (!f_input) return -3;
	int n;
	char *p;
	int pos_fileSize;
	int pos_endData;
	
	
	// on écrit le type de l'élément (0 = fichier)
	n = 0;
	fwrite(&n, sizeof(int), 1, f_output);
	
	
	// on écrit le nom de l'élément
	p = strrchr(filePath, '/');
	p = (p? p+1 : filePath);
	fwrite(p, strlen(p)+1, 1, f_output);
	
	
	// on écrit la taille de l'élément (elle n'est pas encore connue, alors on retient la position actuelle)
	pos_fileSize = ftell(f_output);
	fwrite(&n, sizeof(int), 1, f_output);  // on met la taille à 0 en attendant
	
	
	// on écrit le fichier compressé
	int r = deflateFile(f_input, f_output, 6);
	pos_endData = ftell(f_output);
	n = pos_endData - pos_fileSize - sizeof(int);
	fseek(f_output, pos_fileSize, SEEK_SET);
	fwrite(&n, sizeof(int), 1, f_output);
	
	// on se place à la fin des data
	fseek(f_output, pos_endData, SEEK_SET);
	
	
	fclose(f_input);
	return 100*r;
}





int ice_addFolder(FILE *f_output, char *folderPath)
{
	int x;
	char elementPath[128];
	char *p;
	
	
	// on écrit le type de l'élément (1 = dossier)
	int n = 1;
	fwrite(&n, sizeof(int), 1, f_output);
	
	
	// on écrit le nom du dossier
	p = strrchr(folderPath, '/');
	p = (p? p+1 : folderPath);
	fwrite(p, strlen(p)+1, 1, f_output);
	
	
	// on écrit le nombre de fichier
	char *elements[512];
	n = folder_GetElements(folderPath, NULL, elements);
	fwrite(&n, sizeof(int), 1, f_output);
	
	
	// on écrit les sous-éléments
	for (x=0; x < n; x++) {
		if (folderPath[strlen(folderPath)] == '/')	sprintf(elementPath, "%s%s", folderPath, elements[x]);
		else														sprintf(elementPath, "%s/%s", folderPath, elements[x]);
		free(elements[x]);
		ice_addElement(f_output, elementPath);
	}
	
	return 0;
}



int ice_addElement(FILE *f_output, char *elementPath)
{
	if (isFile(elementPath)) {
		return ice_addFile(f_output, elementPath);
	}
	
	else if (isFolder(elementPath)) {
		return ice_addFolder(f_output, elementPath);
	}
	
	else return -1;
}




int Ice(const char *elementPath)
{
	char *path = (char *) elementPath;
	char iceFile[128];
	char *p;
	int r;
	int isDir = 0;
	
	
	// on créé le fichier output
	if (isFile(path)) {
		strcpy(iceFile, path);
		p = strstr(iceFile, ".tns");
		if (p) *p = 0;
		p = strrchr(iceFile, '.');
		if (p) *p = 0;
		strcat(iceFile, ".ice.tns");
		
		// on ouvre le fichier output
		FILE *f_output = fopen(iceFile, "wb+");
		if (!f_output) return -2;
		
		
		// on ajoute l'élément
		r = ice_addElement(f_output, path);
		fclose(f_output);
	}
	
	
	else if (isFolder(path)) {
		char tmpFile[128];
		strcpy(tmpFile, path);
		strcat(tmpFile, ".ice.tmp");
		strcpy(iceFile, path);
		strcat(iceFile, ".ice.tns");
		isDir = 1;
		
		// on ouvre les fichiers output
		FILE *f_tmp = fopen(tmpFile, "wb+");
		if (!f_tmp) return -2;
		FILE *f_ice = fopen(iceFile, "wb");
		if (!f_ice) {fclose(f_tmp); return -3;}
		
		// on compresse une première fois (path --> tmp)
		r = ice_addElement(f_tmp, path);
		
		// on compresse une deuxième fois (tmp --> ice)
		if (!r) {
			fseek(f_tmp, 0, SEEK_SET);  // on revient au début du fichier pour la lecture
			fwrite(&isDir, sizeof(int), 1, f_ice);
			r = deflateFile(f_tmp, f_ice, 6);
		}
		
		// on ferme et supprime le fichier temporaire
		fclose(f_tmp);
		fclose(f_ice);
		remove(tmpFile);
	}
	
	else return -1;
	
	return r;
}







