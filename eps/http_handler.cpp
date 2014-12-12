#include <sys/stat.h>
#include <http_handler.h>
#include <hd.hpp>
 void http_handler::send_header(http_conn* c){
  
  size_t sz;
  char date[25];

  time_t tm_t;
   time(&tm_t);
  strncpy(date,asctime(localtime(&tm_t)),25);
  date[24]='\0';
   std::ostringstream ost;
   ost.rdbuf()->pubsetbuf(c->header, 1024);
   ost.seekp(0);
      ost<<hd::s0.c_str()
      <<hd::s2.c_str()
      <<hd::s17.c_str()
      <<date<<hd::s19.c_str()  
      <<hd::s9<<hd::s20.c_str()
      <<hd::s18.c_str()
      <<hd::s11.c_str()
      <<c->buf_size<<"\r\n"
    <<hd::s5.c_str();
    sz=static_cast<size_t>(ost.tellp());
  c->header_len=sz;
     c->msg_add(c->header, sz);
    };

inline  int http_handler::req_gen (http_conn* c){

  http_conn* sp=c;

  char* b=sp->buf_recv;
  char* mid;
  char* tmp=b;
  char* end=b+sp->size_recv;
  int n;
   while(*b==' '){ b++; if(b==end){ return 1; }; };
 
 do{
    
  if(std::equal(b,b+3,hd::s14.c_str())){ sp->request.method=HTTP_GET; b+=3; break; };
  if(std::equal(b,b+4,hd::s15.c_str())){ sp->request.method=HTTP_POST; b+=4; break; };
 
    }while(1);

  while(*b==' '){ b++; if(b==end){ return 1; }; };
   n=0;
   while(*(b+n)!=' '){  n++; };

   sp->request.path=b; b+=n; *b='\0';


    do{
 
    tmp=const_cast<char*>(hd::s16.c_str());
   if((tmp=std::search(b, end, tmp, tmp+4))==end){ 
 
     tmp=const_cast<char*>(hd::s30.c_str());
   if((tmp=std::search(b, end, tmp, tmp+4))==end){ sp->request.host=NULL; break;  }; 
      };
         tmp+=5;
  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
   sp->request.host=tmp;
    
 while(*tmp!=' ' && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   *tmp='\0'; break;
  }while(1);
 
 
    tmp=const_cast<char*>(hd::s31.c_str());
   if((tmp=std::search(b, end, tmp, tmp+4))==end){ return 1; };
     sp->request.header_size=tmp-sp->buf_recv+4;
      
  do{
      tmp=const_cast<char*>(hd::s9.c_str());
   if((tmp=std::search(b, end, tmp, tmp+12))==end){ 
    
     tmp=const_cast<char*>(hd::s10.c_str());
   if((tmp=std::search(b, end, tmp, tmp+12))==end){ sp->request.content_type=0; break;  }; 
     
   }; 
       
       tmp+=13;
     
    while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
        
         sp->request.content_type=tmp;
          
    while(*tmp!=' '){ tmp++; if(tmp==end){ return 1; }; }; *tmp='\0'; tmp++;
    while(*tmp==' '){ tmp++;  if(tmp==end){ return 1; }; }; 

     sp->request.encoding=tmp;
 
    while(*tmp!=' ' || *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; }; *tmp='\0'; break;

   }while(1);
 

  do{

     tmp=const_cast<char*>(hd::s12.c_str());

   if((tmp=std::search(b, end, tmp, tmp+10))==end){ 

      tmp=const_cast<char*>(hd::s26.c_str());
   if((tmp=std::search(b, end, tmp, tmp+10))==end){ sp->request.connection_keep=1; break;  }; 
      };
         tmp+=11;

  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
   
    mid=tmp+1;

  while(*tmp!=' ' && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   
    *tmp='\0'; if(std::equal(mid, mid+4, hd::s32.c_str())){ sp->request.connection_keep=0; sp->keep=0; break; };

      sp->request.connection_keep=1; break;
 
      }while(1);

    do{

      tmp=const_cast<char*>(hd::s28.c_str());
   if((tmp=std::search(b, end, tmp, tmp+16))==end){ 

      tmp=const_cast<char*>(hd::s29.c_str());
   if((tmp=std::search(b, end, tmp, tmp+16))==end){ sp->request.content_language=NULL; break;  }; 
      };
         tmp+=17;
  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
   sp->request.content_language=tmp;
  while(*tmp!=' '  && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   *tmp='\0'; break;
  }while(1); 
  
 
 if(sp->request.method==HTTP_POST){
  
   do{

     tmp=const_cast<char*>(hd::s11.c_str());
   if((tmp=std::search(b, end, tmp, tmp+14))==end){ 

     tmp=const_cast<char*>(hd::s27.c_str());
   if((tmp=std::search(b, end, tmp, tmp+14))==end){ sp->request.content_length=0; return 1;  }; 
      
      };
         tmp+=15;
  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
  char* point=tmp;
  while(*tmp!=' '  && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   *tmp='\0';
  sp->request.content_length=std::stoll(point); break;
        
      }while(1);
   }; 
 return 0;
    };

void http_handler::handler(conn* s){
    struct stat st;
     size_t sz;
     int error;
     uint8_t i=1;
     cache_t ch;
 switch(s->hand){
   case 0 : s->state=REQ_READ; return;
   case REQ_WRITE : 
          s->msg.msg_iovlen=0; s->buf_size=0; 
             s->state &=~ REQ_WRITE; s->state |= REQ_SHUT;
     return;  
   case REQ_READ :
   if(s->state & REQ_WAIT) s->state &= ~REQ_WAIT; 
    http_conn* cs=static_cast<http_conn*>(s);
    if(req_gen(cs)){ error_hand(cs, 1); return; };
   const char *tmp=cs->request.path;
          
     if(cs->request.method==HTTP_GET){
      try{
          errno=0;
        i=stat(tmp, &st); if(i<0 || errno==ENOENT)  throw 1;
          
       if(st.st_size>100000){ cs->type=REQ_SENDFILE;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0) throw 1;
             send_header(cs); 
              }
               else{ cs->type=REQ_SENDMSG;
         if(cacher(tmp, ch))  throw 1;
             char* bf=ch.pointer.get(); size_t sz=ch.size;
               
               cs->buf_send=bf; cs->buf_size=sz;
              send_header(cs);
              cs->msg_add(bf, sz);
                 };       
            }catch(int er){  error_hand(cs, er); };
                  
          };
        s->state |= REQ_WRITE;

         };
   };
int http_handler::cacher(const char* s, cache_t& ch){
 
 cache_t chc;
 char* p;
 int len;
 
 key_mp key(s);

  auto it=cache_map.find(key);
  if(it!=cache_map.end()){ 
    chc.pointer=it->second.pointer; chc.size=it->second.size; }
    else{ 
          std::fstream ifs(s);
           ifs.seekg (0, ifs.end);
           len = ifs.tellg();
           ifs.seekg (0, ifs.beg);
          
          try { p=new char[len]; }catch(std::bad_alloc& b){ ifs.close(); return 1; };
         chc.pointer=std::shared_ptr<char>(p);
       ifs.read(chc.pointer.get(), len);
      ifs.close();
    chc.size=len;
if(cache_map.insert(std::pair<key_mp, cache_t>(key, chc)).second==false) return 1;
   };
  ch.pointer=chc.pointer; ch.size=chc.size;

  return 0;
 };
void http_handler::error_hand(http_conn* s, uint8_t er){
        int error=er;
         http_conn* cs=s;
         char* bf;
         size_t sz; 
         switch(error){ case 1 : bf=cs->header; sz=hd::err_s1.size();
                                 hd::err_s1.copy(bf, sz); 
                           cs->buf_size=sz; cs->buf_send=bf;
                            cs->msg_add(bf, sz);  break;
                        default :  break;
                      };
                 cs->request.connection_keep=0;
                 
          };

void http_handler::destroy_conn(conn* p){
  http_conn* s=static_cast<http_conn*>(p);
   delete s;
 };

conn* http_handler::init_conn(){
  http_conn* p;
  try{
  p=new http_conn;
     }catch(std::bad_alloc& s){ return nullptr; };
   return static_cast<conn*>(p);
  };

conn* http_handler::init_conn(conn* p){
   http_conn* pp=new (static_cast<http_conn*>(p)) http_conn;
   return static_cast<conn*>(pp);
  };
  
  
