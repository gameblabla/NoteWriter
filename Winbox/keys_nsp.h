#ifndef KEYS_H
#define KEYS_H

#include <nucleus.h>
#include <keys.h>
#include <libndls.h>

char K_getAlphaKey(void);
char K_getNumericKey(void);
char K_getPuncKey(void);
BOOL K_CLICK();
BOOL K_ESC();
BOOL K_ENTER();
BOOL K_DEL();
BOOL K_MENU();
BOOL K_TAB();
BOOL K_CTRL();
BOOL K_SHIFT();
BOOL K_CAPS();
BOOL K_VAR();
BOOL K_HOME();
BOOL K_DOC();
BOOL K_SCRATCHPAD();
BOOL K_UP();
BOOL K_LEFT();
BOOL K_DOWN();
BOOL K_RIGHT();
BOOL K_PUNC();
BOOL K_PARAGRAPH();


// Fonction nouvelle génération !!
void K_getChar(char *keyList);

#endif

