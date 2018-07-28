#include <os.h>
#include <libndls.h>
#include <nucleus.h>
#include "keys_nsp.h"


char K_getAlphaKey(void){
	if (isKeyPressed(KEY_NSPIRE_A)) return 'a';
	if (isKeyPressed(KEY_NSPIRE_B)) return 'b';
	if (isKeyPressed(KEY_NSPIRE_C)) return 'c';
	if (isKeyPressed(KEY_NSPIRE_D)) return 'd';
	if (isKeyPressed(KEY_NSPIRE_E)) return 'e';
	if (isKeyPressed(KEY_NSPIRE_F)) return 'f';
	if (isKeyPressed(KEY_NSPIRE_G)) return 'g';
	if (isKeyPressed(KEY_NSPIRE_H)) return 'h';
	if (isKeyPressed(KEY_NSPIRE_I)) return 'i';
	if (isKeyPressed(KEY_NSPIRE_J)) return 'j';
	if (isKeyPressed(KEY_NSPIRE_K)) return 'k';
	if (isKeyPressed(KEY_NSPIRE_L)) return 'l';
	if (isKeyPressed(KEY_NSPIRE_M)) return 'm';
	if (isKeyPressed(KEY_NSPIRE_N)) return 'n';
	if (isKeyPressed(KEY_NSPIRE_O)) return 'o';
	if (isKeyPressed(KEY_NSPIRE_P)) return 'p';
	if (isKeyPressed(KEY_NSPIRE_Q)) return 'q';
	if (isKeyPressed(KEY_NSPIRE_R)) return 'r';
	if (isKeyPressed(KEY_NSPIRE_S)) return 's';
	if (isKeyPressed(KEY_NSPIRE_T)) return 't';
	if (isKeyPressed(KEY_NSPIRE_U)) return 'u';
	if (isKeyPressed(KEY_NSPIRE_V)) return 'v';
	if (isKeyPressed(KEY_NSPIRE_W)) return 'w';
	if (isKeyPressed(KEY_NSPIRE_X)) return 'x';
	if (isKeyPressed(KEY_NSPIRE_Y)) return 'y';
	if (isKeyPressed(KEY_NSPIRE_Z)) return 'z';
	return 0;
}

char K_getNumericKey(void){
	if (isKeyPressed(KEY_NSPIRE_0)) return '0';
	if (isKeyPressed(KEY_NSPIRE_1)) return '1';
	if (isKeyPressed(KEY_NSPIRE_2)) return '2';
	if (isKeyPressed(KEY_NSPIRE_3)) return '3';
	if (isKeyPressed(KEY_NSPIRE_4)) return '4';
	if (isKeyPressed(KEY_NSPIRE_5)) return '5';
	if (isKeyPressed(KEY_NSPIRE_6)) return '6';
	if (isKeyPressed(KEY_NSPIRE_7)) return '7';
	if (isKeyPressed(KEY_NSPIRE_8)) return '8';
	if (isKeyPressed(KEY_NSPIRE_9)) return '9';
	return 0;
}

char K_getPuncKey(void){
	if (isKeyPressed(KEY_NSPIRE_SPACE))		return ' ';
	if (isKeyPressed(KEY_NSPIRE_PLUS))		return '+';
	if (isKeyPressed(KEY_NSPIRE_MINUS))		return '-';
	if (isKeyPressed(KEY_NSPIRE_MULTIPLY))	return '*';
	if (isKeyPressed(KEY_NSPIRE_DIVIDE))	return '/';
	if (isKeyPressed(KEY_NSPIRE_COMMA))		return ',';
	if (isKeyPressed(KEY_NSPIRE_COLON))		return ':';
	if (isKeyPressed(KEY_NSPIRE_RP))			return ')';
	if (isKeyPressed(KEY_NSPIRE_LP))			return '(';
	if (isKeyPressed(KEY_NSPIRE_PERIOD))	return '.';
	if (isKeyPressed(KEY_NSPIRE_NEGATIVE))	return 250;
	if (isKeyPressed(KEY_NSPIRE_EQU))		return '=';
	
	return 0;
}

BOOL K_PARAGRAPH()
{ return isKeyPressed(KEY_NSPIRE_RET); }

BOOL K_PUNC()
{ return isKeyPressed(KEY_NSPIRE_QUESEXCL); }

BOOL K_CLICK()
{ return isKeyPressed(KEY_NSPIRE_CLICK); }

BOOL K_ESC()
{ return isKeyPressed(KEY_NSPIRE_ESC); }

BOOL K_ENTER()
{ return isKeyPressed(KEY_NSPIRE_ENTER); }

BOOL K_DEL()
{ return isKeyPressed(KEY_NSPIRE_DEL); }

BOOL K_MENU()
{ return isKeyPressed(KEY_NSPIRE_MENU); }

BOOL K_TAB()
{ return (has_colors && isKeyPressed(KEY_NSPIRE_TAB)); }

BOOL K_CTRL()
{ return isKeyPressed(KEY_NSPIRE_CTRL); }

BOOL K_SHIFT()
{ return isKeyPressed(KEY_NSPIRE_SHIFT); }

BOOL K_CAPS()
{ 
	return 0;
	//return isKeyPressed(KEY_NSPIRE_CAPS); 
}

BOOL K_VAR()
{ return isKeyPressed(KEY_NSPIRE_VAR); }

BOOL K_HOME()
{ return isKeyPressed(KEY_NSPIRE_HOME); }

BOOL K_DOC()
{ return isKeyPressed(KEY_NSPIRE_DOC); }

BOOL K_SCRATCHPAD()
{
	return isKeyPressed(KEY_NSPIRE_SCRATCHPAD) || isKeyPressed(KEY_NSPIRE_eEXP);
}

BOOL K_UP()
{ return isKeyPressed(KEY_NSPIRE_UPRIGHT) || isKeyPressed(KEY_NSPIRE_UP) || isKeyPressed(KEY_NSPIRE_LEFTUP); }
BOOL K_LEFT()
{ return isKeyPressed(KEY_NSPIRE_LEFTUP) || isKeyPressed(KEY_NSPIRE_LEFT) || isKeyPressed(KEY_NSPIRE_DOWNLEFT); }
BOOL K_DOWN()
{ return isKeyPressed(KEY_NSPIRE_RIGHTDOWN) || isKeyPressed(KEY_NSPIRE_DOWN) || isKeyPressed(KEY_NSPIRE_DOWNLEFT); }
BOOL K_RIGHT()
{ return isKeyPressed(KEY_NSPIRE_UPRIGHT) || isKeyPressed(KEY_NSPIRE_RIGHT) || isKeyPressed(KEY_NSPIRE_RIGHTDOWN); }







// -----------------!!!WARNING!!!-----------------------
// !!!! NOUVELLE GENERATION DE FONCTIONS DE TOUCHE !!!!
// -----------------!!!WARNING!!!-----------------------
// keyList doit être un tableau de 4 octets

static void K_pushChar(char *keyList, char c)
{
	int x=0;
	while (x<4 && keyList[x]) x++;
	if (x<4) keyList[x] = c;
}


static void K_pushAlphaKey(char *keyList){
	if (isKeyPressed(KEY_NSPIRE_A)) K_pushChar(keyList,'a');
	if (isKeyPressed(KEY_NSPIRE_B)) K_pushChar(keyList,'b');
	if (isKeyPressed(KEY_NSPIRE_C)) K_pushChar(keyList,'c');
	if (isKeyPressed(KEY_NSPIRE_D)) K_pushChar(keyList,'d');
	if (isKeyPressed(KEY_NSPIRE_E)) K_pushChar(keyList,'e');
	if (isKeyPressed(KEY_NSPIRE_F)) K_pushChar(keyList,'f');
	if (isKeyPressed(KEY_NSPIRE_G)) K_pushChar(keyList,'g');
	if (isKeyPressed(KEY_NSPIRE_H)) K_pushChar(keyList,'h');
	if (isKeyPressed(KEY_NSPIRE_I)) K_pushChar(keyList,'i');
	if (isKeyPressed(KEY_NSPIRE_J)) K_pushChar(keyList,'j');
	if (isKeyPressed(KEY_NSPIRE_K)) K_pushChar(keyList,'k');
	if (isKeyPressed(KEY_NSPIRE_L)) K_pushChar(keyList,'l');
	if (isKeyPressed(KEY_NSPIRE_M)) K_pushChar(keyList,'m');
	if (isKeyPressed(KEY_NSPIRE_N)) K_pushChar(keyList,'n');
	if (isKeyPressed(KEY_NSPIRE_O)) K_pushChar(keyList,'o');
	if (isKeyPressed(KEY_NSPIRE_P)) K_pushChar(keyList,'p');
	if (isKeyPressed(KEY_NSPIRE_Q)) K_pushChar(keyList,'q');
	if (isKeyPressed(KEY_NSPIRE_R)) K_pushChar(keyList,'r');
	if (isKeyPressed(KEY_NSPIRE_S)) K_pushChar(keyList,'s');
	if (isKeyPressed(KEY_NSPIRE_T)) K_pushChar(keyList,'t');
	if (isKeyPressed(KEY_NSPIRE_U)) K_pushChar(keyList,'u');
	if (isKeyPressed(KEY_NSPIRE_V)) K_pushChar(keyList,'v');
	if (isKeyPressed(KEY_NSPIRE_W)) K_pushChar(keyList,'w');
	if (isKeyPressed(KEY_NSPIRE_X)) K_pushChar(keyList,'x');
	if (isKeyPressed(KEY_NSPIRE_Y)) K_pushChar(keyList,'y');
	if (isKeyPressed(KEY_NSPIRE_Z)) K_pushChar(keyList,'z');
}

static void K_pushNumericKey(char *keyList){
	if (isKeyPressed(KEY_NSPIRE_0)) K_pushChar(keyList,'0');
	if (isKeyPressed(KEY_NSPIRE_1)) K_pushChar(keyList,'1');
	if (isKeyPressed(KEY_NSPIRE_2)) K_pushChar(keyList,'2');
	if (isKeyPressed(KEY_NSPIRE_3)) K_pushChar(keyList,'3');
	if (isKeyPressed(KEY_NSPIRE_4)) K_pushChar(keyList,'4');
	if (isKeyPressed(KEY_NSPIRE_5)) K_pushChar(keyList,'5');
	if (isKeyPressed(KEY_NSPIRE_6)) K_pushChar(keyList,'6');
	if (isKeyPressed(KEY_NSPIRE_7)) K_pushChar(keyList,'7');
	if (isKeyPressed(KEY_NSPIRE_8)) K_pushChar(keyList,'8');
	if (isKeyPressed(KEY_NSPIRE_9)) K_pushChar(keyList,'9');
}

static void K_pushPuncKey(char *keyList){
	if (isKeyPressed(KEY_NSPIRE_SPACE))		K_pushChar(keyList,' ');
	if (isKeyPressed(KEY_NSPIRE_PLUS))		K_pushChar(keyList,'+');
	if (isKeyPressed(KEY_NSPIRE_MINUS))		K_pushChar(keyList,'-');
	if (isKeyPressed(KEY_NSPIRE_MULTIPLY))	K_pushChar(keyList,'*');
	if (isKeyPressed(KEY_NSPIRE_DIVIDE))	K_pushChar(keyList,'/');
	if (isKeyPressed(KEY_NSPIRE_COMMA))		K_pushChar(keyList,',');
	if (isKeyPressed(KEY_NSPIRE_COLON))		K_pushChar(keyList,':');
	if (isKeyPressed(KEY_NSPIRE_RP))			K_pushChar(keyList,')');
	if (isKeyPressed(KEY_NSPIRE_LP))			K_pushChar(keyList,'(');
	if (isKeyPressed(KEY_NSPIRE_PERIOD))	K_pushChar(keyList,'.');
	if (isKeyPressed(KEY_NSPIRE_NEGATIVE))	K_pushChar(keyList,250);
	if (isKeyPressed(KEY_NSPIRE_EQU))		K_pushChar(keyList,'=');
	if (isKeyPressed(KEY_NSPIRE_RET))		K_pushChar(keyList,'\n');
	if (isKeyPressed(KEY_NSPIRE_QUESEXCL))	K_pushChar(keyList, 20);
	return ;
}


void K_getChar(char *keyList)
{
	memset(keyList, 0, 4);
	K_pushAlphaKey(keyList);
	K_pushNumericKey(keyList);
	K_pushPuncKey(keyList);
}


