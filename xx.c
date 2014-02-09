#include <x.h>
int c;
struct quad{
float v1[3];
float v2[3];
float v3[3];
float v4[3];
float n1[3];
float n2[3];
float n3[3];
float n4[3];};
void init(){
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
};
void loadtex(int*,const char*,int);

void draw(){
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   Fon();
 DrawScene(models,0,2); 
glPushMatrix();
glTranslatef(-2.0,0.0,0.0);
 Fon();
    DrawScene(models, 2,2);
glPopMatrix();

glXSwapBuffers(dp,win);

};
void loadtex(int* tex,const char* s,int n){
IMG_Init(IMG_INIT_JPG);
SDL_Surface* sur;
sur=IMG_Load(s);if(sur==NULL){printf("NO LOAD IMAGE\n");
exit(1);};
glBindTexture(GL_TEXTURE_2D,tex[n]);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,sur->w,sur->h,0,GL_RGB,GL_UNSIGNED_BYTE,sur->pixels);
 SDL_FreeSurface(sur);
 IMG_Quit();};
void check(){int i=0;
/*while(run){*/
if(XCheckWindowEvent(dp,win1,ButtonPressMask,&e)){
switch(e.type){
case ButtonPress : run=0;
pattr->background_pixel=col1.pixel;
XClearWindow(dp,win1);
XClearWindow(dp,win2);
XDrawString(dp, win1, gc, 32, 32,"STOPPED YOUR GAME", strlen("STOPPED YOUR GAME"));
XDrawString(dp, win2, gc, 70, 20,"FACK YOU FOR STOPPED GAME IM DESIRE GAME    YA YA MAIN FURRER", strlen("FACK YOU FOR STOPPED GAME IM DESIRE GAME    YA YA MAIN FURRER"));
XFlush(dp);sleep(3);
glXDestroyContext(dp, glc);
XFree(font);
XFreeGC(dp,gc);
XDestroyWindow(dp, win);
XDestroyWindow(dp,win1);
XDestroyWindow(dp, win2);
XDestroyWindow(dp,win3);
DelModels(models,4);
glDeleteShader(vs);
glDeleteShader(gs);
glDeleteShader(fs);
glDeleteProgram(prog);
XCloseDisplay(dp);int i;
 break;
default:break;};
    };
  

  if(XCheckWindowEvent(dp,win2,ButtonPressMask,&e)){
switch(e.type){
   case ButtonPress : 
for(i=0;i<2;i++){models[i].scale[0]-=0.01; models[i].scale[1]-=0.01; models[i].scale[2]-=0.01; };
         break;
 default : break;
    };
  };

   if(XCheckWindowEvent(dp,win3,ButtonPressMask,&e)){
switch(e.type){
   case ButtonPress : /*glTranslatef(0.001,0.0,0.0);*/
for(i=0;i<2;i++){models[i].scale[0]+=0.01; models[i].scale[1]+=0.01; models[i].scale[2]+=0.01; };
         break;
 default : break;
    };
  };
};
void Rec(){char* tir="NNNNNNNNNNNMMMMMMMMMRRRR RRR";
XDrawString(dp, win2, gc, 50, 50,     tir, strlen(tir));
XDrawString(dp, win1, gc, 20, 30,     "CLICK HERE FOR STOP", strlen("CLICK HERE FOR STOP"));
XDrawString(dp, win3, gc, 20, 30,     "CLICK", strlen("CLICK"));
XSync(dp,False);};
void windows(){
int i;float x=0;
int count=200;
for(i=0;i<count;i++){
rec[i].x=i;rec[i].y=(int)x*x/500;rec[i].width=1;rec[i].height=60-2*rec[i].y;
if(i<100){x+=1;}else{x-=1;};};
XSetWindowAttributes attr1;
Visual* visual;
XColor co1,co2;
int screen=DefaultScreen(dp);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"Blue",&co1,&co2);
visual=DefaultVisual(dp,screen);
attr1.background_pixel=co1.pixel;
attr1.border_pixel=co1.pixel;
attr1.colormap=DefaultColormap(dp,screen);
attr1.event_mask=ExposureMask | KeyPressMask | StructureNotifyMask |KeyReleaseMask | KeymapStateMask | ButtonPressMask;
attr1.backing_store = NotUseful;
attr1.override_redirect=True;
attr1.save_under=False;
win1 = XCreateWindow(dp, root, 1000, 10,200,60,4, DefaultDepth(dp,screen), InputOutput,
						visual,CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
win2 = XCreateWindow(dp, root, 10, 10,200,60,3, DefaultDepth(dp,screen), InputOutput,
						visual,CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);attr1.background_pixel=WhitePixel(dp,screen);
XDefineCursor(dp, win2,cursor( dp, win2 ) );
win3 = XCreateWindow(dp, root, 10, 100,200,60,0, DefaultDepth(dp,screen), InputOutput,
						visual,CWBackPixel |CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
image_load("/home/g/S.jpg",dp,win1);
image_load("/home/g/S.jpg",dp,win2);
image_load("/home/g/s.jpg",dp,win3);
XMapWindow( dp, win2 );
XMapWindow(dp,win3);
XGrabKeyboard(dp, win1, True, GrabModeAsync,                  
            GrabModeAsync, CurrentTime);                                     
        XGrabPointer(dp, win1, True, ButtonPressMask,                 
            GrabModeAsync, GrabModeAsync, win1, None, CurrentTime);
XShapeCombineRectangles(dp,win1,ShapeBounding,0,0,rec,200,ShapeIntersect,Unsorted);
XMapWindow( dp, win1 );
XFlush(dp);
XFree(visual);};
Cursor cursor(Display* d,Window w){
Cursor cursor;
static char bm_no_data[100];
int x,y;
for (x=0;x<10;x++){
for(y=0;y<10;y++){
bm_no_data[x*10+y] =0xff;};};
int screen=DefaultScreen(d);
XColor c;
Pixmap pix;
pix=XCreatePixmap(d,w,10,10,1);
XAllocNamedColor(d,DefaultColormap(d,screen),"Green",&c,&col2);
pix = XCreateBitmapFromData(d,w, bm_no_data, 10, 10);
cursor = XCreatePixmapCursor(d, pix, pix, &c, &c,0,0);
XFreePixmap(d,pix);
/*XFreeGC(d,Gc);*/
return cursor;};

