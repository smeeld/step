#include <x.h>

struct sh{
char* buffer;
int size;};
int load_sh(const char* f,struct sh* psh){
struct stat st;int er=0;
int count=0;
int temp=0;
int file=open(f,O_RDONLY);
if((er=fstat(file,&st))!=0){printf("UNCNOWN ERRROR\n");return -1;};
char* buf=(char*)malloc(sizeof(char)*st.st_size);if(buf==NULL){printf("UNCNOWN ERRROR\n");return -1;};
while((temp=read(file,buf+count,st.st_size))!=0){
count+=temp;temp=0;};
psh->buffer=buf;
psh->size=st.st_size;
close(file);
return 0;}
int works(int type,const char* file,GLuint* shid){
const char* point;
int info;
GLuint shv;
int s_z;
struct sh stsh;
if(load_sh(file,&stsh)!=0){printf("ERROR CREATE  SHADER\n");return -1;};
printf("LENGT==%d\n",stsh.size);
point=stsh.buffer;
printf("FROM++++++%s\n",stsh.buffer);
switch(type){
case 1:
shv=glCreateShader(GL_VERTEX_SHADER);printf("CREATE SH===%d\n",type);break;
case 2:
shv=glCreateShader(GL_GEOMETRY_SHADER);printf("CREATE SH===%d\n",type);break;
case 3:
shv=glCreateShader(GL_FRAGMENT_SHADER);printf("CREATE SH===%d\n",type);break;
default: break;};
glShaderSource(shv,1,&point,&stsh.size);
glCompileShader(shv);
free(stsh.buffer);
*shid=shv;printf("SHHHHHHHHHHH %d\n",*shid);
glGetShaderiv(shv,GL_COMPILE_STATUS, &info);

printf("STATUS COMPILE==%x\n",info);
return 0;
};
GLuint CreateShader(void){


GLuint pr=glCreateProgram();

works(1,"/home/g/vs.glsl",&vs);
works(3,"/home/g/fs.glsl",&fs);/*
works(3,"/home/g/fs.glsl",&fs);*/
  glAttachShader(pr, vs);/*
 glAttachShader(pr, gs);*/
  glAttachShader(pr,fs);
  glLinkProgram(pr);
printf("LINKING\n");
return pr;
};
void Sp(struct model* m, int in){
 float* s;
 GLuint* i;
 int sz=m[in].size;
 int Y=sqrt(sz);

 if((s=(float*)malloc(sizeof(float)*sz*8))==NULL){ printf("ERROR MEM\n");return; };
if((i=(GLuint*)malloc(sizeof(GLuint)*sz*6))==NULL){ printf("ERROR MEM\n");return; };
int x,y;
float a=0;
float b=0;a=0;
m[in].count=0;
float X=(float)Y;
float sq2=2*M_PI/(X-1);
float sq=M_PI/(X-1);
for(x=0;x<Y;x+=1){
    b=0;
  for(y=0;y<Y;y+=1){
s[3*Y*x+3*y]=0.3*sin(a)*sin(b);
s[3*Y*x+3*y+1]=0.3*cos(a);
s[3*Y*x+3*y+2]=0.3*sin(a)*cos(b);
s[3*sz+3*Y*x+3*y]=sin(a)*sin(b);
s[3*sz+3*Y*x+3*y+1]=sin(a)*cos(b);
s[3*sz+3*Y*x+3*y+2]=cos(a);
s[6*sz+2*Y*x+2*y]=x/X;
s[6*sz+2*Y*x+2*y+1]=y/X;
   b+=sq2;};
 a+=sq;};

for(x=0;x<Y-1;x+=1){
for(y=0;y<Y-1;y++){
i[6*Y*x+6*y]=x*Y+y;
i[6*Y*x+6*y+1]=x*Y+Y+y;
i[6*Y*x+6*y+2]=x*Y+y+1;
i[6*Y*x+6*y+3]=x*Y+y+1;
i[6*Y*x+6*y+4]=x*Y+y+Y;
i[6*Y*x+6*y+5]=x*Y+Y+y+1;
  };
 };

m[in].pos[0]=-0.3; m[in].pos[1]=0.1; m[in].pos[2]=0.3;
m[in].scale[0]=1.0; m[in].scale[1]=1.0; m[in].scale[2]=1.0;

m[in].index=i;
m[in].vertex=s;

glGenTextures(1,m[in].tex);
loadtex(m[in].tex, m[in].texfile,0);
  LoadVBO(m,in);
 
};

METHODDEF(void)
_error_exit (j_common_ptr cinfo)
{
  _error_ptr err = (_error_ptr) cinfo->err;

  (*cinfo->err->output_message) (cinfo);

  longjmp(err->setjmp_buffer, 1);
}

int ret_image(const char* file, struct image* img,int type){

  int  h, w, count;

 unsigned char* buf; unsigned char* buf2;
  struct jpeg_decompress_struct cinfo;
  
  struct _error_mgr jerr;

  FILE * infile;
  JSAMPARRAY buffer;
  int row_len;

  if ((infile = fopen(file, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", file);
    return -1;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = _error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
   
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -1;
  }
 
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, infile);

  (void) jpeg_read_header(&cinfo, TRUE);

  (void) jpeg_start_decompress(&cinfo);
  
  w=cinfo.output_width;

  h=cinfo.output_height;
  count=0;
  row_len = w * cinfo.output_components;
 
  
 
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_len, 1);

  if((buf=(unsigned char*)malloc(sizeof(unsigned char)*row_len* h))==NULL){ fclose(infile); return -1; }; 
   
  if((buf2=(unsigned char*)malloc(sizeof(unsigned char)*w * 4 * h))==NULL){ fclose(infile); return -1; }; 

  while (cinfo.output_scanline < cinfo.output_height) {
   
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    
    memcpy(&buf[count], buffer[0], row_len);

   count+=row_len;
 
   }

  (void) jpeg_finish_decompress(&cinfo);
 
  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

 int y, x=0;

 switch(type){

case TP_WIN:
for(y=0; y<w*h*3; y+=3){

 buf2[x]=buf[y+2]; buf2[x+1]=buf[y+1];buf2[x+2]=buf[y]; buf2[x+3]=0xff; x+=4; };

break;
case TP_TEX:
for(y=0; y<w*h*3; y+=3){

 buf2[x]=buf[y]; buf2[x+1]=buf[y+1];buf2[x+2]=buf[y+2]; buf2[x+3]=0xff; x+=4; };
break;
   };
  img->buf=buf2;
  img->h=h;
 img->w=w;

  free(buf);

 return 0;

 };
  

void image_load(const char* file,Display* d,Window w){

   struct image img;

  if(ret_image(file, &img,TP_WIN)<0){ return; };
 
  Pixmap pix=XCreatePixmap(d, XDefaultRootWindow(d), img.w, img.h, DefaultDepth(d,DefaultScreen(d)));

  XImage* i=XCreateImage(d,XDefaultVisual(d, XDefaultScreen(d)),DefaultDepth(d,DefaultScreen(d)) ,ZPixmap,0, img.buf, img.w, img.h, 32,0);

XPutImage(d, pix, DefaultGC(d, 0), i, 0, 0,0, 0, 1200, 1100);
XSetWindowBackgroundPixmap(d, w, pix);
XFreePixmap(d,pix);
free(img.buf);
    };
void Par(struct model* m,int in){
int x,y,sq;
float xt,yt;
float* v;float* n;float* t;GLuint* i;
int sz=m[in].size;
sq=sqrt(sz);
float w=1.5;
float temp=(float)sq;
v=(float*)malloc(sizeof(float)*8*sz);

i=(GLuint*)malloc(sizeof(GLuint)*6*sz);

xt=-0.8;
for(x=0;x<sq;x++){
 xt+=w/temp;
 yt=-0.8;
for(y=0;y<sq;y+=1){
yt+=w/(temp);
v[3*x*sq+3*y]=xt;
v[x*3*sq+3*y+1]=0.01/((xt*xt)+(yt*yt));
v[x*3*sq+3*y+2]=yt;
 v[3*sz+3*x*sq+3*y]=0.0;
v[3*sz+x*3*sq+3*y+1]=0.0;
v[3*sz+x*3*sq+3*y+2]=0.0;
 v[6*sz+x*2*sq+2*y]=x/temp;
 v[6*sz+x*2*sq+2*y+1]=y/temp;  
   };
  };
 int Y=sq;
for(x=0;x<(Y-1);x+=1){
for(y=0;y<(Y-1);y++){
i[3*Y*x+3*y]=x*Y+y;
i[3*Y*Y+3*Y*x+3*y]=x*Y+y+1;
i[3*Y*x+3*y+1]=x*Y+y+Y;
i[3*Y*Y+3*Y*x+3*y+1]=x*Y+y+Y+1;
i[3*Y*x+3*y+2]=x*Y+y+1;
i[3*Y*Y+3*Y*x+3*y+2]=x*Y+Y+y;
   };
  };

m[in].vertex=v;
m[in].index=i;
  m[in].pos[0]=0.4; m[in].pos[1]=-0.1; m[in].pos[2]=-0.3;
m[in].scale[0]=1.0; m[in].scale[1]=1.0; m[in].scale[2]=1.0;
  glGenTextures(1,m[in].tex);
glBindTexture(GL_TEXTURE_2D,m[in].tex[0]);
loadtex(m[in].tex, m[in].texfile,0);
  LoadVBO(m,in);

  };
LoadVBO(struct model* m,int i)
 {
  int sz=m[i].size;
   glGenBuffers(1,m[i].buf);
  glBindBuffer(GL_ARRAY_BUFFER, m[i].buf[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sz*8,(float*)m[i].vertex,GL_STREAM_DRAW);

free(m[i].vertex);

 };
 DelModels(struct model* m,int c){
   int i;
   
 for(i=0;i<c;i++){
  glDeleteBuffers(1,m[i].buf);
     free(m[i].index);
   glDeleteTextures(1,m[i].tex);
    };
free(m);
  };  
void DrawScene(struct model* m, int first_num, int num)
   {
    glBindTexture(GL_TEXTURE_2D, m[0].tex[0]);
glUniform1i(glGetUniformLocation(prog, "i"),1);

/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);*/

int x, sz,loc,vec;
loc=glGetAttribLocation(prog, "texcoord");
glEnableVertexAttribArray(loc);
vec=glGetAttribLocation(prog, "vec");
glEnableVertexAttribArray(vec);/*
glEnableClientState(GL_VERTEX_ARRAY);*/
for(x=first_num;x<first_num+num;x++){
int sz=m[x].size;
glPushMatrix();

 glTranslatef(m[x].pos[0],m[x].pos[1], m[x].pos[2]);
/*glBindTexture(GL_TEXTURE_2D, m[x].tex[0]);*/
glRotatef(ang,0.2,1.0,-0.1);if(ang<360){ang+=0.05;}else{ang=0;};
glScalef(m[x].scale[0],m[x].scale[1],m[x].scale[2]);
glBindBuffer(GL_ARRAY_BUFFER,m[x].buf[0]);

glVertexAttribPointer(loc, 2 , GL_FLOAT, GL_FALSE, 0, (char*)(sizeof(float)*6*sz) );
glVertexAttribPointer(vec, 3 , GL_FLOAT, GL_FALSE, 0, (char*)NULL );


/*
glTexCoordPointer(2,GL_FLOAT,0,(char*)(sizeof(float)*6*sz));
/
glVertexPointer(3,GL_FLOAT,0,(char*)NULL);*/


glDrawElements(GL_TRIANGLES, 6*sz,GL_UNSIGNED_INT,m[x].index);
  glPopMatrix();
};glDisableVertexAttribArray(loc);
glDisableVertexAttribArray(vec);
/*
glDisableClientState(GL_VERTEX_ARRAY);
/*glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/ 
 };
 
void Fon()
  {/*
  float vec[]={-2.0,2.0,0.9,2.0,2.0,0.9,-2.0,-2.0, 0.9,-2.0,-2.0, 0.9};
  float tex[]={0.0,0.0,0.0,1.0,1.0,1.0,1.0,0.0};
  GLuint ind[]={0,1,2,3};
glUniform1i(glGetUniformLocation(prog, "i"),0);

int v=glGetAttribLocation(prog, "vec");
glEnableVertexAttribArray(v);
int t=glGetAttribLocation(prog, "texcoord");
glEnableVertexAttribArray(t);
glVertexAttribPointer(t, 2 , GL_FLOAT, GL_FALSE, 0, tex );
glVertexAttribPointer(v, 3 , GL_FLOAT, GL_FALSE, 0, vec );
glDrawElements(GL_QUADS, 4,GL_UNSIGNED_INT,ind);
  
glDisableVertexAttribArray(v);
glDisableVertexAttribArray(t);
*/
glUniform1i(glGetUniformLocation(prog, "i"),0);
glBegin(GL_QUADS);
glVertex3f(-2.0,2.0,0.9);
glTexCoord2f(0.0,0.0);
glVertex3f(2.0,2.0,0.9);
glTexCoord2f(0.0,1.0);
glVertex3f(2.0,-2.0,0.9);
glTexCoord2f(1.0,1.0);
glVertex3f(-2.0,-2.0, 0.9);
glTexCoord2f(1.0,0.0);
glEnd();

};
