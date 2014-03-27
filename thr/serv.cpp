#include <serv.h>

 int main()
 {
pid_t pid;
int i;
num=100000;
 
 struct passwd* p;
 
 if((p=getpwnam("g"))==NULL){  exit(-1); };
 
  std::shared_ptr<serv> ser1;
  std::shared_ptr<serv> ser2;

 try{

  ser1=std::make_shared<serv>();

    }
  catch(int c){ int s=c;  
      switch(s){ 
         case 1 : std::cout<<"Error Open Socket"<<std::endl; break; 
         case 2 : std::cout<<"Error Bind Socket"<<std::endl; break;
         case 3 : std::cout<<"Error  For Events"<<std::endl; break;
         default : std::cout<<"Error  For Events"<<std::endl; break;
            }; 
 exit(-1); }
 
 catch(std::bad_alloc& d){ std::cout<<"Error MEmory"<<std::endl; exit(-1); }

try{

  ser2=std::make_shared<serv>(ser1->sock);

    }
  catch(std::bad_alloc& d){ std::cout<<"Error MEmory"<<std::endl; exit(-1); }
 
 /*
  try{
  
   run2=new serv(run->sock);
 run2->reactor();
 }catch(int i){  std::cout<<"Error  For Events"<<std::endl; }
 catch(bad_alloc& d){ std::cout<<"Error MEmory"<<std::endl; }
/*if((pid=fork())>0){ int fd=open("/var/run/ser.pid", O_RDWR | O_CREAT);
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
 setgid(p->pw_gid);*/
 signal(SIGINT, serv::sig_hand );
 signal(SIGTERM, serv::sig_hand );
 signal(SIGPIPE, SIG_IGN );
 
 while(serv::run){ 
    ser1.get()->proc_queue();
    ser2.get()->proc_queue();
   usleep(1); };

  };
     

