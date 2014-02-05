#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

 char* header="HTTP/1.1 200 OK\r\n"
     "Date: xxxxxxxxxxxxxxxxxxxxxxxx GMT\r\n"
     "Server: MATRIX\r\n"
     "Content-Type: %s; charset=utf-8\r\n"
     "Content-Length: %d \r\n"
     "Connection: keep-alive\r\n\r\n";
char* path="/var/www/html%s";

 char* errors[]={"<html><body><h2>check your url box</body></html>",
               "INTERNAL SERVER ERROR",
                "HTTP/1.1 400 Bad Request" };
struct head_inf{
  int path_len;
  int header_len;
  }head_inf_t, *head_p;
void meta_init(struct head_inf* m){
    m->header_len=strlen(header)+82;
   m->path_len=strlen(path);
    };

char* date;
time_t tm_t;
struct cache{
 char file[64];
  char* buf; 
 size_t size;
  };
 struct cache* caches;
 int cache_count;
 int conn_count;
struct connection
  {
    int fd;
    int type;
    int buf_size;
    int size_tr;
     char buf_get[995];
     char* buf_send;
     int file_fd;
   };
 struct connection* conn;
int sock;
void write_s(struct connection* c);
int cacher(char* s, struct cache* ch);
int check_cache(char* s, struct cache** ch);
void conn_close(struct connection* c);
static int efd;
static int current_number;
  struct epoll_event *events;

int check_cache(char* s,struct cache** ch){
int i;
 
 for(i=0;i<cache_count;i++){
   if(!strcmp(caches[i].file, s)){ *ch=caches+i; return 0; };
   
   };
  *ch=caches+cache_count;
return cacher(s, &caches[cache_count++]);
};
void sig_hand(){
int i;
struct connection* c;
 
 for(i=0;i<current_number;i++){
    c=events[i].data.ptr;
     close(c->fd);
  };
  for(i=0;i<cache_count;i++){
     free(caches[i].buf);
      };
   free(events);
    free(caches);
     free(conn);
  close(efd);
   close(sock);
 
 exit(0); };
int cacher(char* s,struct cache* ch){
int fd;
int count=0;
int tmp;
struct stat st;
char t_buf[4096]; 
if((fd=open(s,O_RDONLY))<0){   return 1;};
  fstat(fd, &st);
char* buf; if((buf=(char*)malloc(sizeof(char)*st.st_size))==NULL){ return 2; };
       
while((tmp=read(fd,t_buf,4096))>0){
      
        memcpy(buf+count, t_buf, tmp);
     count+=tmp;
       };
 
 close(fd);
 ch->buf=buf;
  memcpy(ch->file,s,strlen(s));
   ch->size=count;
 return 0;
};
 void read_s(struct connection* c){
   char* sts;
    int i=0;
   char head[head_p->header_len];
      struct stat st;
           errno=0;
      do{ i=read(c->fd, c->buf_get, 4096); 
      
       if( i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; }; shutdown(c->fd,SHUT_RDWR); return; };  
             break; }while(1);
        
     do{
        
         if((sts=strstr(c->buf_get,"HTTP"))==NULL){ i=3; break; };  
   
        if(strncmp(c->buf_get,"GET",3)!=0){ i=3; break; };
         
        c->buf_get[sts-c->buf_get-1]='\0';
          
          i=strlen(c->buf_get)-4;
           
           char file_path[head_p->path_len+i];
            
           sprintf(file_path, path, c->buf_get+4);

           
                if(stat(file_path,&st)<0){ i=1; break; };
                   
                     if(st.st_size>9092){  
                   
                   if((c->file_fd=open(file_path,O_RDONLY))<0){ i=1; break; };
                 i=0;
                     c->type=1;  
                
                 c->buf_size=st.st_size; 
                 
                 c->size_tr=0;
                   break;
                
                 }else{
                 
                struct cache* ch;
                  
              if((i=check_cache(file_path, &ch))!=0){ break; }; 
                  i=0;
                     c->type=0;
                      c->buf_send=ch->buf;
                        c->buf_size=ch->size;
                        c->size_tr=0;     
                    break;
                        };
                     
                    }while(0);

               if( i!=0 ){
                    
                      c->buf_send=errors[i-1];
                    
                   c->buf_size=strlen(errors[i-1]);
                    
                            c->type=0;
                     
                       };
                
            time(&tm_t);
             date=asctime(localtime(&tm_t));
            sprintf(head, header, "*/*", c->buf_size);
         memcpy(head+23,date,24);
               
               do{ errno=0; i=write(c->fd, head , strlen(head));
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
              shutdown(c->fd,SHUT_RDWR); return; }; 
                break; }while(1);
           
              write_s(c);
               
            };

  void write_s(struct connection* c){
     int i;
     
     if(c->type==0){
      do{ errno=0; i=write(c->fd,c->buf_send+c->size_tr,c->buf_size-c->size_tr);
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
              shutdown(c->fd,SHUT_RDWR); return; }; 
                c->size_tr+=i;
                 break; }while(1);
                  };
    if(c->type==1){ 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, c->buf_size);
           if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
              shutdown(c->fd,SHUT_RDWR); return; }; 
                c->size_tr+=i; break;
                 }while(1);
                };
       
          if(c->size_tr==c->buf_size){ if(c->type==1){ close(c->file_fd); };
               shutdown(c->fd,SHUT_RDWR); }; 
    return;
     };
int set_non_bl(int s){
int fl=1;
if (ioctl(s, FIONBIO, &fl) &&
	    ((fl = fcntl(s, F_GETFL, 0)) < 0 ||
	     fcntl(s, F_SETFL, fl | O_NONBLOCK) < 0)) {close(s);
                };
     return 0;
      };
 
 int sock_opt(int s){
  int done=1;
   
   int i=sizeof(done);
        setsockopt(s,SOL_SOCKET,TCP_NODELAY,&done,i);       
              setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &done, i);
   };
 int main()
 {
pid_t pid;
int i;
  int s;
cache_count=0;
conn_count=0;
 
 struct passwd* p;
struct connection* conn2;

struct sockaddr_in socket_s={ 
      .sin_family=AF_INET,
      .sin_addr.s_addr=INADDR_ANY,
       .sin_port=htons(80)
       };

 int length=sizeof(struct sockaddr);
 
 if((p=getpwnam("g"))==NULL){  exit(-1); };
  
if((sock=socket(AF_INET,SOCK_STREAM,0))<0){  exit(-1);};
  
 if(bind(sock,(struct sockaddr*)&socket_s,length)<0){  exit(-1); };
  sock_opt(sock);
 set_non_bl(sock);
 listen(sock,1024);
 
if((pid=fork())>0){ int fd=open("/var/run/ser.pid", O_RDWR | O_CREAT);
           char bs[16]; sprintf(bs,"%d",pid);
          write(fd,bs,strlen(bs));
        close(fd);
      chmod("/var/run/ser.pid", S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP);
      exit(0);
     };

   setsid();
      chdir("/");
    long n = sysconf(_SC_OPEN_MAX);
for(i = n-1; i >= 0; --i) {
     if(i!=sock){ close(i); 
         };
        };
   setpgrp();
  int std = open("/dev/null", O_RDWR);
dup(std);
dup(std);
 setuid(p->pw_uid);
 setgid(p->pw_gid);
 signal(SIGINT, sig_hand );
 signal(SIGTERM, sig_hand );

 if((efd=epoll_create(1024))<0){   sig_hand(0); exit(-1); };
 if((events=(struct epoll_event*)malloc(32000*sizeof(struct epoll_event)))==NULL){ sig_hand(0); exit(-1); }
 if((caches=(struct cache*)malloc(1024*sizeof(struct cache)))==NULL){ sig_hand(0); exit(-1); };
  if((conn=(struct connection*)malloc(1024*sizeof(struct connection)))==NULL){ sig_hand(0); exit(-1); };
  
 
   head_p=&head_inf_t;
   meta_init(head_p);

    ( conn+sock )->fd=sock;

struct epoll_event ev={
      .data.ptr=conn+sock,
      .events=EPOLLIN
        };
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
 ev.events=EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
 
     
while(1)
{  
  
current_number=epoll_wait(efd,events,1024,-1);
  
    for(i=0;i<current_number;i++)
     {   conn2 = (struct connection*)events[i].data.ptr;
       if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) && (events[i].events & EPOLLRDHUP))
	    {
	       epoll_ctl(efd, EPOLL_CTL_DEL, conn2->fd, &events[i]);
                 close (conn2->fd);
                 
                  continue; 
                };           

           if(conn2->fd==sock){
                    if((s=accept(sock,(struct sockaddr*)&socket_s,&length))<0){  continue; };
                     
                      if(s>1024){
                           close(s); continue;
                                };
                         
                       set_non_bl(s);
                           conn[s].fd=s;
                          
                           ev.data.ptr=(conn+s);
                           
                          if(epoll_ctl(efd,EPOLL_CTL_ADD,s,&ev)<0){ close(s); continue; }; 
                          read_s(conn+s);
                          continue; 
                           }
                           else{ if(events[i].events & EPOLLIN){ read_s(conn2); continue; }else{ write_s(conn2); continue; };
                                  };
                               
                              };
                
                            };
                         };  

     

