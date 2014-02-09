#include <stdio.h>
#include <fcntl.h>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <GL/glx.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
struct String{
char* buf;
int size;
int pos_x;
int pos_y;
};
struct f_desc{
char* buffer;
size_t s;};
void ScrollStrings(Display*,Window,GC,struct String*,int,int,int);
int LoadBuf(const char*,struct String**);
int LoadText(Display*,Window,const char*,GC,struct String**);
void DrawStrings(Display*,Window,GC,struct String*,int,int);
void DeleteStringBuf(struct String*);
Cursor cursor(Display*,Window);
int FileRead(const char*,struct f_desc*);
char* str="STOP";
XSetWindowAttributes attr1;
void image(const char*,Display*, Window);
XColor c,col2;
Display* dp;int run;
Window root;
Window win;
GC gc;
XGCValues gv;

int main(int v,char* s[]){run=1;
struct String* strings_pointer=NULL;
int first_str_num=0;
int str_count=0;
int sw=0;
char* key_input=NULL;
int current_string_first=0;
XEvent e;
dp=XOpenDisplay(NULL);
int screen=DefaultScreen(dp);
attr1.colormap=DefaultColormap(dp,screen);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"white",&c,&col2);
XFontStruct* font=XLoadQueryFont(dp,"-*-helvetica-*-r-*-*-16-*-*-*-*-*-*-*");
attr1.event_mask=ExposureMask | KeyPressMask | StructureNotifyMask | KeymapStateMask | ButtonPressMask;
attr1.backing_store = NotUseful;
attr1.override_redirect=True;
attr1.save_under=False;
gv.foreground=c.pixel;
gv.font=font->fid;
root=RootWindow(dp,screen);
win = XCreateWindow(dp, root, 0, 0,1400,1200,0,DefaultDepth(dp,screen), InputOutput,
						DefaultVisual(dp,screen),CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
Window win2=XCreateWindow(dp, win, 200, 10,150,50,0,DefaultDepth(dp,screen), InputOutput,
						DefaultVisual(dp,screen),CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
Window win3=XCreateWindow(dp, win, 400, 10,150,50,0,DefaultDepth(dp,screen), InputOutput,
						DefaultVisual(dp,screen),CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
Window win4=XCreateWindow(dp, win, 10, 80,300,300,0,DefaultDepth(dp,screen), InputOutput,
						DefaultVisual(dp,screen),CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);
Window win5=XCreateWindow(dp, win, 600, 10,150,50,0,DefaultDepth(dp,screen), InputOutput,
						DefaultVisual(dp,screen),CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect | CWBorderPixel, &attr1);


image("/home/g/s.jpg",dp,win3);

image(s[1],dp,win);
image("/home/g/s5.jpg",dp,win2);

XDefineCursor(dp, win,cursor( dp, win ) );
gc=XCreateGC(dp,win2,GCForeground | GCFont,&gv);

XAllowEvents(dp, ReplayKeyboard, CurrentTime);
XMapWindow( dp, win );
XMapWindow(dp,win2);
XMapWindow(dp,win5);
XMapWindow(dp,win3);
XDrawString(dp,win2,gc,20,20,str,strlen(str));
XDrawString(dp,win3,gc,20,20,"READ FILE",strlen("READ FILE"));
XDrawString(dp,win5,gc,20,20,"SCROLL",strlen("SCROLL"));
XGrabKeyboard(dp, win5, True, GrabModeAsync,                  
            GrabModeAsync, CurrentTime); 
XSync(dp,True); 
int mid,mid_p;   
while(1){
XNextEvent(dp,&e);
switch(e.type){
case ButtonPress:
if(e.xbutton.window==win2){XClearWindow(dp,win2);
XDrawString(dp,win2,gc,20,20,"OK",strlen("OK"));
XFlush(dp);
DeleteStringBuf(strings_pointer);
sleep(1);
XDestroyWindow(dp,win);XDestroyWindow(dp,win2);XCloseDisplay(dp);exit(0);};

if(e.xbutton.window==win3){
if((str_count=LoadText(dp,win4,s[2],gc,&strings_pointer))<0){XMapWindow(dp,win4); 
XDrawString(dp,win4,gc,20,20,"NO FILE",strlen("NO FILE"));
XDrawString(dp,win4,gc,20,60,s[2],strlen(s[2]));
XFlush(dp);
sleep(3);
XUnmapWindow(dp,win4);
};
break;};
if(e.xbutton.window==win5){
if(str_count>30){if((str_count-30)>current_string_first){
 ScrollStrings(dp,win4,gc,strings_pointer,1,current_string_first,str_count);
current_string_first+=1;};
     };break;};
case KeyPress:
if(e.xkey.window==win5){
key_input=XKeysymToString(XLookupKeysym(&e.xkey,0));
if(strncmp(key_input,"Down",strlen("Down"))==0){
if((str_count-current_string_first)>30){
ScrollStrings(dp,win4,gc,strings_pointer,1,current_string_first,30);current_string_first+=1;
break;
}else{break;};/*
else
{ScrollStrings(dp,win4,gc,strings_pointer,0,current_string_first,str_count-current_string_first);
printf("FROM 2  CURRENT_STRING_FIRST=%d STR_COUNT==%d\n", current_string_first,str_count);

};*/
  };
if(strncmp(key_input,"Up",strlen("Up"))==0){
if(current_string_first>0){
 ScrollStrings(dp,win4,gc,strings_pointer,-1,current_string_first,30);
current_string_first-=1;
break;}else{break;};
 };
};
break;
default:break;
      };
  };
};

Cursor cursor(Display* d,Window w){
Cursor cursor;
 static char bm_no_data[100];
int x,y;
for (x=0;x<10;x++){
for(y=0;y<10;y++){
bm_no_data[x*10+y] =0xff;};};
Pixmap pix;
pix=XCreatePixmap(d,w,10,10,DefaultDepth(d,DefaultScreen(d)));
/*gv.foreground=c.pixel;
Gc=XCreateGC(d,pix,GCForeground,&gv);
XFillRectangle(d, pix, Gc, 0, 0, 10, 10);*/printf("JJ\n");
pix = XCreateBitmapFromData(d,w, bm_no_data, 10, 10);
cursor = XCreatePixmapCursor(d, pix, pix, &c, &c,0,0);
XFreePixmap(d,pix);
/*XFreeGC(d,Gc);*/
return cursor;};
void image(const char* file,Display* d,Window w){
SDL_Surface* sur;
IMG_Init(IMG_INIT_JPG);
sur=IMG_Load(file);if(sur==NULL){printf("NO LOAD IMAGE\n");
exit(1);};
int wi=sur->w;
int h=sur->h;
char* buf=sur->pixels;
unsigned char* img=(unsigned char*)malloc(sizeof(char)*4*wi*h);
int x,y;
for(x=0,y=0;x<wi*h*4,y<wi*h*3;x+=4,y+=3){
img[x]=buf[y+2];img[x+1]=buf[y+1],img[x+2]=buf[y];img[x+3]=0xff;};
Pixmap pix=XCreatePixmap(d, w, wi, h, DefaultDepth(d,DefaultScreen(d)));
XImage* i=XCreateImage(d,XDefaultVisual(d, XDefaultScreen(d)),DefaultDepth(d,DefaultScreen(d)) ,ZPixmap,0,img,
wi,h,32,0);

XPutImage(d, pix, DefaultGC(d, 0), i, 0, 0,0, 0, 1200, 1100);
XSetWindowBackgroundPixmap(d, w, pix);
XFreePixmap(d,pix);
free(img);
IMG_Quit();};
int FileRead(const char* f,struct f_desc* psh){
struct stat st;
int er;
int temp=0;
int count=0;
 int file;if((file=open(f,O_RDONLY))<0){return -1;};
if((er=fstat(file,&st))!=0){printf("UNCNOWN ERRROR\n");return -1;};
char* buf=(char*)malloc(sizeof(char)*st.st_size);if(buf==NULL){printf("UNCNOWN ERRROR\n");return -1;};
while((temp=read(file,buf+count,st.st_size))!=0){
count+=temp;temp=0;};
psh->buffer=buf;
psh->s=st.st_size;
close(file);
return 0;};
int LoadBuf(const char* file,struct String** strings_p){
struct f_desc f_d;
struct String* string_temp;
struct String* string_mas;
string_mas=(struct String*)malloc(sizeof(struct String));
if(FileRead(file,&f_d)!=0){return -1;};
int count_size=0;
int temp=0;
int count=0;
while(count_size<f_d.s){
temp=string_retrive(f_d.buffer,count_size);
string_temp=realloc((struct String*)string_mas,sizeof(struct String)*(count+1));if(string_temp==NULL){printf("MEMORY ALLOCATE ERROR\n");return -1;};
string_mas=string_temp;
string_mas[count].size=temp;
string_mas[count].buf=&f_d.buffer[count_size];
string_mas[count].pos_x=10;
string_mas[count].pos_y=50+count*20;

count++;
count_size+=(temp+1);
};
*strings_p=string_mas;
return count;
};
int string_retrive(char* buf,int pos){
int count=1;
char sym;
while((sym=buf[pos+count])!='\n'){count++;};
return count;
};
int LoadText(Display* d,Window w,const char* file,GC gc,struct String** strings){
int i,x,y;
struct String* temp=NULL;
int strings_num;
if((strings_num=LoadBuf(file,&temp))<0){return -1;};
image("/home/g/s.jpg",d,w);
XResizeWindow(d,w,1350,700);XMapWindow(d,w);
if(strings_num<30){
DrawStrings(d,w,gc,temp,strings_num,0);}
else{DrawStrings(d,w,gc,temp,30,0);}

XFlush(d);
*strings=temp;
return strings_num;
};
void DrawStrings(Display* d,Window w,GC gc,struct String* strings,int str_num,int first_str_num){
int i;int first=first_str_num;
for(i=0;i<str_num;i++){
XDrawString(d,w,gc,strings[first+i].pos_x,i*20+30,strings[first+i].buf,strings[first+i].size);
strings[first+i].pos_y=i*20+30;};
};
void DeleteStringBuf(struct String* str){
free(str[0].buf);
free(str);
};
void ScrollStrings(Display* d,Window w,GC gc,struct String* s,int c,int cur_pos,int num){
XClearWindow(d,w);
DrawStrings(d,w,gc,s,num,cur_pos+c);

XFlush(d);
};
