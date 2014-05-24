
#include <window.h>
window::window(){

   rt=1;
dp=XOpenDisplay(NULL);
 root=RootWindow(dp, DefaultScreen(dp)); prt=this;
 };

window::window(window* winp, int x, int y, int w, int h, shape* r){

if(winp){ dp=winp->dp;rt=0; root=winp->root;  prt=winp; }else{ rt=1;
dp=XOpenDisplay(NULL); root=RootWindow(dp, DefaultScreen(dp)); prt=this; };
 create(x, y, w, h, r);
 XFlush(dp);
};

void window::create(int x, int y, int w, int h, shape* r){

int screen=DefaultScreen(dp);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"Green", &col1, &col2);

attr.background_pixel=col1.pixel;
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

if(r){
if(r->r) XShapeCombineRectangles(dp, win, ShapeBounding,0, 0, r->r, r->num, ShapeIntersect,Unsorted);
 
 if(r->fone) image_load(r->fone);
};
XDefineCursor(dp, win, cursor() );
XMapWindow(dp, win);
XDrawString(dp, win, gc, 30, 40,"STOP", strlen("STOP"));
XFlush(dp);

};
 
 window* window::add_window(int x, int y, int w, int h, shape* r){
  window* p;
  try{ 
   p=new window(this, x, y, w, h, r);
    }catch(std::bad_alloc& s){ return NULL; };
  lst.push_back(std::shared_ptr<window>(p));
  return p;
  };
Cursor window::cursor(){

 Cursor cursor;
static char bm_no_data[100];
int x,y;
for (x=0;x<10;x++){
for(y=0;y<10;y++){
bm_no_data[x*10+y] =0xff;};};
int screen=DefaultScreen(dp);
XColor c;
Pixmap pix;
pix=XCreatePixmap(dp, win, 10,10,1);
XAllocNamedColor(dp, DefaultColormap(dp, screen),"Green", &c, &col1);
pix = XCreateBitmapFromData(dp, win, bm_no_data, 10, 10);
cursor = XCreatePixmapCursor(dp, pix, pix, &c, &c,0,0);
XFreePixmap(dp, pix);
return cursor;

 };

glwindow::glwindow(window* wp, int x, int y, int w, int h, shape* r): window(){

  create(x,y,w,h,r);
  XMapWindow( dp, win );
 XFlush(dp);
int i;float z=0;
int count=200;
XRectangle rec[200];
shape sh;
sh.num=200;
sh.r=rec;
sh.fone="/usr/share/prog/image/s.jpg";
for(i=0;i<count;i++){

rec[i].x=i;rec[i].y=(int)z*z/500;rec[i].width=1;rec[i].height=60-2*rec[i].y;

if(i<100){z+=1;}else{z-=1;};};

button=add_window(1000, 10,200,60, &sh);

};
 
void glwindow::create(int x, int y, int w, int h, shape* r=NULL){ 

 XVisualInfo* visual;
GLint att[]={GLX_RGBA,
		GLX_RED_SIZE, 0,
		GLX_GREEN_SIZE, 0,
		GLX_BLUE_SIZE, 0,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_ALPHA_SIZE, 8,None};
dp=XOpenDisplay(NULL);
int screen=DefaultScreen(dp);

visual=glXChooseVisual(dp, 0, att);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"Green",&col1, &col2);
attr.background_pixel=BlackPixel(dp,screen);
attr.border_pixel=0;
attr.colormap=XCreateColormap(dp,root,visual->visual,AllocNone);
attr.event_mask=ExposureMask | KeyPressMask | StructureNotifyMask |KeyReleaseMask | KeymapStateMask | ButtonPressMask;
attr.backing_store = NotUseful;
attr.override_redirect=True;

attr.save_under=False;
win = XCreateWindow(dp, prt->root, x, y, w, h, 0, visual->depth, InputOutput,
visual->visual,CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect, &attr);

glc=glXCreateContext(dp, visual, NULL, GL_TRUE); if(glc==NULL){ return; };

XSelectInput ( dp, win, ExposureMask | KeyPressMask | ButtonPressMask );
glXMakeCurrent(dp, win, glc);
glViewport(0, 150, 1400,1200);
glClearColor(0,0,0,0);
glEnable(GL_NORMALIZE);
glEnable(GL_DEPTH_TEST);
glEnable(GL_TEXTURE_2D);

  XFree(visual);
 };

glwindow::~glwindow(){ 
 glXDestroyContext(dp, glc);

  };

void glwindow::show(scene& s){
uint8_t run=1;
 while(run){

 if(XCheckWindowEvent(dp, lst.begin()->get()->win, ButtonPressMask, &e)){
   run=0;
   };
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 s.draw();

glXSwapBuffers(dp, win);
 };
};

void window::image_load(const char* file){

   image img;

  if(img.ret_image(file, TP_WIN)<0) return;
 
  Pixmap pix=XCreatePixmap(dp, XDefaultRootWindow(dp), img.w, img.h, DefaultDepth(dp, DefaultScreen(dp)));

  XImage* i=XCreateImage(dp, XDefaultVisual(dp, XDefaultScreen(dp)),DefaultDepth(dp, DefaultScreen(dp)) ,ZPixmap,0, (char*)img.buf, img.w, img.h, 32,0);

XPutImage(dp, pix, gc, i, 0, 0,0, 0, 1200, 1100);
XSetWindowBackgroundPixmap(dp, win, pix);
XFreePixmap(dp, pix);
    };


