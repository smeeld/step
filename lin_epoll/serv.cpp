#include <serv.h>

 int main()
 {
pid_t pid;
int i;

int list_sock;

 passwd* p;

  std::shared_ptr<serv> ser1;
  std::shared_ptr<serv> ser2;

  conn* mas[2];
 sockaddr_in socket_s;
      socket_s.sin_family=AF_INET;
         socket_s.sin_addr.s_addr=INADDR_ANY;
         socket_s.sin_port=htons(80);

 int len=sizeof(struct sockaddr);
  
  try{
  
  if((p=getpwnam("g"))==NULL){ throw 1; };
  
if((list_sock=socket(AF_INET,SOCK_STREAM,0))<0){  throw 2; };
  
 if(bind(list_sock,(struct sockaddr*)&socket_s,len)<0){ throw 3; };
    }
  catch(int er){ 
 
              switch(er){

               case 1 : std::cout<<" Not User named g please do this user"<<std::endl; exit(-1);
               case 2 : std::cout<<" Not Create Socket"<<std::endl; exit(-1); 
               case 3 : std::cout<<" Not Bind Socket"<<std::endl; close(list_sock); exit(-1); 
               };
             };
 i=1;
  
 if (ioctl(list_sock, FIONBIO, &i) &&
	    ((i = fcntl(list_sock, F_GETFL, 0)) < 0 ||
	     fcntl(list_sock, F_SETFL, i | O_NONBLOCK) < 0)) {close(list_sock); exit(-1); };
  

 listen(list_sock,1024);
 
 if((pid=fork())>0){ int fd=open("/var/run/ser.pid", O_RDWR | O_CREAT);
           char bs[16]; sprintf(bs,"%d",pid);
          write(fd,bs,strlen(bs));
        close(fd);
      chmod("/var/run/ser.pid", S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP);
      exit(0);
     };
if((p=getpwnam("g"))==NULL){  exit(-1); };
 

   setsid();
      chdir("/");
    long n = sysconf(_SC_OPEN_MAX);
for(i = n-1; i >= 0; --i) {
     if(i!=list_sock){ close(i); };
        };
   setpgrp();
  int std = open("/dev/null", O_RDWR);
dup(std);
dup(std);
 setuid(p->pw_uid);
 setgid(p->pw_gid);
 signal(SIGINT, serv::sig_hand );
 signal(SIGTERM, serv::sig_hand );
 signal(SIGPIPE, SIG_IGN );
 

 
 i=0;
 try{

  ser1=std::make_shared<serv>(list_sock);

    }
  
 catch(std::bad_alloc& d){ i++; }
 catch(int c){ ser1.reset(); i++; };
 
try{

  ser2=std::make_shared<serv>(list_sock);

    }
  catch(std::bad_alloc& d){ if(i){ exit(-1); }; }
  catch(int c){ if(i){ exit(-1); }; ser2.reset(); i=2; }

 while(serv::run){ 
 
 switch(i){
   case 0 :
     ser1.get()->proc_queue(); 
     ser2.get()->proc_queue();
        break;
  case 1 :
 
     ser1.get()->proc_queue(); 
        break;
  case 2 : 

  ser2.get()->proc_queue(); 
        break;
 
  default : break;
  
     };
   usleep(1); 
  };
};
     

