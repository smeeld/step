#include <serv.h>
struct headers{
 
const char* s0="HTTP/1.1 200 OK\r\n";
const char* s1="Date: xxxxxxxxxxxxxxxxxxxxxxxx GMT\r\n";
const char* s2="Server: MATRIX\r\n";
const char* s3="Content-Type: %s; charset=utf-8\r\n";
const char* s4="Content-Length: %d \r\n";
const char* s5="Connection: keep-alive\r\n\r\n";
const char* s6="Connection: close\r\n\r\n";
const char* s7="HTTP/1.1";
const char* s8="Server: MATRIX\r\n";
const char* s9="Content-Type: ";
const char* s10="Content-Length: ";
const char* s11="Connection: ";
const char* s12="keep-alive";
const char* s13="Keep-Alive";
const char* s14="GET";
const char* s15="PUT";
const char* s16="Host";
const char* s17="Date: ";
const char* s18="; charset=utf-8\r\n";
const char* s19=" GMT\r\n";
const char* s20="text/html";
const char* s21="text/javascript";
const char* s22="image/jpeg";
const char* s23="image/gif";
const char* err_s1="HTTP/1.1 404 Not Found\r\n";
const char* err_s2="HTTP/1.1 500 Internal Server Error\r\n";
const char* err_s3="HTTP/1.1 400 Bad Request\r\n";
}hd;
serv* serv::server;
int serv::run;
int num;

 serv::serv(){
    run=1;
   int tmp=1;
       sockddr.sin_family=AF_INET;
         sockddr.sin_addr.s_addr=INADDR_ANY;
         sockddr.sin_port=htons(80);

 length=sizeof(struct sockaddr);
 socklen=sizeof(struct sockaddr);
  que_round=0;
  
 if((sock=socket(AF_INET,SOCK_STREAM,0))<0){  throw 1; };
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
 if(bind(sock,(struct sockaddr*)&sockddr,length)<0){  throw 2; };

  set_non_block(sock);
  
  if((efd=epoll_create(65536))<0){ close(sock);  throw 3; };
 
    
   ev.data.fd=sock;
    ev.events=EPOLLIN;
 
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
  
   ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
 listen(sock,1000);
 que_count=1;
  que_round=0;
  run=1;
  server=this;
 
 pthread_t pt;
 pthread_create(&pt,0,th,this);
  };


 serv::serv(int s){
  sock=s;
   /*int tmp=1;
       sockddr.sin_family=AF_INET;
         sockddr.sin_addr.s_addr=INADDR_ANY;
         sockddr.sin_port=htons(80);

 length=sizeof(struct sockaddr);
 socklen=sizeof(struct sockaddr);
  que_round=0;
  
 if((sock=socket(AF_INET,SOCK_STREAM,0))<0){  throw 1; };
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
 if(bind(sock,(struct sockaddr*)&sockddr,length)<0){  throw 2; };

  set_non_block(sock);
  */
  if((efd=epoll_create(65536))<0){  throw 3; };
 
    
   ev.data.fd=sock;
    ev.events=EPOLLIN;
 
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
  
   ev.events=EPOLLIN |  EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;
 /*listen(sock,1000);*/
 que_count=1;
  que_round=0;
  run=1;
  server=this;
 
 pthread_t pt;
 pthread_create(&pt,0,th,this);
  };
 
 serv::~serv(){ 
          
                for_each(cache_map.begin(), cache_map.end(), [](pair<size_t,cache_t> ch){ delete[] ch.second.pointer; });
                 for_each(conn_map.begin(), conn_map.end(), [](pair<int,conn*> ch){ delete ch.second; });cout<<"EXIT"<<endl;
               }; 
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
 
/*
     mt.lock(); 
   if(ques.size()==0) {  mt.unlock(); continue; };
     int ss=ques.front();ques.pop();
   cs=conn_map[ss];if(cs==NULL){ cout<<"NULL  ==================  "<<ss<<"  SZ  "<<ques.size()<<endl; mt.unlock(); continue; };
  
   */
    /*std::cout<<" QUEU  "<<cs->state<<" SOCK  "<<cs->fd<<"  SZZZ   "<<ques.size()<<std::endl;*/
      
   switch(cs->state){
    case SOCK_READ : error=0;
      pos=cs->buf_recv; rsz=cs->size_recv;
    
     if(!strncmp(pos,hd.s14,3)){
      tmp=pos+3; i=0;
 try{
      while(i==0){ tmp++;
          
       while(*(tmp+i)!=' '){  i++; if((pos+rsz)==(tmp+i)){ throw 1; }; }; };
       
        char s[i]; s[i]='\0';
         copy(tmp, tmp+i, s);
          errno=0;
        i=stat(s,&st); if(i<0 || errno==ENOENT){ throw 2; };
          
       if(st.st_size>10000){ cs->type=1;
          cs->buf_size=st.st_size;
         if((cs->file_fd=open(s,O_RDONLY))==0){ throw 2; };
              }
               else{ cs->type=0;
         if((i=cacher((const char*)s, ch))!=0){ throw i; };
               cs->buf_send=ch.pointer; cs->buf_size=ch.size;
            
               };
           
         }catch(int er){ error=er; 
         switch(error){ case 1 : cs->buf_send=const_cast<char*>(hd.err_s1); cs->buf_size=strlen(hd.err_s1); break;
                    case 2 : cs->buf_send=const_cast<char*>(hd.err_s1); cs->buf_size=strlen(hd.err_s1); break;
                    default : cs->buf_send=const_cast<char*>(hd.err_s3); cs->buf_size=strlen(hd.err_s3); break;
                       
                 }; cs->state=SOCK_WRITE;               
               };
                  if(cs->state!=SOCK_WRITE){
                   cs->send_header(); };
                  /*ev.data.fd=cs->fd;
                    ev.events=EPOLLET | EPOLLOUT | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
          epoll_ctl(efd,EPOLL_CTL_MOD,cs->fd,&ev);*/
                        break;
        };
         
        
 /*     
  case KEEP_WAIT : 
    if(cs->keep_count==5){cout<<" DEL "<<cs->fd<<endl; 
                      shutdown(cs->fd,SHUT_RDWR);  }
      else{ cs->keep_count++; queue_insert(cs->fd); }; 
            
            break;*/
 default : break;
      };
     };
  
void conn::send_header(){
  
  int sz;
  char date[25];

  time_t tm_t;
   time(&tm_t);
  strncpy(date,asctime(localtime(&tm_t)),25);
  date[24]='\0';
   ostringstream ost;
   ost.rdbuf()->pubsetbuf(header, 1024);
   ost.seekp(0);
      ost<<hd.s0
      <<hd.s2
      <<hd.s17
      <<date<<hd.s19  
      <<hd.s9<<hd.s20
      <<hd.s18
      <<hd.s10
      <<buf_size<<"\r\n"
    <<hd.s5;
 
  header_len=static_cast<int>(ost.tellp());
     
                 
        state=SOCK_HEADER;
         return;
          
          
          };
         
 void serv::reactor(){
 int cur, s, i, tmp, fl=-1;
 conn* p;

   while(1)
{  
  
  cur=epoll_wait(efd,events,512, 1);
     
    for(i=0; i<cur; i++)
     { 
         pev=&events[i]; s=events[i].data.fd;
          p=conn_map[s];
       if ((pev->events & EPOLLERR) || (pev->events & EPOLLHUP) && (pev->events & EPOLLRDHUP))
	         { 
	           delete p;
                 mt.lock();conn_map.erase(s);mt.unlock();
           epoll_ctl(efd, EPOLL_CTL_DEL, s, pev);
              close(s);
                 
                  continue; 
                      };

           
          
            if(s==sock){ 
                      
          if((s=accept(sock,(struct sockaddr*)&sockddr,&socklen))<0){  continue; };
                             
              set_non_block(s);
                   
                    ev.data.fd=s;
                 /* cout<< "NEW  "<<s<<endl;/
                  ev.events=EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP | EPOLLRDHUP;*/
                  if(epoll_ctl(efd,EPOLL_CTL_ADD,s,&ev)<0){ close(s); continue; };
                   
                     try{  p=new conn(s); }catch(std::bad_alloc& g){
                        epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s);  continue; }; 
                                     
                       pair<conn_it,bool> it=conn_map.insert(pair<int,conn*>(s,p));
                                    
                       
                        if(it.second==false){delete p; conn_map.erase(s);
                        epoll_ctl(efd,EPOLL_CTL_DEL,s,&ev); close(s); continue; };
                              
                            if(p->read_s()){
                               queue_insert(s);
                                     };
                           proc_thread(p);

                            if(p->write_s()){ 
                                 queue_insert(s);
                                        };
                             continue;
                            }
                    else {
                     
                        if(pev->events & EPOLLIN){           
                              if(p->read_s()){
                                     queue_insert(s); };                  
                                        continue;
                                    };
                 if(pev->events & EPOLLOUT)
                          { if(p->write_s()){ 
                                 queue_insert(s);
                                  continue;
                             };
                            };
                          };
                         };
                       };
                      };
                      
                    

 inline void serv::queue_insert(int s){ 
   /*cout<<"QUEUE INSERT "<<s<<endl;*/
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
          try { chc.pointer=new char[len]; }catch(std::bad_alloc& b){ ifs.close(); return 2; };
       ifs.read(chc.pointer, len);
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
 void serv::sig_pipe(int n){ cout<<"PIPEW"<<endl;  };
 void* serv::th(void* a) { ((serv*)a)->reactor(); return NULL; };
              
 int conn::write_s(){
   int i; 
           
           if(state==SOCK_READ || state==KEEP_WAIT){   return 0; };

           
         if(state==SOCK_HEADER){
                 
          do{errno=0;  send(fd,header,header_len,0);   

           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
                
               shutdown(fd,SHUT_RDWR);  return 0; }; 
                state=SOCK_WRITE; i=0; break;
                    }while(1);
                  };
      
         if(type==0){
       do{ errno=0; i=write(fd, buf_send+size_tr, buf_size-size_tr);
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){  errno=0; continue; };
                  shutdown(fd,SHUT_RDWR);  return 0; }; 
                size_tr+=i; i=0; break;
                    }while(1);
                 };
    if(type==1){ 
        do{errno=0; i=sendfile(fd, file_fd, 0, buf_size);
           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
                shutdown(fd,SHUT_RDWR);  return 0; }; 
                size_tr+=i; i=0; break;
                 }while(1);
                };
             
          if(size_tr==buf_size){ if(type==1){ close(file_fd); };
             size_recv=0;  shutdown(fd,SHUT_RDWR); state=KEEP_WAIT; i=0;/*cout<<"WR TR  "<<fd<<endl;*/size_tr=0; };

              
           
    return i;
      };

 int conn::read_s(){ 
   int i;
         
      
       if(state==SOCK_WRITE){ return 0; };
    do{errno=0; i=read(fd, buf_recv, 1024); 
        
       if( i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0;  continue; }; 
         size_recv=0; i=0; state=KEEP_WAIT; shutdown(fd,SHUT_RDWR);  i=0; break; };
            size_recv+=i;state==SOCK_READ; i=0;
                
             break; }while(1);/*cout<<"RD  "<<fd<<"  SZ "<<size_recv<<endl;*/
          return i;
        };
      
                                 
