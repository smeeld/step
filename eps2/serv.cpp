#include <serv.h>
template <typename T>
 serv<T>::serv(int s, uint8_t m=1, const T& _t=http_handler()): qcount(0) {
  int fd, i, sck=s;
  epv ev;
 
  try{  
   ques.resize(m);
     }catch(std::bad_alloc& c){ return; }
 if((fd=epoll_create(1024))<0)  return;
    auto it=ques.begin();
     it->ev.data.fd=sck;
    it->ev.events=EPOLLIN;
   epoll_ctl(fd,EPOLL_CTL_ADD, s, &it->ev);
   i=0;
  do{
   ques.insert(it, que<T>(_t, fd, sck)); sck=0;
   if((fd=epoll_create(1024))<0)  break;
  ++i; }while(i<m);
 it=ques.begin();
 while(it!=ques.begin()){
   qcount=0;
   it->srv=this;
  pthread_create((*it).pt, NULL, th, (void*)it);
   it++;
    }; 
  }

template <typename T> template <typename U>
 void serv<T>::reactor(serv<T>::que<T>& qs){

 int cur, s, i, sck=qs.sock;
 unsigned int nm=0;
 conn* p;
 que<T>& q=qs;
 epv ev, *pev; 
   while(1)
{  
  
  if(!q.wque.empty()){ 
    
  p=q.wque.front(); q.wque.pop();
   shutdown(p->fd,SHUT_RDWR);
     
   };
  
  cur=epoll_wait(q.efd, q.dev, q.evsize, ((nm>0) ? 0: -1));
    
    for(i=0; i<cur; i++)
     { 
         pev=&q.dev[i]; s=q.dev[i].data.fd; p=q.conn_map[s];
         
      
          if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
              q.conn_map.erase(s);    
           epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
              close(s);
                q.hand.destroy_conn(p);
                 --nm; continue; 
                      };

            if(s==sck){ 
                      
          if((s=accept(sck,(struct sockaddr*)&sockddr,&socklen))<0)  continue;
                          
              set_non_block(s);
                   
                    ev.data.fd=s;
                    ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
                 try{
                  if(epoll_ctl(efd, EPOLL_CTL_ADD,s, &ev)<0) throw 1;
                      p=q.hand.init_conn(); p->fd=s;
                      if(p==NULL) throw 1;
                       std::pair<conn_mp::iterator, bool> it=q.conn_map.insert(std::make_pair(s, p));        
                        if(it.second==false) throw 2; }catch(int s){ 
                            switch(s){
                            case 2 : q.hand.destroy_conn(p); break;
                            default : break;
                            };
                         epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s);
                         }; ++nm;
                        continue;
                        }
               else {
                        if(pev->events & EPOLLIN){
                               if(read_s(p)) continue;
                                   q.hand.handler(p);
                                    };
                 if(pev->events & EPOLLOUT){
                         if(write_s(p)) q.wque.push(p);
                          continue;  
                           };
                        };
                     };
                  };
               };
   
                      

template <typename T>
  inline void serv<T>::set_non_block(const int s){

 int fl=1;
if (ioctl(s, FIONBIO, &fl) &&
	    ((fl = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, fl | O_NONBLOCK) < 0)) close(s); 
 };

template <typename T>          
 int serv<T>::write_s(conn* c){
  size_t i; 
           
           if(c->state==REQ_READ || c->state==REQ_WAIT)  return 0;
      
         if(c->type==REQ_SENDMSG){ i=0;
       do{ errno=0; i=sendmsg(c->fd, &c->msg, 0);
  
           if(i<0){
                 if(errno==EINTR) continue;
                   if(errno!=EAGAIN) shutdown(c->fd,SHUT_RDWR);
                       return 0;
                      }; 
                   c->size_tr+=i; i=0; break;
                    }while(1);
                 };

    if(c->type==REQ_SENDFILE){ 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, c->buf_size);
           if(i<0){ 
                 if(errno==EINTR) continue;
                  if(errno!=EAGAIN) shutdown(c->fd,SHUT_RDWR);
                      return 0;
                    }; 
                   c->size_tr+=i; i=0; break;
                    }while(1);
                 };
             
          if(c->size_tr==c->buf_size){ if(c->type==REQ_SENDFILE) close(c->file_fd);
             if(c->keep){ 
               
              c->size_recv=0; c->state=REQ_WAIT;c->size_tr=0; i=1; }else{ shutdown(c->fd,SHUT_RDWR); i=0; };
                 };
       return i;
      };


template <typename T>
 int serv<T>::read_s(conn* c){ 
   size_t i;
        
      if(c->state==REQ_WRITE) return 1;
       if(c->state==REQ_WAIT) c->state=REQ_READ;
    do{ errno=0; i=recv(c->fd, c->buf_recv, 1024, 0); 
        
       if(i<0 || i==0){ if(errno==EINTR)  continue; 
         if(errno==EAGAIN) return 1;
        shutdown(c->fd, SHUT_RDWR); c->state=REQ_WAIT; return 1;
             };
            c->size_recv+=i; i=0;
                
             break; }while(1);
  
     if(c->size_read){ if(c->size_recv==c->size_read){ i=0; }else{ i=1; };  return i; };
        
         c->state=REQ_HEAD;  
        return i;
        };
/*template <typename T>
inline void serv::pass_hand(const conn* c){

 conn* s=const_cast<conn*>(c);
  uint8_t nm=c->q_nm;
   if(nm==0){ 
   ++qcount; if(qcount==starter) qcount=1; s->q_nm=nm=qcount; };
 que & q=ques[--nm];
 s->hand=1; 
 q.mt.lock();
 q.hque.push(s);
 q.mt.unlock();
 q.cv.notify_one();
  };*/
template <typename T> template <typename U>
 void* serv<T>::th(void* p){ 
   typedef typename std::list<serv<T>::que<T> >::iterator it;
    it t=(it)p;
    t->srv->reactor(*t); return NULL; };

 /* int serv::reactor2(uint8_t c){
  
   que& q=ques[c];
   conn_mp& cmp=q.conn_mp;
    conn* cs=NULL;
     uint8_t st=1, ev;
      int s=0, i, cur;
   dev=(ev*)malloc(sizeof(ev)*1024);
   if(dev) st=0;
  while(st){
  
   do{
    std::unique_lock<std::mutex> lck(q.mt);
     if(q.rque.empty()) break;
      s=q.rque.front(); q.rque.pop(); break; }while(1);
      if(s){
        ev.data.fd=s;
         ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;        
       if(epoll_ctl(efd,EPOLL_CTL_ADD,s ,&ev)<0){ close(s); continue; };
                   
              q.
               epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s); };   
              };
    cur=epoll_wait(q.epfd, dev, 1024, -1);
     for(i=0; i<cur, i++){
       pev=&dev[i]; s=dev[i].data.fd; p=cmp[s];
         if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
                std::unique_ptr<conn> ptr(p);
              cmp.erase(s);
               
           epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
              close(s);
                  continue; 
            };                       
       if(pev->events & EPOLLIN){
              if(read_s(p)) continue;
                    q.handler(p);
               if(write_s(p)){ ++nm; q.tque.push(p); };
                  continue
                 };
      if(pev->events & EPOLLOUT){
       if(write_s(p)){ ++nm; q.tque.push(p); };
           continue;
            };  
          };
        };
      };
*/
void http_handler::send_header(http_conn* c){
  
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

inline  int http_handlep::req_gen (http_conn* c){

  http_conn* sp=c;
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
void http_handlep::handler(conn* s){
    struct stat st;
     size_t sz;
     int error;
     uint8_t i;
     cache_t ch;
    http_conn* cs=static_cast<http_conn*>(s);
    if(req_gen(cs)){ error_hand(cs, 1); return; };
   const char *tmp=cs->request.path;
          
     if(cs->request.method==HTTP_GET){
      try{
          errno=0;
        i=stat(tmp, &st); if(i<0 || errno==ENOENT){  throw 1; };
          
       if(st.st_size>100000){ cs->type=REQ_SENDFILE;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0){ throw 1; };
              }
               else{ cs->type=0;
         if(cacher(tmp, ch)){ cs->type=REQ_SENDMSG;
                              cs->buf_size=st.st_size;
         if((cs->file_fd=open(tmp, O_RDONLY))==0){ throw 1; };
             
            }else{
               cs->buf_send=ch.pointer.get(); cs->buf_size=ch.size;
                 };
               };
           
         }catch(int er){  error_hand(cs, er); return; };
           
            send_header(cs);          
           struct iovec* pvc=cs->msg.msg_iov;   
           cs->msg.msg_iovlen=2;
           pvc->iov_base=&cs->header;
           pvc->iov_len=cs->header_len;
           pvc++;
           pvc->iov_base=&cs->buf_send;
           pvc->iov_len=cs->buf_size;
           cs->state=REQ_WRITE;
          };
       };
  
int http_handler::cacher(const char* s, cache_t& ch){
 
 cache_t chc;
 char* p;
 int len;
 
 key_mp key(s);

  
  if(( auto it=cache_map.find(key))!=cache_map.end()){ 
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
         switch(error){ case 1 :  hd::err_s1.copy(cs->header, hd::err_s1.size()); 
                           cs->buf_size=hd::err_s1.size(); cs->buf_send=cs->header;
                             cs->state=REQ_WRITE; break;
                        default :  break;
                      };
                 cs->request.connection_keep=0;
            struct iovec* pvc=cs->msg.msg_iov;   
           
           cs->msg.msg_iovlen=1;
           pvc->iov_base=&cs->header;
           pvc->iov_len=cs->header_len;
          
          };

void http_handler::destroy_conn(conn* p){
  http_conn* s=static_cast<http_conn*>(p);
   delete s;
 };

conn* http_handler::init_conn(){
  http_conn* p=new http_conn();
   return static_cast<conn*>(p);
  };
  
  
 
