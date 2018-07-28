#ifndef DIALOG_H
#define DIALOG_H


// Arguments
typedef struct DIALOG_ARGS {
	Widget		*widget;
	char			*title;
	wBACKGROUND *title_backg;
	Widget		*button1;
	Widget		*button2;
} DialogArgs;

// Cr�ation
Widget *wDialog1(const char *title, const char *but);
Widget *wDialog2(const char *title, const char *but1, const char *but2);
void DrawDialog(Widget *w);
int ActivateDialog(Widget *w);
void CloseDialog(Widget *w);
int AddDialog(Widget *w, Widget *child, int spot);

// M�thodes
void wDialog_SetTitle(Widget *w, const char *title);
char *wDialog_GetTitle(Widget *w);
void wDialog_SetTitleBackground(Widget *w, wBACKGROUND *bg);

#endif