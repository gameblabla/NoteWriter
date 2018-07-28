#ifndef REQUEST_H
#define REQUEST_H

enum REQUEST_TYPES {
	REQUEST_ALL,
	REQUEST_INT,
	REQUEST_ID,
	REQUEST_VAR,
	REQUEST_NAME,
};

// Arguments
typedef struct REQUEST_ARGS {
	char	*str;
	char *voidStr;
	char *text;
	int type;
	int min, max;
	
	int x;
} RequestArgs;

Widget *wRequest(const char *str, const char *text, int type, int max);
Widget *wExRequest(const char *str, const char *text, int type, int max, const char *voidStr);
void DrawRequest(Widget *w);
int  ActivateRequest(Widget *w);
void CloseRequest(Widget *w);

// Méthodes
void wRequest_SetVoidStr(Widget *w, const char *voidStr);
void wRequest_SetStr(Widget *w, const char *str);
void wRequest_SetText(Widget *w, const char *t);
char *wRequest_GetText(Widget *w);
char *wRequest_GetAllocatedText(Widget *w);
char *wRequest_GetTitle(Widget *w);
void wRequest_SetMax(Widget *w, int max);

// Méthodes spécifiques aux REQUEST_INT
int wRequest_GetInt(Widget *w);
void wRequest_SetInt(Widget *w, int n);
void wRequest_SetMin(Widget *w, int min);

#endif
