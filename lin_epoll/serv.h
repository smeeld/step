#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <thread>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <all.hpp>
#include <functional>
#include <pthread.h>
#include <mutex>
#include <string.h>
#define REQ_HEAD 1
#define REQ_READ 2
#define REQ_WRITE 3
#define REQ_HAND 4
#define REQ_WAIT 5
#define MAX_CON 10000
#define HTTP_GET 1
#define HTTP_POST 2
typedef std::string str;
struct hd{ 
static const str s0;
static const str s1;
static const str s2;
static const str s3;
static const str s4;
static const str s5;
static const str s6;
static const str s7;
static const str s8;
static const str s9;
static const str s10;
static const str s11;
static const str s12;
static const str s13;
static const str s14;
static const str s15;
static const str s16;
static const str s17;
static const str s18;
static const str s19;
static const str s20;
static const str s21;
static const str s22;
static const str s23;
static const str s24;
static const str s25;
static const str s26;
static const str s27;
static const str s28;
static const str s29;
static const str s30;
static const str s31;
static const str s32;
static const str err_s1;
static const str err_s2;
static const str err_s3;
};
struct request_head{
int method;
int header_size;
char* path;
char* host;
char* content_type;
unsigned long long content_length;
char* encoding;
char* content_language;
int connection_keep;
char* body;
 };
class serv;
typedef struct epoll_event epv;
typedef std::string str;
typedef struct cch{
  std::shared_ptr<char> pointer;
  size_t size;
 }cache_t;
class conn{
public:
conn(){};
conn(int s):fd(s){ keep=0; hand=0; size_read=0; size_tr=0; size_recv=0; state=REQ_READ; keep_count=0; };
~conn(){};
int fd;
int index;
int state;
int keep;
int type;
int hand;
request_head request;
char buf_recv[1024];
int file_fd; 
int header_len;
char header[1024];
std::ostringstream ost;
int8_t keep_count;
size_t size_tr;
size_t size_recv;
size_t size_read;
char* buf_send;
size_t buf_size;
  };   
struct key_mp{
key_mp(const char* s){ int len=strlen(s); if(len<128) { strcpy(str, s); }else{ strncpy(str, s, 127); str[127]='\0'; }; };
char str[128];
 };

  template <typename T>

struct cmp{

 bool operator()(const T& t1, const T& t2) const {

  return !strcmp(t1.str,t2.str); };

 };

template <typename T>
struct hash_mp{

size_t operator()(const T& ss) const { 
   char* p=const_cast<char*>(ss.str);
  std::size_t sz=0; int len=strlen(ss.str);

    for (; len > 0; --len){
      sz = (sz * 131) + *p++;
    };
 return sz; };

 };
 
typedef std::unordered_map<key_mp, cache_t, hash_mp<key_mp>, cmp<key_mp> > cache_mp;
typedef std::unordered_map<int, std::shared_ptr<conn> > conn_mp;
typedef cache_mp::iterator cache_it;
typedef conn_mp::iterator conn_it; 


class serv{
public:
serv(int) throw();
 ~serv(){};
 void reactor();
static void sig_hand(int);
static void sig_pipe(int n);
static int run;
private:
int sock;
 int efd;
int cacher(const char*, cache_t&);
 void pass_hand(const conn*);
void send_header(conn*);
 int read_s(conn*);
 int write_s(conn*);
 int handler();
 static void* th(void*);
 int serv_count;
 cache_mp cache_map;
  conn_mp conn_map;
  conn_it it;
 std::queue<conn*> ques;
 std::queue<conn*> hques;
 std::queue<conn*> rques;
 std::mutex mt;
 struct sockaddr_in sockddr;
 int length;
 socklen_t socklen;
 epv ev, *pev;
 epv events[1024];
  void set_non_block(const int);
 int req_gen(conn*);

  };
