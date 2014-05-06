#include <serv.h>

uint8_t serv::run;

 serv::serv(int s, uint8_t m) {
  sock=s;
 do{
 
if((efd=epoll_create(1024))<0){  break; };
 
      ev.data.fd=sock;
    ev.events=EPOLLIN;

epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
  
   ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
 run=1; starter=m-1; uint8_t n=m-1;
 qcount=starter;
pthread_t pt[n];
try{

  ques=new que[n];
}catch(std::bad_alloc& c){ run=0; close(efd); break; };
 
while(n>0){
 
 pthread_create(&pt[--n], NULL, th, this); 
    };
   break; 
  }while(1);
 }

 void serv::reactor(){

 int cur, s, i;
 uint8_t tm=starter, tmp=0;
 conn* p;
   while(run)
{  
  
  if(!tque.empty()){ 
    
  p=tque.front(); tque.pop();
   shutdown(p->fd,SHUT_RDWR); p->state=REQ_WAIT;
     
   };
 tmp=tm;
while(tmp>0){
  que & q=ques[--tmp];
    q.mt.lock();
 if(!q.rque.empty()){ 

    p=q.rque.front(); p->hand=0; q.rque.pop(); q.mt.unlock();

if(p->state==REQ_READ){ read_s(p); }else{
  
 if(write_s(p)){ tque.push(p); }; 
        };
       }else{ q.mt.unlock(); };
    };
          
 
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
                           p=new conn(s); std::shared_ptr<conn> ptr(p);       
                       std::pair<conn_it,bool> it=conn_map.insert(std::move(pr<int, std::shared_ptr<conn> >(s, std::move(ptr))));
                                
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
                          tque.push(p); };
                          continue;  
                            };
                          };
                         };
                       };
  conn_mp::iterator it=conn_map.begin();
 while(it!=conn_map.end()){
  s=it->first; ++it;
   epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s);
               };
            };
                      

int serv::cacher(const char* s, cache_t& ch){
 
 cache_t chc;
 char* p;
 int len;
 
 key_mp key(s);

  cache_it it; mtc.lock();
  if((it=cache_map.find(key))!=cache_map.end()){ 
    chc.pointer=it->second.pointer; chc.size=it->second.size; }
    else{ mtc.unlock();
          std::fstream ifs(s);
           ifs.seekg (0, ifs.end);
           len = ifs.tellg();
           ifs.seekg (0, ifs.beg);
          
          try { p=new char[len]; }catch(std::bad_alloc& b){ ifs.close(); return 1; };
         chc.pointer=std::shared_ptr<char>(p);
       ifs.read(chc.pointer.get(), len);
      ifs.close();
    chc.size=len; mtc.lock();
if(cache_map.insert(std::pair<key_mp, cache_t>(key, chc)).second==false){ mtc.unlock(); return 1; };
   }; mtc.unlock();
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
   ++qcount; if(qcount==starter){ qcount=0; };
 que & q=ques[qcount];
 s->hand=1; 
 q.mt.lock();
 q.hque.push(s);
 q.mt.unlock();
  };

  int serv::handler(){ 
 size_t sz;
  int error;
  conn* cs; 
   const char *tmp;
  uint8_t i=--qcount;
  cache_t ch;
  struct stat st;
  que & q=ques[i];
    while(run){
   error=0;
  q.mt.lock();
   if(q.hque.empty()){ q.mt.unlock(); usleep(10); continue; };

     cs=q.hque.front(); q.hque.pop();

     
     q.mt.unlock();
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
         switch(error){ case 1 :  hd::err_s1.copy(cs->header, hd::err_s1.size()); 
                           cs->buf_size=hd::err_s1.size(); cs->buf_send=cs->header; cs->state=REQ_WRITE; break;
                        default :  break;
                   
                    
                 }; cs->buf_size=0; cs->buf_send=NULL;               
               };
               if(error==0){ send_header(cs); cs->state=REQ_HEAD; };
                   
           };q.mt.lock(); q.rque.push(cs); q.mt.unlock();
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
 void serv::start(){  reactor(); };
 void* serv::th(void* p){ ((serv*)p)->handler(); }; 
 
