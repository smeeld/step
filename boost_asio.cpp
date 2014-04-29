#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
using boost::asio::ip::tcp;
typedef struct _cache{
int size;
char* cache;
} cache;
 
  string errors[]={"NO SUCH FILE PLEASE CHECK THE URL IN  REQUEST BOX",
                "INTERNAL SERVE ERROR" };

class conn;
class serv

{
public:
serv();
~serv();

boost::asio::io_service io;
tcp::acceptor accept;
tcp::socket sock;
unordered_map<string, cache> cache_map;
int check_cache(const char*,cache&);
unordered_set<class conn*> conn_mas;
void start(void);
static void sig_hand(int);
static  serv* serv_ptr;
};
  class conn{
public:
conn( serv*);
~conn();
tcp::socket sock;
void start(const boost::system::error_code&);
private:
char buf[64];
 size_t count;
 serv* server;
  int pos;

char* read_buf[1024];
boost::asio::streambuf outs;
cache ch;
size_t buffer_size;
size_t tr_count;
void write_handler(const boost::system::error_code&,std::size_t);
void writer(const boost::system::error_code&,std::size_t);
size_t header;
 }; 
serv::serv():
io(),
accept(io, tcp::endpoint(boost::asio::ip::address::from_string("10.10.10.1"),80), tcp::acceptor::reuse_address(true)),
sock(io)
{ serv_ptr=this; signal(SIGINT, sig_hand);

  signal(SIGTERM, sig_hand); };
serv::~serv(){ int i;
             unordered_map<string, cache>::iterator it=cache_map.begin();
              unordered_set<conn*>::iterator cit=conn_mas.begin();
              while(it!=cache_map.end()){ 
                                free(it->second.cache);
                                        it++; 
                                               };
                while(cit!=conn_mas.end()){ delete *cit; cit++; };
         };
void serv::start(){
 
 conn* p=NULL;

try{

  p=new conn(this);
 
   }catch(std::bad_alloc& c){ return; };

 if(!conn_mas.insert(p).second){ p->sock.close(); delete p; };

 accept.async_accept(p->sock, boost::bind(&conn::start, p, _1));  
     
  };
int serv::check_cache(const char* s, cache& ch)
 {unordered_map<string,cache>::iterator it;
  
   if((it=cache_map.find(string(s)))!=cache_map.end())
       { ch.cache=it->second.cache; ch.size=it->second.size; return 0 ; }
     else {
           fstream fs(s);
           if(!fs.good()){ return 1; };
            fs.seekg(0,fs.end);
 
        int size=fs.tellg();
         fs.seekg(0,fs.beg);
          try{
         ch.cache=new char[size];
          }catch(std::bad_alloc& a){ fs.close(); return 2; };
 do{
       fs.read(ch.cache,size);
      }while(!fs.eof());
       ch.size=size;
       cache_map.insert(pair<string,cache>(string(s),ch));
       fs.close();
       return 0;
     };
    };

 void serv::sig_hand(int s){ serv_ptr->io.stop(); };

conn::conn( serv* serv):
sock(serv->io),
server(serv)
{ 
  tr_count=0;
   };

conn::~conn(){ 
 
 sock.close(); server->conn_mas.erase(server->conn_mas.find(this));
  
  };

 void conn::start(const boost::system::error_code& er){

 if(er){
 sock.close(); return;
};

  sock.async_read_some(boost::asio::buffer(read_buf, 1024), boost::bind(&conn::write_handler, this, _1, _2));
 server->start();
  };

void conn::writer(const boost::system::error_code& er,std::size_t tr){
   
   if(er==boost::asio::error::eof || tr==0) { delete this; };
   tr_count+=tr;
  if(tr_count==buffer_size){ delete this;
  }else
    { 
      size_t s=tr_count-header;
       
  boost::asio::async_write(sock, boost::asio::buffer(ch.cache+s, ch.size-s),boost::bind(&conn::writer,this,_1, _2));
      };
     };

  void conn::write_handler(const boost::system::error_code& er, std::size_t tr)
 { int i;
  do{
   if(tr == 0 || er==boost::asio::error::eof){ delete this; }
     else
     {
    read_buf[tr]='\0';
   std::ostream ost(&outs);
   std::string str((const char*)read_buf);
   std::string name(str.substr(4,str.find(' ', 5)-4));
   if(!str.compare(0,3,"GET")){  
             if((i=server->check_cache((const char*)name.c_str(),ch))!=0){
                  
                ch.size=errors[i-1].size();
               ch.cache=(char*)errors[i-1].c_str();
                };
         time_t tm_t;
        time(&tm_t);
    string datastr(asctime(localtime(&tm_t)));
     ost<<"HTTP/1.1 200 OK\r\n"
     <<"Date: "
     <<datastr.substr(0,datastr.find('\n')).c_str()<<" GMT\r\n"
     <<"Server: MATRIX\r\n"
     <<"Content-Type: */*; charset=utf-8\r\n"
     <<"Content-Length: "<<ch.size<<"\r\n"
     <<"Connection: keep-alive\r\n\r\n";
     header=outs.size();
    ost<<ch.cache;
       buffer_size=ch.size+header;
 
       boost::asio::async_write(sock,outs,boost::bind(&conn::writer,this,_1, _2)); break;
      };
    };
   }while(0);   
 };
  
class serv* serv::serv_ptr;

int main(){

 serv ser;

  ser.start();
  ser.io.run();
 };
