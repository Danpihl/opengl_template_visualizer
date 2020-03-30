#ifndef _COLORS_H_
#define _COLORS_H_

#define COLOR_IDX_MAX 1280

extern int color_red[COLOR_IDX_MAX];
extern int color_green[COLOR_IDX_MAX];
extern int color_blue[COLOR_IDX_MAX];

void rainbow_color(float d, int *r, int *g, int *b);

#endif