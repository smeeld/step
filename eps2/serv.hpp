#include <base.h>

template <typename T>
class serv{
typedef std::unordered_map<int, conn* > conn_mp;

struct que{

 que(const T& _t, int s, int sck, serv<T>* sr)
  : efd(s), hand(_t), srv(sr), evsize(1024){
              ptr.fd=sck; ptr.next=NULL; ptr.prev=NULL; };
 que(const que&)=delete;
 ~que(){int s; conn_ptr* tm=ptr.next, *tmp;
   while(tm){ 
    s=tm->fd;
    epoll_ctl(efd, EPOLL_CTL_DEL, s, NULL);
     close(s);
    tmp=tm->next;
    delete tm; tm=tmp; 
      };
    close(efd);
  while(cache_count){ delete cache_ptr[--cache_count]; };
  pthread_cancel(pt);
  };

 conn_mp conn_map;
 std::queue<conn*> rque;
 std::queue<conn*> wque;
 std::mutex mt;
 std::condition_variable cv;
 pthread_t pt;
T hand;
epv ev, dev[1024];
int  efd, evsize;
serv<T>* srv;
conn_ptr ptr;
conn* cache_ptr[1024];
size_t cache_count;
};
public:
serv(int s=0, int m=8, T const & _t=T()): qcount(0){
  int fd, i=0, sck=s;
  epv* pev; que* p;
 qlist=(que**)malloc(sizeof(que*)*m);
  if(qlist==NULL) return;
do{
 if((fd=epoll_create(1024))<0) break;
  try{ 
   p=new que(_t, fd, sck, this);
      }catch(std::bad_alloc& c){ close(fd); break; };
   pev=&p->ev;
    pev->data.ptr=&p->ptr;
    pev->events=EPOLLIN;
   epoll_ctl(fd, EPOLL_CTL_ADD, sck, pev);
   qlist[qcount++]=p;
   pthread_create(&p->pt, NULL, th, (void*)p);
    
     }while(qcount<m);

  };



serv(const serv&)=delete;

 ~serv(){ while(qcount){ delete qlist[--qcount]; }; };

private:
int sock;
 int efd;
 pthread_t ptr;
 uint8_t qcount;
 void reactor(que&);

 /*void pass_hand(const conn*, uint8_t);*/
 int read_s(conn*);
 int write_s(conn*);
  void set_non_block(const int);
static void* th(void*);
 que** qlist;
 std::mutex mtc;
 struct sockaddr_in sockddr;
 socklen_t socklen;
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
            
            size_t cnt=c->msg.msg_iovlen;
         if(cnt){ i=0;
        errno=0; i=sendmsg(c->fd, &c->msg, 0);
         do{
           if(i<0){
                 if(errno==EINTR) continue;
                   if(errno!=EAGAIN) shutdown(c->fd,SHUT_RDWR);
                       return 0;
                      };
                    
                   c->size_tr+=i;
                   cnt=c->msg_check(c->size_tr);
                   if(cnt) return 0;
                   if(c->type==REQ_SENDFILE)  c->size_tr=0;
                    i=1; break;
                     }while(1);
                   };

    if(c->type==REQ_SENDFILE){
        size_t bsz=c->buf_size; 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, bsz);
           if(i<0){ 
                 if(errno==EINTR) continue;
                  if(errno!=EAGAIN) shutdown(c->fd,SHUT_RDWR);
                      return 0;
                    }; 
                   c->size_tr+=i; i=0;
                    if(c->size_tr==bsz) i=1;
                    break;
                    }while(1);
                 };
             
          if(i){ if(c->type==REQ_SENDFILE) close(c->file_fd);shutdown(c->fd,SHUT_RDWR);/*
             if(c->keep){ 
               
              c->size_recv=0; c->state=REQ_WAIT;c->size_tr=0; i=1; }else{  shutdown(c->fd,SHUT_RDWR); i=0; };*/i=0;
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

template <typename T>
void serv<T>::reactor(serv<T>::que& qs){
 que& q=qs;
 unsigned int nm=0;
  conn_ptr* gptr=&q.ptr, *pnt;
 conn* p;
 epv ev, *pev, *bev=q.dev;
 int cur, s, i, sck=gptr->fd, efd=q.efd, sz=q.evsize;
   while(1)
{  
  
  if(!q.wque.empty()){ 
    
  p=q.wque.front(); q.wque.pop();
   shutdown(p->fd, SHUT_RDWR);
     
   };
  
  cur=epoll_wait( efd, bev, sz, (nm>0) ? 0: -1);
    
    for(i=0; i<cur; i++)
     { 
         pev=bev+i; pnt=static_cast<conn_ptr*>(pev->data.ptr);
         s=pnt->fd;
          if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
              if(s==sck) continue; 
             p=static_cast<conn*>(pnt);  
           if(p->prev) p->prev->next=p->next;
            if( p->next) p->next->prev=p->prev;
              epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
               if(q.cache_count==1024){
                q.hand.destroy_conn(p); 
                       }else{
                          p->~conn();  p=q.hand.init_conn(p); q.cache_ptr[q.cache_count++]=p; };
                  close(s);
                 --nm; continue; 
                      };

            if(s==sck){ 
                   
          if((s=accept(sck,(struct sockaddr*)&sockddr,&socklen))<0) continue;
                          
              set_non_block(s);
                   
                 try{
                    if(q.cache_count==0){
                      p=q.hand.init_conn(); }else{ p=q.cache_ptr[--q.cache_count]; };
                      if(p==NULL) throw 1; p->fd=s;
                      pnt=static_cast<conn_ptr*>(p);
                      ev.data.ptr=static_cast<void*>(pnt);
                      ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
                      if(epoll_ctl(efd, EPOLL_CTL_ADD,s, &ev)<0) throw 2;
                     pnt->next=gptr->next; pnt->prev=gptr; gptr->next=pnt;
                      if(pnt->next) pnt->next->prev=pnt;
                           }catch(int s){ 
                            switch(s){
                            case 2 : q.hand.destroy_conn(p); break;
                            default : break;
                            };
                        
                         };  ++nm;
                        continue;
                        }
               else {  p=static_cast<conn*>(pnt);
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
 void* serv<T>::th(void* p){ 
   
    que* ptr=(que*)p;
    ptr->srv->reactor(*ptr); return NULL; };

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
