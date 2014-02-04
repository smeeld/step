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

char* path="/var/www/html%s";
char* header="HTTP/1.1 200 OK\r\n"
     "Date: xxxxxxxxxxxxxxxxxxxxxxxx GMT\r\n"
     "Server: MATRIX\r\n"
     "Content-Type: %s; charset=utf-8\r\n"
     "Content-Length: %d \r\n"
     "Connection: keep-alive\r\n\r\n";
  char* error_message="<html><dody>NO SUCH FILE</body></html>";
  char* mime[]={"text/html", "image/jpeg"};
int http_version;

char* date;
time_t tm_t;
char* str;
char string[512];
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
     int file_fd;
     int file_sz;
     int error;
     int flag;
    struct msghdr msg;
     struct iovec iov[2];
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
 
 exit(0); };
int cacher(char* s,struct cache* ch){
int fd;
int count;
int tmp;
struct stat st;
char t_buf[4096];
  count=0; 
  size_t Sz;
if((fd=open(s,O_RDONLY))<0){   return -1;};
  fstat(fd, &st);
char* buf=(char*)malloc(sizeof(char)*st.st_size);

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
  char* buf;
  char* sts;
    int i=0;
      struct stat st;
      c->msg.msg_name=NULL;
                  c->msg.msg_namelen=0;
                   c->msg.msg_iov=c->iov;
                    c->msg.msg_iovlen=1;
                    c->msg.msg_flags=0;
                      c->iov[0].iov_base=&(c->buf_get);
                       c->iov[0].iov_len=1024;
           errno=0;
      do{ i=recvmsg(c->fd, &(c->msg),0); 
      
       if( i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; }; shutdown(c->fd,SHUT_RDWR); return; };  
             break; }while(1);
         
        if(strncmp(c->buf_get,"GET",3)==0)
       {
         sts=strstr(c->buf_get,"HTTP"); 
        c->buf_get[sts-c->buf_get-1]='\0';
          
           sprintf(str,path, c->buf_get+4);
            do{
              if(strstr(str,".html")==NULL){ 
                    if((c->file_fd=open(str,O_RDONLY))>0){
                 c->error=0;
                     c->type=1;
                  c->msg.msg_iovlen=1; 
            fstat(c->file_fd,&st); 
                c->buf_size=st.st_size;
                  c->file_sz=st.st_size; 
                    c->size_tr=0;
                }else{c->error=1; };
                  break;
               };
                 struct cache* ch;
                  
              if(check_cache(str, &ch)==0){ 
                  c->error=0;
                   c->type=0;
                     c->msg.msg_iovlen=2;
                      c->iov[1].iov_base=ch->buf;
                       c->iov[1].iov_len=ch->size;
                        c->buf_size=ch->size;
                        c->size_tr=0;      
                       }else{c->error=1; };
                    break;
                  }while(0);
   
               if(c->error==1){ c->type=0;
                     c->msg.msg_iovlen=2;
                     c->iov[1].iov_base=error_message;
                    c->iov[1].iov_len=strlen(error_message);
                      c->buf_size=c->iov[1].iov_len;
                     }; 
            time(&tm_t);
             date=asctime(localtime(&tm_t));
            sprintf(string,header,mime[c->type],c->buf_size);
         memcpy(string+23,date,24);
             c->iov[0].iov_base=string;
              c->iov[0].iov_len=strlen(string);
                c->buf_size+=c->iov[0].iov_len; 
                 c->flag=0;  
         write_s(c);
           return;
            };
        write(c->fd,"HTTP/1.1 400 Bad Request",strlen("HTTP/1.1 400 Bad Request"));
       };
  void write_s(struct connection* c){
     int i;
     if(c->flag==0){
      do{ errno=0; i=sendmsg(c->fd,&(c->msg),0);
          if(i==0 || i<0){ if(errno==EAGAIN || errno==EINTR){ errno=0; continue; };
              shutdown(c->fd,SHUT_RDWR); return; }; 
                c->size_tr+=i;
                if(c->type==1){ c->flag=1; }; break; }while(1);
                  };
    if(c->flag==1){ 
        do{errno=0; i=sendfile(c->fd, c->file_fd, 0, c->file_sz);
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
cache_count=0;
conn_count=0;
char strpath[strlen(path)];
str=strpath;
struct connection* conn2;
 struct passwd* p;
 signal(SIGINT, sig_hand );
 signal(SIGTERM, sig_hand );
 events=(struct epoll_event*)malloc(32000*sizeof(struct epoll_event));
 caches=(struct cache*)malloc(1024*sizeof(struct cache));
  conn=(struct connection*)malloc(32000*sizeof(struct connection));
int length=sizeof(struct sockaddr);
 struct sockaddr_in socket_s={ 
      .sin_family=AF_INET,
      .sin_addr.s_addr=INADDR_ANY,
       .sin_port=htons(80)
       };

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

    ( conn+sock )->fd=sock;
if((efd=epoll_create(1024))<0){  exit(-1); };
struct epoll_event ev={
      .data.ptr=conn+sock,
      .events=EPOLLIN
        };
epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev);
 ev.events=EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
  int s;
     
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
                    
                       set_non_bl(s); 
                           (conn+s)->fd=s;
                          
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

     

