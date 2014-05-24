
#include <models.h>

int model::build(const char* f){ 
 
 struct stat st;

 int i;
size_t count=0, temp=0, file, er;
char* b, *p;
 shd_count=0;
 tex_count=0;
 program=glCreateProgram();
  if(program==0) return 1;
   flag=1;
if((er=stat(f, &st))!=0)  return 1;
 if((file=open(f, O_RDONLY))<0) return 1;
 
 if((b=(char*)malloc(sizeof(char)*(st.st_size+1)))==NULL) return 1;
 while((temp=read(file, b+count, st.st_size))>0){
        count+=temp; temp=0;
     }; b[count]='\0';
 close(file);
 p=b;
 char* pp=p+count;

 do{ if((p=parse(p, "vershaderfile", pp))!=NULL){  shader(0, p); p+=strlen(p)+1; continue; };
      break; }while(1);
     p=b;
 do{ if((p=parse(p, "fragshaderfile", pp))!=NULL){  shader(1, p); p+=strlen(p)+1; continue; };
    break; }while(1);
glUseProgram(program);
glLinkProgram(program);
 p=b; i=0;
 do{ if((p=parse(p, "rpos",pp))!=NULL){  rpos[i++]=std::stof(p); p+=strlen(p)+1; continue; };
      break; }while(1);p=b; i=0;
 do{ if((p=parse(p, "pos",pp))!=NULL){  pos[i++]=std::stof(p); p+=strlen(p)+1; continue; };
      break; }while(1);p=b; i=0;
 do{ if((p=parse(p, "scale", pp))!=NULL){  scale[i++]=std::stof(p); p+=strlen(p)+1; continue; };
      break; }while(1);
 p=b;
if((p=parse(p, "rstep", pp))!=NULL){ rstep=std::stof(p); }else{ rstep=0.0; };
p=b;
if((p=parse(p, "type", pp))!=NULL){ type=std::stoi(p); }else{ type=0; }; 
p=b;
if((p=parse(p, "mode", pp))!=NULL){ type=std::stoi(p); if(type){ mode=GL_TRIANGLES; }else{ mode=GL_QUADS; }; }else{ mode=GL_TRIANGLES; };  


p=b;
 if((p=parse(p, "texture_number", pp))!=NULL){ texture_number=atoi(p); }else{ texture_number=0; };

 glGenTextures(texture_number, tex);
p=b;
 do{ if((p=parse(p, "texfile", pp))!=NULL){  loadtex((const char*) p); p+=strlen(p)+1; continue; };
      break; }while(1);
 p=b;
 if((p=parse(p, "vertex_number", pp))!=NULL){ vertex_number=atoi(p);  }else{ vertex_number=0; };
 p=b;
 if((p=parse(p, "texcoord_number", pp))!=NULL){ texcoord_number=atoi(p); }else{ texcoord_number=0; };
 p=b;
 if((p=parse(p, "index_number", pp))!=NULL){ index_number=std::atol(p); }else{ index_number=0; };                  
  
 float* s=NULL;
 GLuint* ind=NULL;
 
try{
 if((s=(float*) malloc(sizeof(float)*(2*texcoord_number+3*vertex_number)))==NULL){ throw std::bad_alloc(); };
if((ind=(GLuint*)malloc(sizeof(GLuint)*index_number))==NULL){  throw std::bad_alloc(); };
}catch(std::bad_alloc& c){ free(vertex); free(index);  return 1; }

p=b;
if((p=parse(p, "vec", pp))!=NULL){ parse_array(p, s, pp); };
p=b;
if((p=parse(p, "coord", pp))!=NULL){ parse_array(p, s+vertex_number*3, pp); };
 p=b;
if((p=parse(p, "index", pp))!=NULL){ parse_array(p, ind, pp); };
 index=ind;
 vertex=s;
 free(b);
 ang=0;
switch(type){
 case 1 : i=filler(sphere()); break;
 default : i=0; break;
   }; 

 buffer_gen();
 return i;
 
}

 void model::deleter(){
 free(vertex); free(index);
 if(flag){
   while(shd_count>0){ glDeleteShader(shaders[--shd_count]); };
  glDeleteProgram(program);
  glDeleteTextures(tex_count, tex);
  glDeleteBuffers(1, &buf);
  };
 };

 void model::shader(GLuint n, char* s){

struct stat st;
int count=0, temp=0, file, er;
GLuint shd;
GLint ln;
char* b;

if((er=stat(s, &st))!=0){ return; };
 if((file=open(s, O_RDONLY))<0){ return; };
 
 if((b=(char*)malloc(sizeof(char)*st.st_size+1))==NULL){ return; };
 while((temp=read(file, b+count, st.st_size-count))!=0){
        count+=temp; temp=0;
     };b[count]='\0';
close(file);

switch(n){
case 0 :
shd=glCreateShader(GL_VERTEX_SHADER); break;
case 1 :
shd=glCreateShader(GL_FRAGMENT_SHADER); break;
default : break;
};
 ln=count;
glShaderSource(shd, 1, (const GLchar**) &b, &ln);
glCompileShader(shd);
glAttachShader(program, shd);
int info;
GLuint shv;
glGetShaderiv(shd,GL_COMPILE_STATUS, &info);
 free(b); 
 shaders[shd_count++]=shd;
};


void model::loadtex(const char* s){
 
 image img;
 char tx[5]; tx[4]='\0'; tx[3]=(char)(tex_count+97);
  tx[0]='t'; tx[1]='e'; tx[2]='x';
 if(img.ret_image(s, TP_TEX)<0) return; 
glBindTexture(GL_TEXTURE_2D, tex[tex_count]);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, img.w, img.h, 0, GL_RGBA,GL_UNSIGNED_BYTE, img.buf);
 glUniform1i(glGetUniformLocation(program, tx), tex_count);
 glActiveTexture(GL_TEXTURE0 + tex_count);
  glBindTexture(GL_TEXTURE_2D, tex[tex_count]);
  tex_count++;
 
  };

 void model::draw(){
 int x, loc, vec;
 glUseProgram(program);
 glBindTexture(GL_TEXTURE_2D, tex[0]);
  loc=glGetAttribLocation(program, "texcoord");
   glEnableVertexAttribArray(loc);
 vec=glGetAttribLocation(program, "vec");
  glEnableVertexAttribArray(vec);
glPushMatrix();

 glTranslatef(pos[0], pos[1], pos[2]);

glRotatef(ang, rpos[0], rpos[1], rpos[0]);if(ang<360){ ang+=rstep; }else{ang=0;};
glScalef(scale[0], scale[1], scale[2]);
glBindBuffer(GL_ARRAY_BUFFER, buf);

glVertexAttribPointer(loc, 2 , GL_FLOAT, GL_FALSE, 0, (char*)(sizeof(float)*3*vertex_number) );
glVertexAttribPointer(vec, 3 , GL_FLOAT, GL_FALSE, 0, (char*)NULL );

glDrawElements(mode, index_number, GL_UNSIGNED_INT, index);
  glPopMatrix();
glDisableVertexAttribArray(loc);
glDisableVertexAttribArray(vec);

};


void model::buffer_gen(){

  glGenBuffers(1, &buf);
  glBindBuffer(GL_ARRAY_BUFFER, buf);
glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(3*vertex_number+2*texcoord_number), (float*)vertex, GL_STREAM_DRAW);

 }
template <typename T>
int model::filler(const T& p) { return p.create(this) ; };

