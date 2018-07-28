#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H


typedef struct PROGRESSBAR_ARGS {
	int max;
	int value;
	Uint32 color1;
	Uint32 color2;
} ProgressBarArgs;


// Cr�ation
Widget *wProgressBar(int max, int value, int color1, int color2);
Widget *wActiveProgressBar(int max, int value, int color1, int color2);
Widget *wExProgressBar(int max, int value, int color1, int color2, BOOL isActivable);
Widget *wProgressBarV(int max, int value, int color1, int color2);
Widget *wActiveProgressBarV(int max, int value, int color1, int color2);
Widget *wExProgressBarV(int max, int value, int color1, int color2, BOOL isActivable);
void DrawProgressBar(Widget *w);
int ActivateProgressBar(Widget *w);

// M�thodes
int wPBar_GetValue(Widget *w);
void wPBar_SetValue(Widget *w, int value);
void wPBar_SetColors(Widget *w, Uint32 color1, Uint32 color2);
int wPBar_GetMax(Widget *w);
void wPBar_SetMax(Widget *w, int max);

#endif