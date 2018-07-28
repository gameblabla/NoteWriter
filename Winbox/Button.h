#ifndef BUTTON_H
#define BUTTON_H

enum BUTTON_MODE {
	BUTTON_PRESS,
	BUTTON_SELECT,
};

// Arguments
typedef struct BUTTON_ARGS {
	char	*text;
	int	state; // 0,1 = bouton sélectionnable, -1 = bouton-pressoir
	nSDL_Font	*font;
	nSDL_Font	*font_selected;
	wBACKGROUND *bg;
	wBACKGROUND *bg_selected;
} ButtonArgs;

Widget *wButton(const char *text, int mode);
Widget *wButtonImage(SDL_Surface *img, SDL_Surface *img_selected, int mode);
void DrawButton(Widget *w);
int  ActivateButton(Widget *w);
void CloseButton(Widget *w);

void wButton_SetText(Widget *w, const char *text);
void wButton_SetMode(Widget *w, int mode);
int  wButton_GetMode(Widget *w);
void wButton_SetFont(Widget *w, nSDL_Font *font, nSDL_Font *font_selected);
void wButton_SetBackground(Widget *w, wBACKGROUND *bg, wBACKGROUND *bg_selected);


#endif
