#include <iostream>
#include <string.h>
#include <stdio.h>

typedef uint8_t in;

class Tree{

struct pointer{

 pointer* next;
 char* str;
 char* char_ptr;
 };

public:

 Tree();
 
 ~Tree();
 
 char* insert(const char*);
 void erase(const char*);
 char* search(const char*);
 char* search(const char*, size_t);
 
private:

in hash(const char* s, in);
in bits[9984];
pointer* list[156];

};

Tree::Tree() {
 
unsigned i=9984; /* with 156 characters and no more then 64 character per word */ 
 
 memset(bits, 0, i);
 i=156;
  while(i>0){ list[--i]=NULL; }; 
 
 }; 

Tree::~Tree(){ 

 in i=156;

 pointer *ptr, *prev;

  while(i>0){ prev=list[--i];
 
   while(prev){
   
   ptr=prev->next;
printf("DELETED %s\n", prev->str);
    delete [] prev->char_ptr;
  
   prev=ptr;
   }
  }
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

 char* Tree::search(const char* s, size_t n){ 

  in mid=0, tmp=0, code;
  size_t sz;

  pointer* ptr;

if(((0xff & *s)>>4)==0xd){ code=2; }else{ code=1; };
 
 sz=n/code;
 
 while(tmp < sz && tmp < 64){ 

  mid=hash(s+code*tmp, code); 

 if(!(bits[64*tmp+mid] > 0)){ return NULL; }; 
  
  ++tmp;
 
 };
  
 ptr=list[mid];
printf("SEA %d\n",mid);
 while(ptr){  
  printf("SEA %d\n",mid);
   if(!strcmp(s, ptr->str)){ return ptr->str; };

    ptr=ptr->next;
 
   };

 return NULL;

 } 
char* Tree::search(const char* s){
  
   return search(s, strlen(s);
 
 };
 
char* Tree::insert(const char* s){

 pointer *ptr, *prev;
 char* sp;
 in mid=0, tmp=0, code;

 size_t sz;

if(((0xff & *s)>>4)==0xd){ code=2; }else{ code=1; };
 
 sz=strlen(s)/code;

 while(tmp < sz && tmp<64){ 

 mid=hash(s+code*tmp, code);

  ++bits[64*mid+tmp]; 
  ++tmp;

};

  ptr=list[mid];
  prev=ptr;

while(ptr){ 
 
  if(!strcmp(s, ptr->str)){ std::cout<<"YES ITS\n";return ptr->str; };
  prev=ptr;
  ptr=ptr->next; 

 };
 
 try{

  sp=new char[sizeof(pointer)+strlen(s)+1];

   }catch(std::bad_alloc& c){ return NULL; }; 

  ptr=reinterpret_cast<pointer*>(sp); 
 if(prev){ prev->next=ptr; printf("NEXT\n");}
 else{ list[mid]=ptr; };
  ptr->str=sp+sizeof(pointer);
  ptr->char_ptr=sp;
  ptr->next=NULL;
  strcpy(ptr->str, s);
printf("INSERTER OKK %s\n",ptr->str);
 return ptr->str;

 };

void Tree::erase(const char* s){

 in mid=0, tmp=0, code;
 size_t sz;
 pointer *ptr, *prev;

if(((0xff & *s)>>4)==0xd){ code=2; }else{ code=1; };

  sz=strlen(s)/code;
 
 while(tmp < sz && tmp<64){ 

  mid=hash(s+code*tmp, code); 

 if(!(bits[64*mid+tmp] > 0)){ return; };
   
 --bits[mid+tmp]; 

  ++tmp;

 };
 
 ptr=list[mid];
 prev=ptr;

do{ 
 
  if(ptr==NULL){ return; }; 
  
   if(!strcmp(s, ptr->str)){ break; };

    prev=ptr;

    ptr=ptr->next;
  
   }while(1);
 
 prev->next=ptr->next;
if(ptr==list[mid]){ list[mid]=ptr->next; };
printf("ERASE %s\n", ptr->str);
 delete [] ptr->char_ptr;

 };

