#include <iostream>
#include <packet.hpp>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
 #include <arpa/inet.h>

template <typename T>
struct sender
  {

   int sock;
   struct sockaddr_in sck;
   const char* address;
   T packet;
   uint8_t mode;
 enum{
  interactive_tag,
  from_file_tag
   };

sender(uint8_t md=interactive_tag, const char* addr="127.0.0.1"): sock(0), mode(md), address(addr){};

int transfer()
  {  size_t tmp;
     sck.sin_family=AF_INET;
     sck.sin_addr.s_addr=inet_addr(address);
     sck.sin_port=htons(120);
  if(!sock)
   {
     if((sock=socket(AF_INET, SOCK_STREAM, 0))<=0)
            { std::cout<<"Cannot Open Socket\n"; return -1; };
     if(connect(sock, (struct sockaddr*)&sck, sizeof(struct sockaddr))<0)
            { std::cout<<"Cannot Connect To Server\n"; return -1; };
    };

  if((tmp=send(sock, packet.pointer(), packet.size(), 0))<=0)
       { std::cout<<"Cannot Send Request to a Server\n"; sock=0; return 0; };

   return tmp;
 };



 void interactive(std::istream& ist, std::ostream& ost)
   {
 
  while(1)
   { 
     uint8_t type;
     ost<<"Choose Type\n 1 is a Pont\n 2 is a Line\n 3 is a Ellipse\n q for quit from \n Value=";
     ist>>type;
  switch(type)
   { 
   case 0x31 :
    ost<<"\nPoint Selected\nInsert Coords for a Point with foramt: [ X1 Y1 ] : ";
    type=T::HDR_POINT;
    break;
   case 0x32 :
    ost<<"\nLine Selected\nInsert Coords for a Line with format: [ X1 Y1 X2 Y2 ] : ";
    type=T::HDR_LINE;
    break;
   case 0x33 :
    ost<<"\nEllipse Selected\nInsert Parameters for a Ellipse with format: [ X1 Y1 Height Width Angle1 Angle2 ] : ";
    type=T::HDR_ELLIPSE;
    break;
   case 0x71 : ost<<"Goodbay \n"; return;
  };
 
   if(make_from_istream(type, std::cin)==0) if(transfer()<0) return;
  };
};
 

int make_from_istream(uint8_t type, std::istream& ist)
  {
   uint8_t chr;   
   ist>>chr;
  if(chr=='[')
  {
   packet.make_from_istream(type, ist);
   ist>>chr;
   if(chr==']') return 0;
   };
 return -1;
  };

void thread()
 {
  switch(mode)
   {
     case interactive_tag:  interactive(std::cin, std::cout);
    };
  }

};
 
