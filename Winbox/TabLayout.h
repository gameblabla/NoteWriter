#ifndef TABLAYOUT_H
#define TABLAYOUT_H

struct TAB {
	char *text;
	BOOL canBeDeleted;
	int px;
};

typedef struct TAB_LAYOUT_ARGS {
	int nWidgets;
	Widget **widgets;
	int cWidget;
	int hWidget;
	int nTabs;
	struct TAB *tabs;
} TabLayoutArgs;


Widget *wTab();
void DrawTabLayout(Widget *w);
int ActivateTabLayout(Widget *w);
void CloseTabLayout(Widget *w);

// Méthodes
int wTab_AddTab(Widget *tab, const char *text, Widget *child);
int wTab_AddExTab(Widget *tab, const char *text, Widget *child, BOOL canBeDeleted);
int wTab_SetTab(Widget *w, const char *text, Widget *child, int spot);
int wTab_SetExTab(Widget *w, const char *text, Widget *child, BOOL canBeDeleted, int spot);

int wTab_GetCurrentTab(Widget *tabLayout);
Widget *wTab_GetCurrentWidget(Widget *tab);
char *wTab_GetCurrentTabTitle(Widget *tabLayout);
void wTab_SetTabTitle(Widget *tabLayout, int tab, const char *title);

#endif

