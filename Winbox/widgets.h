#ifndef WIDGETS_H
#define WIDGETS_H

#include "structures.h"
#include "menus.h"


// Les constructs
wCONSTRUCT *wNewConstruct(Widget *w);
int wActivateConstruct(Widget *w);
int wExecConstruct(Widget *w);
void wDrawConstruct(Widget *w);
void wCloseConstruct(Widget *w);
void wConstruct_SaveImage(wCONSTRUCT *construct);
void wConstruct_DontSaveImage(wCONSTRUCT *construct);
void wMoveConstruct(Widget *wid, int x, int y, int w, int h);
void wMoveConstructR(Widget *wid, SDL_Rect *r);
wCONSTRUCT *wGetConstruct(Widget *w);
void wConstruct_ChangeTheme(Widget *w, wTHEME *theme);

int wAddWidgetToConstruct(wCONSTRUCT *c, Widget *w);
int wAddFontToConstruct(wCONSTRUCT *c, nSDL_Font *f);
int wAddSurfaceToConstruct(wCONSTRUCT *c, SDL_Surface *s);
int wAddEltToConstruct(wCONSTRUCT *c, void *e);
int wAddMenuToConstruct(wCONSTRUCT *c, wMENU *m);

int wRemoveWidgetFromConstruct(wCONSTRUCT *c, Widget *w);
int wRemoveFontFromConstruct(wCONSTRUCT *c, nSDL_Font *f);
int wRemoveSurfaceFromConstruct(wCONSTRUCT *c, SDL_Surface *s);
int wRemoveEltFromConstruct(wCONSTRUCT *c, void *e);
int wRemoveMenuFromConstruct(wCONSTRUCT *c, wMENU *m);


// Les widgets
void wDrawWidget(Widget *w);
int wActivateWidget(Widget *w);
int wAddWidget(Widget *parent, Widget *child);
int wPlaceWidget(Widget *parent, Widget *child, int spot);
void wRemoveWidget(Widget *w);
void wCloseWidget(Widget *w);
void wSetFreedArgs(Widget *w, int argsType);
int wIsFreedArg(Widget *w, int argsType);

void wSetBackground(Widget *w, wBACKGROUND *background);
void wDrawBackground(Widget *w);
void wDisplayBounds(Widget *w, BOOL b);
void wDrawBounds(Widget *w);
void wSetHeight(Widget *w, int h);
void wSetWidth(Widget *w, int width);
void wSetSize(Widget *w, int width, int height);
void wSetSpot(Widget *w, int x, int y);
void wSetPosition(Widget *w, int x, int y, int h, int width);
void wSetPositionR(Widget *w, SDL_Rect *r);
SDL_Rect wGetPosition(Widget *w);
void wConnect(Widget *w, int (*callBack) (Widget *, int));
void wSetDynamism(Widget *w, BOOL isDynamic);
void wSetActiveWidget(Widget *w);
Widget *wGetActiveWidget(Widget *w);
int wLayout_GetNumberOfChilds(Widget *layout);
Widget **wLayout_GetChildsList(Widget *layout);
void wSetCustomArgs(Widget *w, void *customArgs, void (*freeCustomArgs) (void *));

int wExecCallback(Widget *w, int signal);
BOOL wIsActivable(Widget *w);
void wFindBounds(Widget *w, SDL_Rect *r, BOOL Hcenter, BOOL Vcenter);
int wFindNextWidget(Widget **widgets, int nWidgets, int key, SDL_Rect previousWidgetBounds);
BOOL wHasDialogParent(Widget *w);
int wLayout_GetNumberOfChilds(Widget *w);
Widget **wLayout_GetChildsList(Widget *w);
Widget **wLayout_GetChilds(Widget *w);
Widget *wLayout_GetChild(Widget *w, int n);
int wLayout_GetCurrentChild(Widget *w);



// Les backgrounds
wBACKGROUND *wBackgroundColor(Uint32 color1, Uint32 color2, short mode);
wBACKGROUND *wBackgroundImage(SDL_Surface *img, short mode);
void wBG_Draw(wBACKGROUND *bg, SDL_Rect *surface);
void wBG_SetColor(wBACKGROUND *bg, Uint32 color1, Uint32 color2, short mode);
void wBG_SetImage(wBACKGROUND *bg, SDL_Surface *img, short mode);


// Autres
SDL_Surface *wInitSDL();

#endif

