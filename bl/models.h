#include <image.h>
#include <typeinfo>
struct parser{


 char* parse(char* p, const char* s, char* pp) const {
  
  char *tmp=p; size_t sz=0, len=strlen(s);
   
 do{
while(*p <45 || *p >122){ if(p==pp) return NULL; ++p; };
  tmp=p; sz=0;
while(*p > 44 &&  *p < 123){ if(p==pp) return NULL; ++p; ++sz;};
   if(sz!=len) continue;
   if(strncmp(tmp, s, sz)==0){ 
while(*p <45 || *p >122){ if(p==pp) return NULL; ++p; }; tmp=p; sz=0;
while(*p > 44 &&  *p < 123){ ++p; ++sz; }; *p='\0'; return tmp;
       };

  }while(1);  
 };


template <typename T>
void parse_array(char* p, T* vec, char* pp){

 size_t sz=0; char* tmp;
do{ if(p==pp && *p=='}')  return; 
   while(*p <45 || *p >122){  ++p; if(p==pp || *p=='}')  return; }; tmp=p;
    while(*p > 44 &&  *p < 123){ if(p==pp)  return; ++p; }; *p='\0'; vec[sz]=cast(tmp, vec+sz); ++sz;
     
       }while(1);
    
  };

template <typename T>
T cast(char* s, T* p){   if(typeid(p[0])==typeid(float)){ return std::stof(s); }else{ return std::stoi(s); }; }; 

  };

class model {

 public:
  model(){};
 ~model(){ deleter(); };
 void draw();
 void buffer_gen();
 int vertex_number;
int texcoord_number;
int texture_number;
int index_number;
float* vertex;
GLuint* index;
int build(const char*);
protected:

template <typename T>
int filler(const T&);
void deleter();
void shader(GLuint, char*);
void loadtex(const char*);
float pos[3];
float rpos[3];
float scale[3];
float ang;
float rstep;
GLuint tex[16];
GLuint shaders[3];
int program;
GLuint buf;
GLuint shd_count;
GLuint tex_count;
GLuint flag, type, mode;
template <typename P=parser >
char* parse(char* a, const char* b, char* c){P p; return p.parse(a, b, c); };
template <typename T, typename P=parser >
void parse_array(char* a, T* b, char* c){ P p;  p.parse_array(a, b, c); };

};


class sphere{
public:
  sphere(){};
  ~sphere(){};
 int create(model*) const;
 
 };

