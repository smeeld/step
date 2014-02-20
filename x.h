#include <stdio.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <math.h>
#include <SDL/SDL_image.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
int size1;
XGCValues gv;
struct model{
float* vertex;
int* index;
float pos[3];
float scale[3];
int tex[2];
char* texfile;

int size;
int buf[2];
int count;
};
int prog,vs,gs,fs,run;
 float ang;
struct model* models;
Window root,win,win1,win2,win3;
Display* dp;
XEvent e;
GC gc;
XColor col1,col2;
XFontStruct* font;
GLXContext glc;
XSetWindowAttributes* pattr;
XRectangle rec[200];
Cursor cursor(Display*,Window);
float x_b,y_b, step,rot,ang1;
int* t;
float* SPn,ppar,SP;
