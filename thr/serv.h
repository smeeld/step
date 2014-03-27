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
#define SOCK_READ 1
#define SOCK_WRITE 3
#define MAX_CON 10000
#define KEEP_WAIT 10
#define SOCK_HEADER 2
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
static const str err_s1;
static const str err_s2;
static const str err_s3;
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
conn(int s):fd(s){  size_tr=0; size_recv=0; state=SOCK_READ; keep_count=0; };
~conn(){};
 
int fd;
int index;
int state;
int type;
char buf_recv[1024];
int file_fd; 
int header_len;
char header[1024];
std::ostringstream ost;
int8_t keep_count;
size_t size_tr;
size_t size_recv;
char* buf_send;
size_t buf_size;
  };
 
typedef std::unordered_map<size_t, cache_t> cache_mp;
typedef std::unordered_map<int, std::shared_ptr<conn> > conn_mp;
typedef cache_mp::iterator cache_it;
typedef conn_mp::iterator conn_it; 


class serv{
public:
serv(); 
serv(int);

 ~serv();
int sock;
 int efd;
 void reactor();
int cacher(const char*, cache_t&);
 void proc_thread(const conn*);
 void proc_queue();
static void sig_hand(int);
static void* th(void*);
static void sig_pipe(int n);
static serv* server;
static int run;
private:
void send_header(conn*);
 int read_s(conn*);
 int write_s(conn*);
 int serv_count;
 cache_mp cache_map;
  conn_mp conn_map;
  conn_it it;
 std::hash<char*> hsh;
 std::queue<int> ques;
 std::mutex mt;
 struct sockaddr_in sockddr;
 int que_count;
 int length;
 socklen_t socklen;
 int alloc_sz;
 int que_round; 
 epv ev, *pev;
 epv events[512];
 inline void queue_insert(int);
  void set_non_block(const int);

  };
