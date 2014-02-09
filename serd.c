#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
 int main(int c, char* s[]){
 int fd;
 int i;
char fb[16];
 pid_t pid=-1;
  if(!strncmp(s[1],"stop",strlen("stop"))){
           if((fd=open("/var/run/ser.pid", O_RDWR))<0){ exit(-1); };
          read(fd,&pid,sizeof(pid_t));
 printf("CLOSED  %d\n",pid);
        close(fd);
         kill(pid, SIGINT);
         unlink("/var/run/ser.pid");
        exit(0);
         };
      pid=fork();
     if(pid>0){
           fd=open("/var/run/ser.pid", O_RDWR | O_CREAT);
          write(fd,&pid,sizeof(pid_t));
        close(fd);
      sleep(2);
  if((fd=open("/var/run/ser.pid",O_RDONLY))<0){ exit(-1); };
       close(fd);
         exit(0);
         };
      if(pid==0){
          
     setsid();
      chdir("/");
    long n = sysconf(_SC_OPEN_MAX);
for(i = n-1; i >= 0; --i) {
     if(i!=fd){ close(i); };
};
 setpgrp();
        execl("/home/g/ser",NULL);

 };
};   
     

