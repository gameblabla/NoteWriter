#ifndef PANEL_H
#define PANEL_h

// Arguments

// Création
Widget *wPanel(BOOL displayBounds);


// Méthodes
SDL_Rect wPanel_GetArea(Widget *w);
void wPanel_Connect(Widget *w, void (*draw) (Widget *));

#endif

