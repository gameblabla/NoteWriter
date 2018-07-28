#ifndef SCROLLEDLAYOUT_H
#define SCROLLEDLAYOUT_H

typedef struct SCROLLED_LAYOUT_ARGS {
	int nWidgets;
	Widget **widgets;
	int cWidget;
	int y;
} ScrolledLayoutArgs;


Widget *wScrolledLayout();
void DrawScrolledLayout(Widget *w);
int ActivateScrolledLayout(Widget *w);

// Méthodes
Widget **wScrolledLayout_GetWidgetsList(Widget *w);
int wScrolledLayout_GetNumberOfWidgets(Widget *w);

#endif

