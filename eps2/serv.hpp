#include <base.h>

template <typename T>
class serv{
typedef std::unordered_map<int, conn* > conn_mp;
/*template <typename U>*/
struct que{

 que(const T& _t, int s, int sck, serv<T>* sr)
  : efd(s), sock(sck), hand(_t), srv(sr), evsize(1024){};
 que(const que&)=delete;
 ~que(){int s;
  auto it=conn_map.begin();
   while(it!=conn_map.end()){
    s=it->first;
    epoll_ctl(efd, EPOLL_CTL_DEL, s, NULL);
     close(s);
    delete it->second; it++;
      };
    close(efd);
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
int sock, efd, evsize;
serv<T>* srv;
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
    pev->data.fd=sck;
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
       do{ errno=0; i=sendmsg(c->fd, &c->msg, 0);
  
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
             
          if(i){ if(c->type==REQ_SENDFILE) close(c->file_fd);
             if(c->keep){ 
               
              c->size_recv=0; c->state=REQ_WAIT;c->size_tr=0; i=1; }else{  shutdown(c->fd,SHUT_RDWR); i=0; };
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
 
 unsigned int nm=0;
 conn* p;
 que& q=qs;
 epv ev, *pev, *bev=q.dev;
 int cur, s, i, sck=q.sock, efd=q.efd; 
   while(1)
{  
  
  if(!q.wque.empty()){ 
    
  p=q.wque.front(); q.wque.pop();
   shutdown(p->fd, SHUT_RDWR);
     
   };
  
  cur=epoll_wait(q.efd, q.dev, q.evsize, (nm>0) ? 0: -1);
    
    for(i=0; i<cur; i++)
     { 
         pev=bev+i; s=bev[i].data.fd; p=q.conn_map[s];
         
      
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
