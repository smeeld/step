#include <iostream>
#include <string.h>
typedef uint8_t in;

class Tree{

public:

 Tree();
 
 ~Tree();
 
 void* insert(const char*);
 void erase(const char*);
 void remove(const char*);
 void* search(const char*);
 
private:

in hash(const char* s, in);
void** bits_clear(const char*);
in check(){ if(bits==NULL || pointers==NULL) return 1; };
in* bits;
void** pointers;

};

Tree::Tree() {
 
unsigned i=4992; /* with 156 characters and no more then 32 character per word */ 
  do{ 
  bits=malloc(i/8); if(!bits) break;
  pointers=malloc(sizeof(void*)*i); if(!pointers){ free(bits); break; }  
   
 while(i-- > 0){ pointers[i]=NULL; };
   }while(0);
  }; 
 

Tree::~Tree(){ 

 unsigned i=4992;

  while(i-- >0){ if(pointers[i]) delete ponters[i]; };
    free(pointers); free(bits);
 };     


inline in Tree::hash(const char* s, in a){
  
  char c;
  uint16_t sh;
  in r;
  char md[2];
if(a>1){

  memcpy(md, s, 2);
  std::swap(md[0], md[1]);
  memcpy(&sh, md, 2);
 
if(sh>53439){sh-=53584; }else{ sh-=53392; };
 
 r=sh; 
}else{ c=*s; r=c; r+=18; };
 
 if(r>156){ r=155; };
  
return r;

 };

 void* Tree::search(const char* s){ 

  in mid=0, tmp=0, code=(((0xff & *s)>>4)==0xd) ? 2 : 1;;
  size_t sz=strlen(s)/code;

 if(sz>32 || check()) return NULL;
 while(tmp < sz){ 

  mid=hash(s+code*tmp, code); 
  mdl=mid*4+tmp/8; 
  if(!(bits[mdl] & 1<<tmp%8)) return NULL; 
  
  ++tmp;
 
 };
  
 return pointers[32*tmp+mid];
 };

 
char* Tree::insert(const char* s, void* pt){

 in mid=0, tmp=0, code=(((0xff & *s)>>4)==0xd) ? 2 : 1;
  unsigned mdl;
  size_t sz=strlen(s)/code;
  if(sz>32 || check()) return NULL;

 while(tmp < sz){
 mid=hash(s+code*tmp, code);
   mdl=mid*4+tmp/8; 
  if(!(bits[mdl] & 1<<tmp%8)) bits[mdl] |=1<<tmp%8; 
  ++tmp;
};

  ptr=pointers[32*mid+tmp;];
  if(!ptr) ptr=pt;
  return ptr;
};
template <typename T, typename... U >
T* Tree::insert(const char* s, U... t){
  void* p=malloc(sizeof(T)); if(!p) return NULL;
   if(!insert(s, p)) return NULL;    
    
  T* pp=new (p) T(t...);

   return p;
 };

void** Tree::bits_clear(const char* s){
  void** ptr;
 in mid=0, tmp=0, code=(((0xff & *s)>>4)==0xd) ? 2 : 1;
 size_t sz=strlen(s)/code;
   if(sz>32 || check()) return NULL;
 while(tmp < sz){ 
  mid=hash(s+code*tmp, code); 
  mdl=mid*4+tmp/8;
  if(!(bits[mdl] & 1<<tmp%8)) return NULL; 
  
  ++tmp;

 };
ptr=pointers+32*mid+tmp;
 do{
  mid=hash(s+code*tmp, code);
   mdl=mid*4+tmp/8;
   if(pointers[mid*32+tmp]) break;
    bits[mdl] &=~(1<<tmp%8); }while(tmp--);
  return ptr;
 };

void Tree::remove(const char* s){

  void** ptr=bits_clear(s); if(!*ptr) return;
   *ptr=NULL;
};

void Tree::erase(const char* s){

  void** ptr=bits_clear(s); if(!*ptr) return;
   delete *ptr;
};  
 
int main(){ std::cout<<"OK\n"; };
