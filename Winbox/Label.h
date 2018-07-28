#ifndef LABEL_H
#define LABEL_H

// Arguments
typedef struct LABEL_ARGS {
	char		*text;
	nSDL_Font	*font;
	int 		alignment;
} LabelArgs;

Widget *wExLabel(const char *text, int alignment, nSDL_Font *font);
Widget *wLabel(const char *text, int alignment);
void DrawLabel(Widget *w);
void CloseLabel(Widget *w);

void wLabel_SetText(Widget *w, const char *text);
char *wLabel_GetText(Widget *w);
void wLabel_SetFont(Widget *w, nSDL_Font *font);
void wLabel_SetAlignment(Widget *w, int alignment);

#endif

