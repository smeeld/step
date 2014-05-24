
#include <scene.h>
 scene::scene(const char* f){
 
  struct stat st;
 uint8_t i, type;
size_t count=0, temp=0; int file, er;
char* b, *p, *pp, *tmp;
try{
if((er=stat(f, &st))!=0) throw 1;
 if((file=open(f, O_RDONLY))<0) throw 1;
 if((b=(char*)malloc(sizeof(char)*(st.st_size+1)))==NULL) throw 2;
 while((temp=read(file, b+count, st.st_size))>0){
        count+=temp; temp=0;
     }; b[count]='\0';
 close(file);
 p=b; pp=b+count;

 do{
 
  if((p=parse(p, "file", pp))!=NULL){ tmp=p; p+=strlen(p)+1; 
                      
      add_model(tmp); continue; };
      break;
   }while(1);
  }catch(int a){ if(a==2) close(file); };
 }

void scene::draw(){
 
auto it=sc.begin();
 while(it!=sc.end()){ it->get()->draw(); ++it; };
 };

void scene::add_model(const char* f){
 model* m;
 try{ 
    m=new model;
  }catch(std::bad_alloc& c){ return; };
   if(m->build(f)){ delete m; return; };
 sc.push_back(std::shared_ptr<model>(m)); 
 };  

