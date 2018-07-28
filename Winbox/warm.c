#include <os.h>
#include "EasyFiles.h"


int warm_element(FILE *f_iced, char *folder);



int warm_file(FILE *f_iced, char *filePath, int dataSize)
{
	// on est positionné, dans f_iced, au niveau des données à décompresser
	FILE *f_output = fopen(filePath, "wb");
	if (!f_output) return -53;
	
	// on décompresse dans f_output
	int r = inflateFilePart(f_iced, f_output, dataSize);
	
	fclose(f_output);
	return 1000*r;
}



int warm_folder(FILE *f_iced, char *folderPath, int dataSize)
{
	// on est positionné, dans f_iced, au niveau des données à décompresser
	// on créé le dossier
	mkdir(folderPath, 777);
	
	// on se place dedans
	strcat(folderPath, "/");
	
	// on décompresse les data du folder
	int x;
	int r;
	for (x=0; x < dataSize; x++) {
		r = warm_element(f_iced, folderPath);
		if (r < 0) return r;
	}
	return 0;
}








int warm_element(FILE *f_iced, char *folder)
{
	int type, dataSize;
	char name[128];
	int x;
	int r;
	
	// on lit le type de l'élément
	fread(&type, sizeof(int), 1, f_iced);
	
	// on lit le nom
	strcpy(name, folder);
	x = strlen(name);
	while ( (name[x++]=fgetc(f_iced)) );
	
	// on lit la taille des data
	fread(&dataSize, sizeof(int), 1, f_iced);
	
	
	if (type == 0) {  // si on a affaire à un fichier
		r = warm_file(f_iced, name, dataSize);
	}
	
	else if (type == 1) {  // si on a affaire à un dossier
		r = warm_folder(f_iced, name, dataSize);
	}
	
	else r = -97;
	
	return r;
}





int Warm(const char *iceFile)
{
	if (!isFile(iceFile) || !strstr(iceFile, ".ice")) return -1;
	
	FILE *f_iced = fopen(iceFile, "rb");
	char folder[128];
	char *p;
	int isDir;
	int r;
	
	// on trouve le dossier du fichier
	strcpy(folder, iceFile);
	p = strrchr(folder, '/');
	if (p) p[1] = 0;  // folder doit se terminer par un '/'
	else strcpy(folder, "./");
	
	// on teste si c'est un simple fichier ou un dossier
	fread(&isDir, sizeof(int), 1, f_iced);
	
	if (isDir) {
		// on créé/ouvre le fichier temporaire
		char tmpFile[128];
		sprintf(tmpFile, "%sice.tmp", folder);
		FILE *f_tmp = fopen(tmpFile, "wb+");
		
		if (f_tmp) {
			// on décompresse une première fois
			r = inflateFile(f_iced, f_tmp);
			
			// on décompresse une deuxième fois
			fseek(f_tmp, 0, SEEK_SET);  // on revient au début pour la lecture
			if (r >= 0) r = warm_element(f_tmp, folder);
			
			// on supprime le fichier temporaire
			fclose(f_tmp);
			remove(tmpFile);
		}
		
		else r = -4;
	}
	
	else {
		// on revient au début du fichier
		fseek(f_iced, 0, SEEK_SET);
		r = warm_element(f_iced, folder);
	}
	
	
	fclose(f_iced);
	return r;
}


