#include <base.h>

template <typename T>
class serv{
typedef std::unordered_map<int, conn* > conn_mp;

struct que{

 que(const T& _t, int s, int sck, serv<T>* sr)
  : efd(s), hand(_t), srv(sr), evsize(1024){
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
             
            size_t cnt=c->msg.msg_iovlen;
           if(cnt==0 || c->buf_size==0){ c->state &= ~REQ_WRITE; std::cout<<"Error Write\n"; return 0; };
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
                i=1;
             break; }while(1);
  
     if(c->size_read){ if(c->size_recv!=c->size_read); i=0; };
        
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
  pnt=gptr->next;
  do{
  
 p=static_cast<conn*>(pnt);
 if( p->state){
 
  std::cout<<"Cocle for "<<p->fd<< " sttse =="<<(int)p->state<<"\n";
  if(p->state & REQ_READ){std::cout<<"For Read\n";
     if(read_s(p)) q.hand.handler(p); };
  if(p->state & REQ_WRITE){std::cout<<"For Write\n";
     if(write_s(p)) q.hand.handler(p); };
  if(p->state & REQ_WAIT){ if(p->keep){ --p->keep;std::cout<<"SHiting wait\n";}
                     else shutdown(p->fd, SHUT_RDWR); };
  };
 pnt=pnt->next;
}while(pnt!=gptr);
  cur=epoll_wait( efd, bev, sz, (nm>0) ? 0: -1);
    
    for(i=0; i<cur; i++)
     { 
         pev=bev+i; pnt=static_cast<conn_ptr*>(pev->data.ptr);
         s=pnt->fd;
          if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
              if(s==sck) continue;
             p=static_cast<conn*>(pnt);  
            p->prev->next=p->next;
            p->next->prev=p->prev;
              epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
               if(q.cache_count==1024){
                q.hand.destroy_conn(p); 
                       }else{
                          p->~conn();  p=q.hand.init_conn(p); q.cache_ptr[q.cache_count++]=p; };
                  close(s);std::cout<<"SHitdown\n";
                 --nm; continue; 
                      };

            if(s==sck){ 
                   
          if((s=accept(sck,(struct sockaddr*)&sockddr,&socklen))<0) continue;
                          
              set_non_block(s);
                   std::cout<<"New Conn For=="<<s<<"\n";
                 try{
                    if(q.cache_count==0)
                      p=q.hand.init_conn();
                    else p=q.cache_ptr[--q.cache_count];
                      if(p==NULL) throw 1; p->fd=s;
                      pnt=static_cast<conn_ptr*>(p);
                      ev.data.ptr=pnt;
                      ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
                      if(epoll_ctl(efd, EPOLL_CTL_ADD,s, &ev)<0) throw 2;
                     pnt->next=gptr->next; pnt->prev=gptr; gptr->next=pnt;
                       pnt->next->prev=pnt;
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
                             p->state |= REQ_READ;std::cout<<"New Read\n";
                                   continue;
                                    };
                 if(pev->events & EPOLLOUT){
                             p->state |= REQ_WRITE;std::cout<<"New Write\n";
                                  
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

template <typename _T>
struct Serv:  serv<_T>{
Serv(int s):serv<_T>(s){};
~Serv(){};
int a; };

