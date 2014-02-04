#include <iostream>
#include <set>
#include <signal.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <exception>
#include <unordered_map>
#include <stdio.h> 
#include <time.h> 

using namespace std;
using boost::asio::ip::tcp;
typedef struct _cache{
int size;
char* cache;
} cache;
class conn;
class serv
: private boost::noncopyable
{
public:
serv();
~serv();
void start(class conn*);
boost::asio::io_service io;
tcp::acceptor accept;
unordered_map<string, cache> cache_map;
int check_cache(const char*,cache&);
std::set<class conn*> conn_mas;


};
  class conn{
public:
conn(class serv&);
~conn(){ 
  try{
 sock.close(); server.conn_mas.erase(server.conn_mas.find(this));
   }catch(std::exception& er){ cerr<<er.what()<<endl; };
  };
tcp::socket sock;
char buf[64];
 size_t count;
 class serv& server;
  int pos;
void read_handler(const boost::system::error_code&);
private:
char* read_buf[1024];
boost::asio::streambuf outs;
cache ch;
size_t buffer_size;
size_t tr_count;
void finish_handler(const boost::system::error_code&);
void write_handler(const boost::system::error_code&,std::size_t);
void writer(const boost::system::error_code&,std::size_t);
size_t header;
 }; 
conn::conn(class serv& serv):
sock(serv.io),
server(serv)
{ server.conn_mas.insert(this);
  tr_count=0;
   };
serv::serv():
io(),
accept(io,tcp::endpoint(boost::asio::ip::address::from_string("10.10.10.1"),80),tcp::acceptor::reuse_address(true))
{ };
serv::~serv(){ int i;
             unordered_map<string, cache>::iterator it=cache_map.begin();
              set<class conn*>::iterator cit=conn_mas.begin();
              while(it!=cache_map.end()){ 
                                free(it->second.cache);
                                        it++; 
                                               };
                while(cit!=conn_mas.end()){ delete *cit; cit++; };cout<<"EX"<<endl;
         };
void serv::start(class conn* c){
 
   
accept.async_accept(c->sock,boost::bind(&conn::read_handler, c, _1));

  };
int serv::check_cache(const char* s,cache& ch)
 {unordered_map<string,cache>::iterator it;
  
   if((it=cache_map.find(string(s)))!=cache_map.end())
       { ch.cache=it->second.cache; ch.size=it->second.size; return 0 ; }
     else {
           fstream fs(s);
           if(!fs.good()){ return -1; };
            fs.seekg(0,fs.end);
 
        int size=fs.tellg();
         fs.seekg(0,fs.beg);
         ch.cache=new char[size];
  
 do{
       fs.read(ch.cache,size);
      }while(!fs.eof());
       ch.size=size;
       cache_map.insert(pair<string,cache>(string(s),ch));
     cout<<"INSERTED"<<endl;
       return 0;
     };
    };
void conn::writer(const boost::system::error_code& er,std::size_t tr){
   
   if(er==boost::asio::error::eof || tr==0) { delete this; };
   tr_count+=tr;/*cout<<"TRANSFERED  "<<tr_count<<"   MID  "<<tr<<" BUF SIZE  "<<buffer_size<<" CAHE "<<ch.size<<endl;*/
  if(tr_count==buffer_size){ delete this;/*buffer_size=0; tr_count=0; 
   sock.async_read_some(boost::asio::buffer(read_buf,1024),boost::bind(&conn::write_handler,this, _1, _2));*/ 
  }else
    { 
      size_t s=tr_count-header;
       
  boost::asio::async_write(sock,boost::asio::buffer(ch.cache+s, ch.size-s),boost::bind(&conn::writer,this,_1, _2));
      };
     };
void conn::finish_handler(const boost::system::error_code& er){
if(er){
    std::cerr << er.message()<<"LARGE ERROR" << std::endl;};
delete this;
  };
void conn::read_handler(const boost::system::error_code& er)
{
if(er){cerr<<er.message()<<endl;
   finish_handler(er); };
 sock.async_read_some(boost::asio::buffer(read_buf, 1024),boost::bind(&conn::write_handler,this, _1, _2));
  server.start(new conn(server));
  
};
  void conn::write_handler(const boost::system::error_code& er, std::size_t tr)
 { 
  do{
   if(tr == 0 || er==boost::asio::error::eof){ delete this; }
     else
     {
    read_buf[tr]='\0';
   std::ostream ost(&outs);
   std::string str((const char*)read_buf);
   std::string name(str.substr(4,str.find(' ', 5)-4));
   if(!str.compare(0,3,"GET")){
             if(server.check_cache((const char*)name.c_str(),ch)<0){
         
              ost<<"NO FILE"<<name.c_str()<<"FOR LOAD"<<endl;
               
             boost::asio::async_write(sock,outs,boost::bind(&conn::finish_handler,this,_1));
         }else
     {
         time_t tm_t;
        time(&tm_t);
    string datastr(asctime(localtime(&tm_t)));
     ost<<"HTTP/1.1 200 OK\r\n"
     <<"Date: "
     <<datastr.substr(0,datastr.find('\n')).c_str()<<" GMT\r\n"
     <<"Server: MATRIX\r\n"
     <<"Content-Type: text/html; charset=utf-8\r\n"
     <<"Content-Length: "<<ch.size<<"\r\n"
     <<"Connection: keep-alive\r\n\r\n";
     header=outs.size();
     ost<<ch.cache;
       buffer_size=ch.size+header;
  
       boost::asio::async_write(sock,outs,boost::bind(&conn::writer,this,_1, _2)); break;
     };
    };
    };
   }while(0);   
 };


int main(){

 serv ser;
  ser.start(new conn(ser));
  ser.io.run();
 };
