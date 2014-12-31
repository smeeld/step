#include <base.h>
#include <sched.h>
#define RES_CONTINUE 0
#define RES_HAND 1
#define RES_WAIT 2
#define RES_SHUT 3

template <typename T>
class serv{
typedef std::unordered_map<int, conn* > conn_mp;

struct que{

 que(const T& _t, int sck, serv<T>* sr, uint8_t nm)
  : sock(sck), hand(_t), srv(sr), evsize(1024), number(nm), worked(0)
        {  work_que.next=std::addressof(work_que);  work_que.prev=std::addressof(work_que);
            wait_que.next=std::addressof(wait_que); wait_que.prev=std::addressof(wait_que); 
        };
 que(const que&)=delete;
 ~que(){
  int s; 
  conn_ptr* hd=&work_que, *tmp=hd->next, *tm, *end;
  while(tmp!=hd)
   {
   do{ 
     tm=tmp;
     tmp=tmp->next;
     s=static_cast<conn*>(tm)->fd;
     epoll_ctl(efd, EPOLL_CTL_DEL, s, NULL);
     close(s);
     hand.destroy_conn(static_cast<conn*>(tm));
     }while(tmp!=hd);
    hd=&wait_que; tmp=hd->next;
    };
  epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
    close(efd);
  };

conn_ptr work_que;
conn_ptr wait_que;
 conn_mp conn_map;
 std::mutex mt;
 std::condition_variable cv;
 pthread_t pt;
T hand;
epv dev[1024];
int  efd, sock, evsize;
serv<T>* srv;
conn_ptr ptr;
conn* cache_ptr[1024];
size_t cache_count;
size_t worked;
uint8_t number;
};

public:
serv(int s=0, int m=0, T const & _t=T()): sock(s), qcount(0){
  epv* pev; que* p;
 if(m==0) m=(int) sysconf(_SC_NPROCESSORS_ONLN);
 qlist=(que**)malloc(sizeof(que*)*m);
 if(qlist==NULL) return;
 do{
 try{ 
   p=new que(_t, s, this, qcount);
    }catch(std::bad_alloc& c){ break; };
   qlist[qcount++]=p;
   pthread_create(&p->pt, NULL, th, (void*)p);
   }while(qcount<m);
  };

serv(const serv&)=delete;
 ~serv(){
   que* p;
 while(qcount)
 {
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
 int read_s(conn*);
 int write_s(conn*);
static void* th(void*);

int set_non_block(const int s){
 int fl=1;
 if (ioctl(s, FIONBIO, &fl) &&
	    ((fl = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, fl | O_NONBLOCK) < 0)) return -1;
 return 0; 
 };

 que** qlist;
 std::mutex mtc;
 struct sockaddr_in sockddr;
 socklen_t socklen;

void queue_in(conn_ptr* prv, conn_ptr* nxt){
  prv->next->prev=nxt;
  nxt->next=prv->next;
  prv->next=nxt;
  nxt->prev=prv;
 };
void queue_del(conn_ptr* p){
 p->next->prev=p->prev;
 p->prev->next=p->next;
 };
void queue_switch(conn* p, conn_ptr* queue){
  queue_del(static_cast<conn_ptr*>(p));
  queue_in(queue, static_cast<conn_ptr*>(p));
   };
void destroy(conn* p, serv<T>::que& q)
 {
  if(q.cache_count<1024)
   { 
   q.hand.clear_conn(p); q.cache_ptr[q.cache_count++]=p;
   }
  else q.hand.destroy_conn(p);
 };

void delete_conn(conn* p, que& q){
  conn_ptr* ptr=static_cast<conn_ptr*>(p);  
   queue_del(ptr);
   if((p->event & REQ_WORKED)!=0) --q.worked;
   epoll_ctl(q.efd, EPOLL_CTL_DEL, p->fd, NULL);
   close(p->fd); 
   destroy(p, q);
   };

int wait_proc(que& q){
 time_t tm;
 conn* p;
  int min=-1;
 conn_ptr* hd=std::addressof(q.wait_que), *tmp=hd->prev;
 p=static_cast<conn*>(tmp);
 time(&tm);
 while(tmp!=hd)
  { 
  min=tm-p->tm;
  if(min < p->keep_count) break;
   tmp=tmp->prev;
   delete_conn(p, q);
   p=static_cast<conn*>(tmp); 
   }; 
 return min;
  };

void wait_conn(conn* p, que& q, int cnt=0)
 {
  time_t tm;
  time(&tm);
  p->tm=tm;
  if(cnt!=0) p->keep_count=cnt;
  queue_switch(p, std::addressof(q.wait_que));
  p->event &= ~REQ_WORKED;
  if(q.worked>0) --q.worked;
 };

void work_conn(conn* p, que& q)
 {
  queue_switch(p, std::addressof(q.work_que));
   p->event |= REQ_WORKED;
  ++q.worked;
 };

void accept_conn(int sck, que& q)
 {
  int s;
  conn* p;
  conn_ptr* ptr;
  struct epoll_event ev;
  if((s=accept(sck,(struct sockaddr*)&sockddr,&socklen))<0) return;         
  if(set_non_block(s)<0){ close(s); return; };
  if(q.cache_count==0) p=q.hand.create_conn();
    else
    {
     p=q.cache_ptr[--q.cache_count];
     p=q.hand.create_conn(p);
    };
   if(p==nullptr){ close(s); return; };
    p->fd=s;
    ptr=static_cast<conn_ptr*>(p);
     ev.data.ptr=ptr;
     ev.events=EPOLLET | EPOLLIN |  EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
     if(epoll_ctl(q.efd, EPOLL_CTL_ADD, s, &ev)<0){ close(s); destroy(p, q); return; };
     p->event |= REQ_WRITE;
     if(p->state & REQ_WRITE){ 
     p->event |=  REQ_WORKED;
     queue_in(std::addressof(q.work_que), static_cast<conn_ptr*>(p));
     ++q.worked;
     }
    else
    {
     time_t tm;
     time(&tm);
     p->tm=tm;
     p->keep_count=100;
     queue_in(std::addressof(q.wait_que), static_cast<conn_ptr*>(p));
    } 
  };

 void work_proc(que& q)
  {
   conn_ptr* hd=std::addressof(q.work_que), *tmp=hd->next;
   conn* p;
   int s;
   while(tmp!=hd)
    {
     p=static_cast<conn*>(tmp);
     tmp=tmp->next;
     if((p->event & REQ_READ) && (p->state & REQ_READ))
      {
       switch(read_s(p))
       {
         case RES_CONTINUE : break;
         case RES_HAND : p->state &= ~REQ_READ; p->hand=REQ_READ; q.hand.handler(p); break;
         case RES_WAIT : p->event &= ~REQ_READ;
              if(((p->event & REQ_WRITE)==0) || ((p->state & REQ_WRITE)==0)) wait_conn(p, q, 10); continue;
         case RES_SHUT : delete_conn(p, q); break;
         default : break;
       };
      };

     if((p->event & REQ_WRITE) && (p->state & REQ_WRITE))
      {
       switch(write_s(p))
       { 
         case RES_CONTINUE :  continue;
         case RES_HAND :  p->state &= ~REQ_WRITE; p->hand=REQ_WRITE; q.hand.handler(p); break;
         case RES_WAIT :  p->event &= ~REQ_WRITE;
                if(((p->event & REQ_READ)==0) || ((p->state & REQ_READ)==0)) wait_conn(p, q, 10); continue;
         case RES_SHUT : delete_conn(p, q); continue; 
         default : break;
       };
      };
     if(p->state & REQ_WAIT){ p->event=0; p->state &=~REQ_WAIT; wait_conn(p, q); continue; };
     if(p->state & REQ_SHUT){  delete_conn(p, q); continue; }; 
     if(p->event==0) wait_conn(p, q, 10);
     };
    };
  };

template <typename T>
 int serv<T>::write_s(conn* c){
   long i; 
   size_t cnt=c->msg.msg_iovlen;
     if(cnt)
      {
       do{
        errno=0; i=sendmsg(c->fd, &c->msg, 0);
       }while(errno==EINTR);
       if(i<0)
        {
          if(errno==EAGAIN) i=RES_WAIT;
           else i=RES_SHUT;
           return i;
        };
         c->size_tr+=i;
         cnt=c->msg_check(c->size_tr);
         if(cnt) return RES_WAIT;
         if(c->type==REQ_SENDFILE) c->size_tr=0;
         i=RES_HAND; c->hand=REQ_WRITE;
       };

    if(c->type==REQ_SENDFILE)
      {
        size_t bsz=c->buf_size;
        if(c->file_fd==0) return 3;
        do{errno=0; i=c->size_tr;
           i=sendfile(c->fd, c->file_fd, 0, bsz-i);
          }while(errno==EINTR);
         if(i<0)
          {
           if(errno==EAGAIN) i=RES_WAIT;
              else i=RES_SHUT;
             return i;
           };
         c->size_tr+=i;
         if(c->size_tr==bsz){ c->hand=REQ_WRITE; i=RES_HAND; }
          else i=RES_WAIT;
        };
       return i;
      };


template <typename T>
 int serv<T>::read_s(conn* c){ 
   long i;
   size_t tmp=c->size_rd;
    do{ errno=0;
       if(tmp==0) i=c->current_rsize;
        else i=tmp-c->size_recv;
       i=recv(c->fd, c->buf_recv, i, 0); 
      }while(errno==EINTR); 
     if(i<0 || i==0)
      {
       if(errno==EAGAIN) i=RES_WAIT;
        else i=RES_SHUT;
        return i;
       };
     if((c->size_recv+=i) < tmp) i=RES_WAIT; 
       else{ c->hand=REQ_READ; i=RES_HAND; };
      return i;
    };

template <typename T>
void serv<T>::reactor(serv<T>::que& qs){
 que& q=qs;
 cpu_set_t cp;
 CPU_ZERO(&cp);
 CPU_SET(q.number, &cp);
 sched_setaffinity(0, sizeof(cpu_set_t), &cp);
 unsigned int nm=0;
 conn_ptr* pnt;
 conn* p;
 epv *bev=q.dev, *pev;
 int s, i, sck=q.sock, efd;
 size_t sz=q.evsize, cur;
 bev[0].data.ptr=0;
 bev[0].events=EPOLLIN;
 do{
   if((efd=epoll_create(1024))>0){ 
  if( epoll_ctl(efd, EPOLL_CTL_ADD, sck, bev)==0) break; };
      q.efd=0;
      que** p=qlist+q.number;
        delete *p; *p=NULL;
        pthread_exit(0);
    }while(1);
   q.efd=efd;
   while(1)
   {  
    s=wait_proc(q);
    nm=q.worked;
    if(nm>0) s=0;
    cur=epoll_wait( efd, bev, sz, s);

    for(i=0; i<cur; i++)
     { 
       pev=bev+i;
       pnt=static_cast<conn_ptr*>(pev->data.ptr);
       if(pnt){ p=static_cast<conn*>(pnt);
        s=p->fd; }
         else s=sck;
       if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) || (pev->events & EPOLLRDHUP))
	    { 
         if(s==sck) this->~serv(); 
         delete_conn(p, q);
          continue; 
         };
        
      if(s==sck){ accept_conn(sck, q); continue;};
        s=((pev->events & EPOLLIN) | (pev->events & EPOLLOUT));
        if(s!=0)
         {
          p->event |= s;
          if((p->event & REQ_WORKED)==0) work_conn(p, q);
         };
       };
      work_proc(q);
      };
    };

template <typename T> 
 void* serv<T>::th(void* p){ 
   
    que* ptr=(que*)p;
    ptr->srv->reactor(*ptr); return NULL; };


