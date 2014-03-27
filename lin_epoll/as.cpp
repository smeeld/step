#include <serv.h>

int serv::run;

 serv::serv(int s){
  sock=s;
 
 if((efd=epoll_create(16384))<0){  throw 1; };
 
    
   ev.data.fd=sock;
    ev.events=EPOLLIN;
 
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
  
   ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
 run=1; 
 pthread_t pt;
 pthread_create(&pt,0,th,this);
  };
 
 serv::~serv(){}; 
void serv::proc_thread(const conn* s){ 
 conn* cs=const_cast<conn*>(s);
 size_t sz;
  int error;
   char *tmp, *pos;
  int j;
  int i=0;
  cache_t ch;
  
   int rsz;
  struct stat st;
       
   switch(cs->state){
    case SOCK_READ : error=0;
      pos=cs->buf_recv; rsz=cs->size_recv-1;
    
     if(!strncmp(pos,hd::s14.c_str(),3)){
      tmp=pos+3; i=0; pos+=(cs->size_tr-1);
 try{
      while(i==0){ tmp++;
          
       while(*(tmp+i)!=' '){  if(pos==(tmp+i)){ throw 1; }; i++; }; };
       
        char s[i]; s[i]='\0';
         std::copy(tmp, tmp+i, s);
          errno=0;
        i=stat(s,&st); if(i<0 || errno==ENOENT){ throw 2; };
          
       if(st.st_size>100000){ cs->type=1;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(s,O_RDONLY))==0){ throw 2; };
              }
               else{ cs->type=0;
         if((i=cacher((const char*)s, ch))!=0){ throw i; };
               cs->buf_send=ch.pointer.get(); cs->buf_size=ch.size;
            
               };
           
         }catch(int er){ error=er; 
         switch(error){ case 1 : cs->buf_send=const_cast<char*>(hd::err_s1.c_str()); cs->buf_size=hd::err_s1.size(); break;
                        case 2 :    cs->buf_send=const_cast<char*>(hd::err_s1.c_str()); cs->buf_size=hd::err_s1.size(); break;
                    default : cs->buf_send=const_cast<char*>(hd::err_s3.c_str()); cs->buf_size=hd::err_s3.size(); break;
                       
                 }; cs->state=SOCK_WRITE;               
               };
                  if(cs->state!=SOCK_WRITE){
                   send_header(cs); };
                        break;
        };
 default : break;
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
     
       
        c->state=SOCK_HEADER;
         return;
          
          
          };
         
 void serv::reactor(){
 int cur, s, i;
 conn* p;

   while(1)
{  
  
  cur=epoll_wait(efd,events,512, -1);
    
    for(i=0; i<cur; i++)
     { 
         pev=&events[i]; s=events[i].data.fd;
         
       if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
	          
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
                                
                        if(it.second==false){  throw std::bad_alloc(); }; }
                       catch(std::bad_alloc& g){ conn_map.erase(s);  
                         epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s);  continue; };   
                        
                            if(read_s(p)){
                               queue_insert(s); continue;
                                     };
                           proc_thread(p);

                            if(write_s(p)){ 
                                 queue_insert(s); continue;
                                        };
                             continue;
                            }
                    else {
                             p=conn_map[s].get();
                        if(pev->events & EPOLLIN){           
                              if(read_s(p)){
                                     queue_insert(s); };                  
                                        continue;
                                    };
                 if(pev->events & EPOLLOUT)
                          { if(write_s(p)){ 
                                 queue_insert(s); };
                                  continue;
                            
                            };
                          };
                         };
                       };
                      };
                      
                    

 inline void serv::queue_insert(int s){ 
   
     mt.lock();
    ques.push(s);
     mt.unlock();
       return;
        };

int serv::cacher(const char* s, cache_t& ch){
 
 cache_t chc;
 int len;
  size_t sz=hsh(const_cast<char*>(s));
  cache_it it;
  if((it=cache_map.find(sz))!=cache_map.end()){
    chc.pointer=it->second.pointer; chc.size=it->second.size; }
    else{
          std::fstream ifs(s);
           ifs.seekg (0, ifs.end);
           len = ifs.tellg();
           ifs.seekg (0, ifs.beg);
          try { chc.pointer=std::shared_ptr<char>(new char[len]); }catch(std::bad_alloc& b){ ifs.close(); return 2; };
       ifs.read(chc.pointer.get(), len);
      ifs.close();
    chc.size=len;
if(cache_map.insert(std::pair<size_t, cache_t>(sz,chc)).second==false){ return 3; };
   };
  ch.pointer=chc.pointer; ch.size=chc.size;
  return 0;
 };
   

  inline void serv::set_non_block(const int s){

 int fl=1;
if (ioctl(s, FIONBIO, &fl) &&
	    ((fl = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, fl | O_NONBLOCK) < 0)) {close(s); };
      return;
 };

 void serv::sig_hand(int n){ serv::run=0; };
 void serv::sig_pipe(int n){};
 void* serv::th(void* a) { ((serv*)a)->reactor(); return NULL; };
              
 int serv::write_s(conn* c){
   int i; 
           
           if(c->state==SOCK_READ || c->state==KEEP_WAIT){   return 0; };

         if(c->state==SOCK_HEADER){
                 
          do{errno=0;  i=send(c->fd,c->header,c->header_len,0);   

           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
               
               shutdown(c->fd,SHUT_RDWR);  return 0; }; 
                c->state=SOCK_WRITE; i=0; break;
                    }while(1);
                  };
      
         if((c->type)==0){
       do{ errno=0; i=write(c->fd, c->buf_send+c->size_tr, c->buf_size-c->size_tr);
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){  errno=0; continue; };
                  shutdown(c->fd,SHUT_RDWR);  return 0; }; 
                c->size_tr+=i; i=0; break;
                    }while(1);
                 };
    if(c->type==1){ 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, c->buf_size);
           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
                shutdown(c->fd,SHUT_RDWR);  return 0; }; 
                c->size_tr+=i; i=0; break;
                 }while(1);
                };
             
          if(c->size_tr==c->buf_size){ if(c->type==1){ close(c->file_fd); };
             c->size_recv=0; c->state=KEEP_WAIT;c->size_tr=0;i=1; };

              
           
    return i;
      };

 int serv::read_s(conn* c){ 
   int i;
         
      switch(c->state){
       case SOCK_WRITE : i=0; break;

       case KEEP_WAIT : c->state=SOCK_READ;
       

         default :
    do{errno=0; i=read(c->fd, c->buf_recv, 1024); 
        
       if( i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0;  continue; }; 
         c->size_recv=0; i=0; c->state=KEEP_WAIT;  i=1; break; };
            c->size_recv+=i;c->state==SOCK_READ; i=0;
                
             break; }while(1); break;
             };
          return i;
        };
  void serv::proc_queue(){
 conn_it it;
 int sock;
  conn* s;
    mt.lock();
   if(!ques.empty()){
       sock=ques.front();
         ques.pop();
       if((it=conn_map.find(sock))==conn_map.end()){ mt.unlock(); return; };
        s=it->second.get();

    if(s->keep_count==1){
                      shutdown(s->fd,SHUT_RDWR);  }
      else{  s->keep_count++;  ques.push(s->fd);  }; 
           }; 
       mt.unlock();
    };
 
                                 
