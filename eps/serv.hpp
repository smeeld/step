#include <base.h>
#include <sched.h>
template <typename T>
class serv{
typedef std::unordered_map<int, conn* > conn_mp;

struct que{

 que(const T& _t, int s, int sck, serv<T>* sr, uint8_t nm)
  : efd(s), hand(_t), srv(sr), evsize(1024), number(nm){
              ptr.fd=sck; ptr.next=&ptr; ptr.prev=&ptr; };
 que(const que&)=delete;
 ~que(){int s; conn_ptr* tm=ptr.next, *tmp, *hd=&ptr;
   while(tm!=hd){ 
    s=tm->fd;
     epoll_ctl(efd, EPOLL_CTL_DEL, s, NULL);
     close(s);
    tmp=tm->next;
    delete tm; tm=tmp; 
      };
  if(efd)
    close(efd);
  while(cache_count){ delete cache_ptr[--cache_count]; };
  
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
uint8_t number;

  };
public:
serv(int s=0, int m=0, T const & _t=T()): qcount(0){
  int fd, i=0, sck=s;
  epv* pev; que* p;
 if(m==0) m=(int) sysconf(_SC_NPROCESSORS_ONLN);
 qlist=(que**)malloc(sizeof(que*)*m);
  if(qlist==NULL) return;
do{

  try{ 
   p=new que(_t, 0, sck, this, qcount);
      }catch(std::bad_alloc& c){ close(fd); break; };
   qlist[qcount++]=p;
   pthread_create(&p->pt, NULL, th, (void*)p);
    
     }while(qcount<m);

  };



serv(const serv&)=delete;

 ~serv(){ que* p; while(qcount){
         p=qlist[--qcount]; 
         if(p){ pthread_cancel(p->pt); delete p; };
        };
     };

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

 void list_in(conn_ptr* prv, conn_ptr* nxt){
  prv->next->prev=nxt;
  nxt->next=prv->next;
  prv->next=nxt;
  nxt->prev=prv;
 };

void list_del(conn_ptr* p){
 p->next->prev=p->prev;
 p->prev->next=p->next;
 };  
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
             
            size_t cnt=c->msg.msg_iovlen;
           if(cnt==0 && c->buf_size==0){ c->state &= ~REQ_WRITE;  return 0; };
         if(cnt){ i=0;
        errno=0; i=sendmsg(c->fd, &c->msg, 0);
         do{
           if(i<0){
                 if(errno==EINTR) continue;
                   if(errno!=EAGAIN) shutdown(c->fd, SHUT_RDWR);
                       return 0;
                      };
                    
                   c->size_tr+=i;
                   cnt=c->msg_check(c->size_tr);
                   if(cnt) return 0;
                   if(c->type==REQ_SENDFILE)  c->size_tr=0;
                    i=1; c->hand=REQ_WRITE; break;
                     }while(1);
                   };

    if(c->type==REQ_SENDFILE){
        size_t bsz=c->buf_size;
        i=c->size_tr;
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, bsz-i);
           if(i<0){ 
                 if(errno==EINTR) continue;
                  if(errno!=EAGAIN) shutdown(c->fd,SHUT_RDWR);
                      return 0;
                    }; 
                   c->size_tr+=i; i=0;
                    if(c->size_tr==bsz){ c->hand=REQ_WRITE; i=1; };
                    break;
                    }while(1);
                 };
             
          if(i){ if(c->type==REQ_SENDFILE) close(c->file_fd); };
               
       return i;
      };


template <typename T>
 int serv<T>::read_s(conn* c){ 
   size_t i;
        if(c->state & REQ_SHUT) return 0;
    do{ errno=0; i=recv(c->fd, c->buf_recv, 1024, 0); 
        
       if(i<0 || i==0){ if(errno==EINTR)  continue; 
         if(errno==EAGAIN) return 0;
        shutdown(c->fd, SHUT_RDWR); c->state=REQ_SHUT; return 0;
             };
            c->size_recv+=i;
            c->hand=REQ_READ;
                i=1;
             break; }while(1);
  
     if(c->size_read){ if(c->size_recv!=c->size_read); i=0; };
        
        return i;
        };

template <typename T>
void serv<T>::reactor(serv<T>::que& qs){
 que& q=qs;
 cpu_set_t cp;
 CPU_ZERO(&cp);
 CPU_SET(q.number, &cp);
 sched_setaffinity(0, sizeof(cpu_set_t), &cp);
 unsigned int nm=0, wnm=0, cnt;
  conn_ptr* gptr=&q.ptr, *pnt;
 conn* p;
 epv ev, *pev, *bev=q.dev;
 int cur, s, i, sck=gptr->fd, efd, sz=q.evsize;
 time_t tm;

    ev.data.ptr=gptr;
    ev.events=EPOLLIN;
 
  do{
   if((efd=epoll_create(1024))>0){ 
  if( epoll_ctl(efd, EPOLL_CTL_ADD, sck, &ev)==0) break; };
      q.efd=0;
      que** p=qlist+q.number;
        delete *p; *p=NULL;
        pthread_exit(0);
    }while(1);
 q.efd=efd;
   while(1)
{  
 cnt=wnm; pnt=gptr->prev;
 while(cnt>0){
 p=static_cast<conn*>(pnt);
 time(&tm);
  if(p->state & REQ_WAIT){
     if((tm-p->tm)>p->keep_count){  --wnm; shutdown(p->fd, SHUT_RDWR); };
       };
    pnt=pnt->prev; --cnt;
   }
  cur=epoll_wait( efd, bev, sz, (wnm>0) ? 0: -1);
    
    for(i=0; i<cur; i++)
     { 
         pev=bev+i; pnt=static_cast<conn_ptr*>(pev->data.ptr);
         s=pnt->fd;
          if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
              if(s==sck) continue; 
             p=static_cast<conn*>(pnt);  
             list_del(static_cast<conn_ptr*>(p));
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
                    if(q.cache_count==0)
                      p=q.hand.init_conn();
                    else p=q.cache_ptr[--q.cache_count];
                      if(p==NULL) throw 1; p->fd=s;
                      pnt=static_cast<conn_ptr*>(p);
                      ev.data.ptr=pnt;
                      ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
                      if(epoll_ctl(efd, EPOLL_CTL_ADD,s, &ev)<0) throw 2;
                      list_in(gptr, pnt);
                           }catch(int s){ 
                            switch(s){
                            case 2 : q.hand.destroy_conn(p); break;
                            default : break;
                            };
                        
                         };  ++nm;
                  q.hand.handler(p);
                        continue;
                        }
                 p=static_cast<conn*>(pnt);
                      
                        if((pev->events & EPOLLIN) && (p->state & REQ_READ)){
                           if(read_s(p)) q.hand.handler(p);
                            };
                 if((pev->events & EPOLLOUT) && (p->state & REQ_WRITE)){
                           if(write_s(p)){ 
                            q.hand.handler(p);
                            if(p->state & REQ_SHUT){ shutdown(p->fd, SHUT_RDWR); continue; }
                              if(p->state & REQ_WAIT){
                               time(&p->tm);
                               list_del(static_cast<conn_ptr*>(p));
                               list_in(gptr->prev, static_cast<conn_ptr*>(p));
                               ++wnm; continue;
                              };   
                             };
                           };
                         };
                       };
                     };

template <typename T> 
 void* serv<T>::th(void* p){ 
   
    que* ptr=(que*)p;
    ptr->srv->reactor(*ptr); return NULL; };


