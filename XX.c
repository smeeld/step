#include <x.h>
int main(int c,char * s[]){run=1;rot=0;
struct quad* f[4];
XVisualInfo* visual;
GLint att[]={GLX_RGBA,
		GLX_RED_SIZE, 0,
		GLX_GREEN_SIZE, 0,
		GLX_BLUE_SIZE, 0,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_ALPHA_SIZE, 8,None};
XSetWindowAttributes attr;
XEvent e;
pattr=&attr;
models=malloc(sizeof(struct model)*2);
dp=XOpenDisplay(NULL);
int screen=DefaultScreen(dp);
root=RootWindow(dp,screen);
visual=glXChooseVisual(dp,0,att);
XAllocNamedColor(dp,DefaultColormap(dp,screen),"Green",&col1,&col2);
attr.background_pixel=BlackPixel(dp,screen);
attr.border_pixel=0;
attr.colormap=XCreateColormap(dp,root,visual->visual,AllocNone);
attr.event_mask=ExposureMask | KeyPressMask | StructureNotifyMask |KeyReleaseMask | KeymapStateMask | ButtonPressMask;
attr.backing_store = NotUseful;
attr.override_redirect=True;

attr.save_under=False;
win = XCreateWindow(dp, root, 0, 0,1400,1200,0, visual->depth, InputOutput,
visual->visual,CWBackPixel | CWSaveUnder | CWColormap | CWEventMask |  CWOverrideRedirect, &attr);
XDefineCursor(dp, win,cursor( dp, win ) );
XGrabKeyboard(dp, win, True, GrabModeAsync,                  
            GrabModeAsync, CurrentTime);                                     
        XGrabPointer(dp, win, True, ButtonPressMask,                 
            GrabModeAsync, GrabModeAsync, win, None, CurrentTime);

XMapWindow( dp, win );
 XFlush(dp);
glc=glXCreateContext(dp,visual,NULL,GL_TRUE);if(glc==NULL){
printf("NO WORK\n");exit(1);};
XSelectInput ( dp, win, ExposureMask | KeyPressMask | ButtonPressMask );
glXMakeCurrent(dp,win,glc);
XFree(visual);
 models[0].size=atoi(s[1]);
models[1].size=atoi(s[2]);

/*
float pos[4]={1,1,1,0.1};
float mat[4]={1.0,1.0,1.0,1.0};
float col[3]={0.0,1.0,0.0};*/
step=1.5;x_b=-0.8;y_b=-0.8;
glViewport(0, 150, 1400,1200);
rot=0;ang1=0;
glClearColor(0,0,0,0);

glEnable(GL_NORMALIZE);
glEnable(GL_DEPTH_TEST);
glEnable(GL_TEXTURE_2D);
GLuint p=CreateShader();
glUseProgram(p);
prog=p;

 models[0].texfile="/home/g/s5.jpg";
ang=0;
models[1].texfile="/home/g/s5.jpg";

 SP=Sp(models,0);
 Par(models,1);

glUniform1i(glGetUniformLocation( prog, "tex"),0);
glUniform1i(glGetUniformLocation( prog, "tex2"),1);

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,models[1].tex[0]);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D,models[0].tex[0]);


/*glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat);
glLightfv(GL_LIGHT0,GL_DIFFUSE,col);
glLightfv(GL_LIGHT0,GL_POSITION,pos);
glDisable(GL_LIGHTING);
glDisable(GL_LIGHT0);*/
windows();printf("OKK\n");
font=XLoadQueryFont(dp,"-*-helvetica-*-r-*-*-16-*-*-*-*-*-*-*");
gv.foreground=col1.pixel;
gv.line_width=2;
gv.font=font->fid;
gv.line_style=LineSolid;
gc=XCreateGC(dp,win1,GCForeground | GCLineWidth | GCLineStyle | GCFont,&gv);
if(gc==NULL){printf("NO\n");exit(1);};run=1;
while(run){check();
draw();Rec();
};exit(0);
}
