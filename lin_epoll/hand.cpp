#include <serv.h>
  

  int serv::handler(){ 
 size_t sz;
  int error;
  conn* cs;
   const char *tmp;
  int i=0;
  cache_t ch;
   int rsz;
  struct stat st;
    while(1){
   error=0;
  mt.lock();
   if(hques.empty()){ mt.unlock(); usleep(10); continue; };

     cs=hques.front(); hques.pop();

     
      mt.unlock();
      
          tmp=cs->request.path;
          
     if(cs->request.method==HTTP_GET){
      try{
          errno=0;
        i=stat(tmp, &st); if(i<0 || errno==ENOENT){ throw 2; };
          
       if(st.st_size>100000){ cs->type=1;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0){ throw 2; };
              }
               else{ cs->type=0;
         if((i=cacher(tmp, ch))!=0){ throw i; };
               cs->buf_send=ch.pointer.get(); cs->buf_size=ch.size;
            
               };
           
         }catch(int er){ error=er; 
         switch(error){ case 1 :  hd::err_s2.copy(cs->header, hd::err_s2.size()); cs->header_len=hd::err_s2.size(); break;
                        case 2 :     hd::err_s1.copy(cs->header, hd::err_s1.size()); cs->header_len=hd::err_s1.size(); break;
                    default : hd::err_s3.copy(cs->header, hd::err_s3.size()); cs->header_len=hd::err_s3.size(); break;
                    
                 }; cs->buf_size=0; cs->buf_send=NULL;               
               };
               if(error==0){ send_header(cs); };
                   cs->state=REQ_WRITE;
           }; mt.lock(); rques.push(cs); mt.unlock(); 
         };
       };
   
  
void serv::send_header(conn* c){
  
  int sz;
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
      <<hd::s10.c_str()
      <<c->buf_size<<"\r\n"
    <<hd::s5.c_str();
 
  c->header_len=static_cast<int>(ost.tellp());

    };

  void* serv::th(void* a) { ((serv*)a)->handler(); return NULL; };        

