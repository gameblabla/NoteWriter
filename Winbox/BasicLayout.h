#ifndef BASICLAYOUT_H
#define BASICLAYOUT_H

typedef struct BASIC_LAYOUT_ARGS {
	int nWidgets;
	Widget **widgets;
	int cWidget;
} BasicLayoutArgs;


Widget *wBasicLayout(int DisplayBounds);
void DrawBasicLayout(Widget *w);
int ActivateBasicLayout(Widget *w);
void CloseBasicLayout(Widget *w);
int AddBasicLayout(Widget *w, Widget *child, int spot);

#endif

