#ifndef PANEL_H
#define PANEL_h

// Arguments

// Cr�ation
Widget *wPanel(BOOL displayBounds);


// M�thodes
SDL_Rect wPanel_GetArea(Widget *w);
void wPanel_Connect(Widget *w, void (*draw) (Widget *));

#endif

