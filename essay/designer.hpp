#include <packet.hpp>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <pthread.h>
#include <cstdlib>

struct window
  { 
Display* dp;
Window win;
Window root;
pthread_t pt;
XSetWindowAttributes attr;
 XFontStruct* font;
 GC gc;
 XGCValues gv;
 XColor col1, col2;
 XEvent e;

window()
 {
 create(20, 20, 600, 300);
 
 };

void create(int x, int y, int w, int h)
 {
dp=XOpenDisplay(NULL);
if(dp==NULL) exit(-1);
int screen=DefaultScreen(dp);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"Green", &col1, &col2);

attr.background_pixel=BlackPixel(dp,screen);
attr.border_pixel=col1.pixel;
attr.colormap=DefaultColormap(dp,screen);
attr.event_mask=ExposureMask | KeyPressMask | StructureNotifyMask |KeyReleaseMask | KeymapStateMask | ButtonPressMask;
attr.backing_store = NotUseful;
attr.override_redirect=True;
attr.save_under=False;
win = XCreateWindow(dp, DefaultRootWindow(dp), x, y, w, h, 4, DefaultDepth(dp,screen), InputOutput,  DefaultVisual(dp,screen), CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr);

gc=NULL; font=NULL;
font=XLoadQueryFont(dp,"-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*");
if(font) gv.font=font->fid;
gv.foreground=col2.pixel;
gv.line_width=2;
gv.line_style=LineSolid;
gc=XCreateGC(dp, win, GCForeground | GCLineWidth | GCLineStyle | GCFont, &gv);

XMapWindow(dp, win);
XDrawString(dp, win, gc, 20, 20,"Lucifer", 7);
XFlush(dp);
if(pthread_create(&pt, NULL, &thr, this)!=0) exit(-1);
 };

static void* thr(void*);
void draw(sox_packet& hdr)
  { 
    switch(hdr.type())
    {
      case 0x42 : XDrawLine(dp, win, gc, hdr[0], hdr[1], hdr[2], hdr[3]); break;
      case 0x41 :  XDrawPoint(dp, win, gc, hdr[0], hdr[1]);  break;
      case 0x43 : XDrawArc(dp, win, gc, hdr[0], hdr[1], hdr[2], hdr[3], 64*hdr[4], 64*hdr[5]); break;
    };
 XFlush(dp);
 };
};

void* window::thr(void* p)
 {
   window* wp=static_cast<window*>(p);
   XWindowEvent(wp->dp, wp->win, ButtonPressMask, &wp->e);
    exit(0);
  };

 struct designer
  {

    struct buffer: sox_packet{};

    window win;

    int process(buffer& p)
   {
     sox_packet& pkt=static_cast<sox_packet&>(p);
     
       if(pkt.good())
       {
          win.draw(pkt); 
          return 0;
        }
     return -1;  
   };
};
