#include <server.hpp>
#include <designer.hpp>
int main()
 {
 pid_t pid;
 int i;
 int lstn;
struct sigaction sgc;
 

 struct sockaddr_in socket_s;
      socket_s.sin_family=AF_INET;
         socket_s.sin_addr.s_addr=INADDR_ANY;
         socket_s.sin_port=htons(120);

 int len=sizeof(struct sockaddr);
  

 if((lstn=socket(AF_INET, SOCK_STREAM,0))<0) return -1;
 if(bind(lstn,(struct sockaddr*)&socket_s,len)<0) return -1;
 

 i=1;
  
 if (ioctl(lstn, FIONBIO, &i) &&
	    ((i = fcntl(lstn, F_GETFL, 0)) < 0 ||
	     fcntl(lstn, F_SETFL, i | O_NONBLOCK) < 0)) {close(lstn); exit(-1); };
  

 listen(lstn,1024);
 
 if((pid=fork())>0) exit(0);

   setsid();
      chdir("/");
    long n = sysconf(_SC_OPEN_MAX);
for(i = n-1; i >= 0; --i) 
    if(i!=lstn) close(i);
      
   setpgrp();
  int std = open("/dev/null", O_RDWR);
  dup(std);
  dup(std);
 
 server<designer> srv(lstn);
 srv.run();
 };
