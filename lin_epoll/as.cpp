#include <serv.h>

uint8_t serv::run;

 serv::serv(int s) {
  sock=s;
 
 if((efd=epoll_create(1024))<0){  throw 1; };
 
    
   ev.data.fd=sock;
    ev.events=EPOLLIN;
 
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
  
   ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
 run=1; tm=-1; starter=0;
 omp_init_lock(&mt);
 omp_init_lock(&mtc);
#pragma omp parallel
{
th();
  };

 }
 void serv::reactor(){

 int cur, s, i;
 uint8_t tm;
 conn* p;

   while(run)
{  
  
  if(!ques.empty()){ 
    
  p=ques.front(); ques.pop();
   shutdown(p->fd,SHUT_RDWR); p->state=REQ_WAIT; tm=1;
     
   };
    omp_set_lock(&mt);
 if(!rques.empty()){ 

    p=rques.front(); p->hand=0; rques.pop(); 

if(p->state==REQ_READ){ read_s(p); }else{
  
 if(write_s(p)){ ques.push(p); }; 
        };
       }; omp_unset_lock(&mt);
   
          
 
  cur=epoll_wait(efd,events,1024, 1);
    
    for(i=0; i<cur; i++)
     { 
         pev=&events[i]; s=events[i].data.fd;
         
      
          if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
                   p=conn_map[s].get();
                   
	          if(p->hand){ p->keep=1; continue; };
                
              conn_map.erase(s);
           epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
              close(s);
                 
                  continue; 
                      };

           
          
            if(s==sock){ 
                      
          if((s=accept(sock,(struct sockaddr*)&sockddr,&socklen))<0){  continue; };
                          
              set_non_block(s);
                   
                    ev.data.fd=s;
                 
                  if(epoll_ctl(efd,EPOLL_CTL_ADD,s,&ev)<0){ close(s); continue; };
                   
                     try{ 
                           p=new conn(s);        
                       std::pair<conn_it,bool> it=conn_map.insert(std::pair<int, std::shared_ptr<conn> >(s, std::shared_ptr<conn>(p)));
                                
                        if(it.second==false){  delete p; throw std::bad_alloc(); }; }
                       catch(std::bad_alloc& g){  
                         epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s);
                          };   
                        
                           continue;
                            }
                    else {
                   
                          p=conn_map[s].get();  if(p->hand){ continue; };
                        
                        if(p->state==REQ_HAND){ continue; };
                             
                        if(pev->events & EPOLLIN){
                               if(read_s(p)){
                                   continue; };
     
                                  pass_hand(p);
                                      continue;
                                    };
                 if(pev->events & EPOLLOUT){
                         if(write_s(p)){
                          ques.push(p); };
                          continue;  
                            };
                          };
                         };
                       };
                      };
                      

int serv::cacher(const char* s, cache_t& ch){
 
 cache_t chc;
 char* p;
 int len;
 
 key_mp key(s);

  cache_it it; omp_set_lock(&mtc);
  if((it=cache_map.find(key))!=cache_map.end()){ 
    chc.pointer=it->second.pointer; chc.size=it->second.size; }
    else{ omp_unset_lock(&mtc);
          std::fstream ifs(s);
           ifs.seekg (0, ifs.end);
           len = ifs.tellg();
           ifs.seekg (0, ifs.beg);
          
          try { p=new char[len]; }catch(std::bad_alloc& b){ ifs.close(); return 1; };
         chc.pointer=std::shared_ptr<char>(p);
       ifs.read(chc.pointer.get(), len);
      ifs.close();
    chc.size=len; omp_set_lock(&mtc);
if(cache_map.insert(std::pair<key_mp, cache_t>(key, chc)).second==false){ omp_unset_lock(&mtc); return 1; };
   }; omp_unset_lock(&mtc);
  ch.pointer=chc.pointer; ch.size=chc.size;

  return 0;
 };
   

  inline void serv::set_non_block(const int s){

 int fl=1;
if (ioctl(s, FIONBIO, &fl) &&
	    ((fl = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, fl | O_NONBLOCK) < 0)) {close(s); };
 };

 void serv::sig_hand(int n){ serv::run=0; };
 void serv::sig_pipe(int n){};

              
 int serv::write_s(conn* c){
   int i; 
           
           if(c->state==REQ_READ || c->state==REQ_WAIT){   return 0; };
 
           if(c->keep){ shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT; return 0; };
                     
           if(c->state==REQ_HEAD){
    
          do{errno=0;  i=send(c->fd,c->header,c->header_len,0);   

           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
               shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT;
                         return 0; }; 
                 break; c->state=REQ_WRITE;
                    }while(1);
                  }
      
         if((c->type)==0){
       do{ errno=0; i=write(c->fd, c->buf_send+c->size_tr, c->buf_size-c->size_tr);
  
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){  errno=0; continue; };
                 shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT; 
                        return 0; }; 
                c->size_tr+=i; i=0; break;
                    }while(1);
                 };
    if(c->type==1){ 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, c->buf_size);
           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
                      
                     shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT;
                        return 0; }; 
                c->size_tr+=i; i=0; break;
                 }while(1);
                };
             
          if(c->size_tr==c->buf_size){ if(c->type==1){ close(c->file_fd); };
             if(c->request.connection_keep){ 
               
              c->size_recv=0; c->state=REQ_WAIT;c->size_tr=0; i=1; }else{ shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT; i=0; };
                 };
       return i;
      };

 int serv::read_s(conn* c){ 
   int i;
        
      if(c->state==REQ_WRITE){ return 1; };

     if(c->keep){ shutdown(c->fd,SHUT_RDWR); c->state=REQ_WAIT; return 1; };
 
    do{errno=0; i=read(c->fd, c->buf_recv, 1024); 
        
       if( i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0;  continue; }; 
          shutdown(c->fd, SHUT_RDWR); c->state=REQ_WAIT; i=1; break; };
            c->size_recv+=i; i=0;
                
             break; }while(1);
  
     if(c->size_read){ if(c->size_recv==c->size_read){ i=0; }else{ i=1; };  return i; };
        
         c->state=REQ_HEAD;  
        return i;
        };
 
 inline  int serv::req_gen (conn* c){

  conn* sp=c;
switch(sp->state){
 case REQ_HEAD :
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
   if((tmp=std::search(b, end, tmp, tmp+12))==end){ sp->request.content_type=NULL; break;  }; 
     
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
   if((tmp=std::search(b, end, tmp, tmp+10))==end){ sp->request.connection_keep=0x00; break;  }; 
      };
         tmp+=11;

  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
   
    mid=tmp+1;

  while(*tmp!=' ' && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   
    *tmp='\0'; if(std::equal(mid, mid+4, hd::s32.c_str())){ sp->request.connection_keep=0;  break; };

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
   if((tmp=std::search(b, end, tmp, tmp+14))==end){ sp->request.content_length=NULL; return 1;  }; 
      
      };
         tmp+=15;
  while(*tmp==' '){ tmp++; if(tmp==end){ return 1; }; };
  char* point=tmp;
  while(*tmp!=' '  && *tmp!='\r'){ tmp++; if(tmp==end){ return 1; }; };
   *tmp='\0';
  sp->request.content_length=std::stoll(point); break;
        
      }while(1);
   }; 
 break;
 };
 return 0;
    };
inline void serv::pass_hand(const conn* c){

 conn* s=const_cast<conn*>(c);
   
 omp_set_lock(&mt);
 s->hand=1; 
 hques.push(s);
 omp_unset_lock(&mt);
  };
  int serv::handler(){ 
 size_t sz;
  int error;
  conn* cs;
   const char *tmp;
  int i=0;
  cache_t ch;
   int rsz;
  struct stat st;
    while(run){
   error=0;
  omp_set_lock(&mt);
   if(hques.empty()){ omp_unset_lock(&mt); usleep(10); continue; };

     cs=hques.front(); hques.pop();

     
     omp_unset_lock(&mt);
      req_gen(cs);
          tmp=cs->request.path;
          
     if(cs->request.method==HTTP_GET){
      try{
          errno=0;
        i=stat(tmp, &st); if(i<0 || errno==ENOENT){  throw 1; };
          
       if(st.st_size>100000){ cs->type=1;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0){ throw 1; };
              }
               else{ cs->type=0;
         if(cacher(tmp, ch)){ cs->type=1;
                              cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0){ throw 1; };
             
            }else{
               cs->buf_send=ch.pointer.get(); cs->buf_size=ch.size;
                 };
               };
           
         }catch(int er){ error=er; 
         switch(error){ case 1 :  hd::err_s1.copy(cs->header, hd::err_s1.size()); cs->header_len=hd::err_s1.size(); break;
                        case 2 :  cs->type=1; cs->buf_size=st.st_size;  error=0; break;
                   
                    
                 }; cs->buf_size=0; cs->buf_send=NULL;               
               };
               if(error==0){ send_header(cs); };
                   cs->state=REQ_HEAD;
           };omp_set_lock(&mt); rques.push(cs);omp_unset_lock(&mt); 
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

  void serv::th(void) { uint8_t s;omp_set_lock(&mtc); s=starter++; omp_unset_lock(&mtc); if(s>0){ handler(); }else{ reactor(); };  };        
  
