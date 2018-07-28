#ifndef BOXLIST_H
#define BOXLIST_H

#include "menus.h"

Widget *wBoxList(wMENU *m, int dItems, int maxSelected);
Widget *wBoxExplorer(char *path, char *pattern, int dItems);
void DrawBoxList(Widget *w);
int  ActivateBoxList(Widget *w);

#endif