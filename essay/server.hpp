#include <new>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>
#include <cstdlib>
template <typename T>
struct server
 {

  typedef typename T::buffer buffer;
  T handler;
  server(int s): socket(s), hd(NULL), efd(0){};
  int socket;
  typedef struct epoll_event ev_t;
   ev_t ev[1024];
   int efd;

  struct client: buffer
  {
    client(int s):sck(s){};
   ~client()
     { 
      close(sck);
      prv->nxt=nxt;
      nxt->prv=prv;
    };
    int sck;
    int cnt;
    client* nxt;
    client* prv;
  };

 client* hd;

  void run()
  {
   struct sockaddr_in sockd;
   int cnt;
   ev[0].data.ptr=0;
   ev[0].events=EPOLLIN;
    if((efd=epoll_create(1024))<0) return;
    if(epoll_ctl(efd, EPOLL_CTL_ADD, socket, &ev[0])<0) return;
  while(1)
  {
   
    cnt=epoll_wait(efd, ev, 1024, -1);
    for(int i=0; i<cnt; i++)
     {
      client* p=static_cast<client*>(ev[i].data.ptr);
      if(p==0)
       {
        if(ev[i].events& EPOLLERR) return;
        accept_c();
        continue;
       };
     if(ev[i].events & EPOLLERR || ev[i].events & EPOLLHUP || ev[i].events & EPOLLRDHUP){ delete_c(p); continue; };
     if(ev[i].events & EPOLLIN)
      {
       int sz;
       if((sz=read_c(p))>0)
         if(handler.process(static_cast<buffer&>(*p))<0) delete_c(p);
       continue;
      };
 
     };
    
    };
  };    
void accept_c()
  {    
   ev_t tmp; int i, s;
  struct sockaddr_in sockd;
  socklen_t sken=sizeof(struct sockaddr_in);
  if((s=accept(socket,(struct sockaddr*)&sockd, &sken))<0) return;         
  if(ioctl(s, FIONBIO, &i) &&
	    ((i = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, i | O_NONBLOCK) < 0)) {close(s); return; };
  try{
   client* cp= new client(s);
     tmp.data.ptr=static_cast<void*>(cp);
     tmp.events=EPOLLET | EPOLLIN |  EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
     if(epoll_ctl(efd, EPOLL_CTL_ADD, s, &tmp)<0)
     { 
       close(s);  delete cp;
     }
      else
       {
        if(hd==NULL){ hd=cp; cp->nxt=cp; cp->prv=cp; }
         else{ cp->nxt=hd; hd->prv->nxt=cp; cp->prv=hd->prv; hd->prv=cp;  hd=cp; };
       };
      }catch (std::bad_alloc& s){};
   };

  int read_c(client* p)
  { 
   do{ errno=0;
      size_t sz=read(p->sck, p->pointer(), p->max_size());
      
   if(sz>0) return sz;
   switch(errno)
   {
    case EAGAIN: return 0;
    case EINTR: continue;
   };
  }while(1);
   delete_c(p);
   return -1;
  };

void delete_c(client* p)
 { 
   epoll_ctl(efd, EPOLL_CTL_DEL, p->sck, NULL);
   delete p;
  };
~server()
  { 
    client* tmp;
     if(hd!=NULL) for(tmp=hd->nxt; tmp!=hd; tmp=tmp->nxt) delete_c(tmp);
      delete(hd);
     close(socket);
     if(efd) close(efd);
  };
    
}; 

