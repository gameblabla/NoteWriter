#ifndef EASYFILES_H
#define EASYFILES_H


BOOL isFile(const char *path); // retourne 1 si path mène vers un fichier, 0 sinon
BOOL isFolder(const char *path); // retourne 1 si path mène vers un dossier, 0 sinon


int file_GetSize(const char *path);  // renvoie la taille totale du fichier
int file_GetaTime(const char *path);  // renvoie l'heure du dernier accès du fichier
int file_GetmTime(const char *path);  // renvoie l'heure de la dernière modification du fichier
int file_Remove(const char *path);


int folder_GetSize(const char *path);  // renvoie la taille totale de tous les fichiers contenus dans le dossier et ses sous-dossiers
int folder_GetNumberOfElements(const char *path);  // renvoie le nombre total d'éléments du dossier (tous fichiers et dossiers confondus)
int folder_GetElements(const char *path, const char *pattern, char **result);  // renvoie le nombre d'éléments et stocke dans result la liste de ses éléments
// result doit être défini de la sorte : char *folderList[MAX_FILES];    où MAX_FILES doit être connu
// ne pas oublier de libérer la mémoire des noms des éléments
int folder_Remove(const char *path);
BOOL matchPattern(const char *str, const char *pattern);


int cfgFile_GetInt(FILE *f, const char *fieldName);
char *cfgFile_GetStr(FILE *f, const char *fieldName);



/* COMPRESSION / DECOMPRESSION */
// STRATEGIES DE COMPRESSION
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_RLE                 3
#define Z_FIXED               4
#define Z_DEFAULT_STRATEGY    0


// OPTIONS D'INITIALISATION
#define Z_DEFAULT_COMPRESSION 6			// LEVEL			---- 1= best speed but weak compression, 9= best compression but low speed
#define Z_DEFLATED 8							// METHODE		---- Must be 8 in the zlib PC version. DelateInit2_ return Z_STREAM_ERROR in case of invalid argument.
#define Z_WINDOW_BITS 15					// WINDOW_BITS	---- Larger this value is, better is the compression and more the memory is used. Must be between 8 and 15.
#define Z_MEMORY_LEVEL 8					// MEM_LEVEL	---- Specifies how much memory should be allocated internally. 1 = min memory and compression, 9 = max memory and compression
#define Z_STRATEGY Z_DEFAULT_STRATEGY	// STRATEGY		---- Should be 0 for normal data.

#define CHUNK 16384



int deflateFile(FILE *source, FILE *dest, int level);
int inflateFile(FILE *source, FILE *dest);
int inflateFilePart(FILE *source, FILE *dest, int bytesToRead);

int Ice(const char *path);
int Warm(const char *iceFile);





#endif

