#include <serv.h>


 int main()
 {
pid_t pid;
int i;
num=100000;
 
 struct passwd* p;
 
 if((p=getpwnam("g"))==NULL){  exit(-1); };
 
  serv run;  
 /*}catch(int c){

      switch(c){ 
         case 1 : std::cout<<"Error Open Socket"<<std::endl; break; 
         case 2 : std::cout<<"Error Bind Socket"<<std::endl; break;
         case 3 : std::cout<<"Error Alloc Memeory For Events"<<std::endl; break;
            }; 
 exit(-1); };*/

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
     if((i!=run.sock) && (i!=run.efd)){ close(i); 
         };
        };
   setpgrp();
  int std = open("/dev/null", O_RDWR);
dup(std);
dup(std);
 setuid(p->pw_uid);
 setgid(p->pw_gid);
 signal(SIGINT, serv::sig_hand );
 signal(SIGTERM, serv::sig_hand );
 
 run.reactor();

  };
     

