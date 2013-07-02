/* graph.h (emx+gcc) */

#ifndef _GRAPH_H
#define _GRAPH_H

#if defined (__cplusplus)
extern "C" {
#endif

/* g_mode() and g_modeset() arguments (MODE) */

#define G_MODE_OFF         0    /* Back to previous mode                    */

#define G_MODE_EGA_C      16    /* EGA 640x200 16 colors                    */
#define G_MODE_EGA_E      17    /* EGA 640x350 16/64 colors                 */

#define G_MODE_VGA_H      24    /* VGA 640x480 16 colors                    */
#define G_MODE_VGA_L      26    /* VGA 320x200 256 colors                   */

#define G_SET_KEEP         0     /* No change                               */

/* g_wmode() arguments */

#define G_NORM 0                /* Normal write mode */
#define G_AND  1                /* `AND' mode        */
#define G_OR   2                /* `OR' mode         */
#define G_XOR  3                /* `XOR' mode        */

/* Fill or draw outline */

#define G_OUTLINE      0             /* Draw outline */
#define G_FILL         1             /* Fill         */

/* EGA colors */

#define G_BLACK        0
#define G_BLUE         1
#define G_GREEN        2
#define G_CYAN         3
#define G_RED          4
#define G_MAGENTA      5
#define G_YELLOW       6
#define G_BROWN        6
#define G_WHITE        7

#define G_INTENSITY    8

/* Screen dimensions */

extern int g_xsize;             /* Screen width             */
extern int g_ysize;             /* Screen height            */
extern int g_colors;            /* Number of colors         */


void g_box (int x0, int y0, int x1, int y1, int color, int fill_flag);
void g_clear (int color);
void g_clip (int x0, int y0, int x1, int y1);
void g_ellipse (int cx, int cy, int rx, int ry, int color, int fill_flag);
int g_get (int x, int y);
void g_hline (int y, int x0, int x1, int color);
void g_line (int x0, int y0, int x1,int y1, int color);
void g_lock (void);
int g_mode (int mode);
int g_modeset (int mode, int flag);
void g_polygon (__const__ int *x, __const__ int *y, int n, int color,
    int fill_flag);
void g_set (int x, int y, int color);
void g_triangle (int x0, int y0, int x1, int y1, int x2, int y2, int color,
    int fill_flag);
void g_unlock (void);
void g_unlockall (void);
void g_vgapal (__const__ char *pal, int first, int n, int wait_flag);
void g_vline (int x, int y0, int y1,int color);
void g_waitv (void);
void g_wmode (int wmode);

#if defined (__cplusplus)
}
#endif

#endif /* not _GRAPH_H */
