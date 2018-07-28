#ifndef CHECK_H
#define CHECK_H

// Arguments
typedef struct CHECK_ARGS {
	char	*text;
	int 	alignment;
	BOOL	checked;
} CheckArgs;


void DrawCheck(Widget *w);
int ActivateCheck(Widget *w);
Widget *wExCheck(const char *text, BOOL isChecked, int alignment);
Widget *wCheck(const char *text, BOOL isChecked);


// Méthodes
void wCheck_Set(Widget *w, BOOL b);
BOOL wCheck_IsChecked(Widget *w);
char *wCheck_GetText(Widget *w);

#endif

