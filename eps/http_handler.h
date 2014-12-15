#include <string.h>
#include <unordered_map>
#include <memory>
#include <serv.hpp>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sstream>
#define MAX_CON 10000
#define HTTP_GET 1
#define HTTP_POST 2

 
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

class http_handler;
 class http_conn : public conn{

 public: 
 http_conn(){};
 ~http_conn(){};
  request_head request;
  char header[1024];
  int header_len;
 };
typedef struct cch{
  std::shared_ptr<char> pointer;
  size_t size;
 }cache_t; 
struct key_mp{
key_mp(const char* s){ 
 int len=strlen(s); if(len<128) { strcpy(str, s); }else{ strncpy(str, s, 127); str[127]='\0'; }; };
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

class http_handler{
 public:
typedef std::unordered_map<key_mp, cache_t, hash_mp<key_mp>, cmp<key_mp> > cache_mp;
 http_handler(){};
 ~http_handler(){};
 void send_header(http_conn*);
 int  req_gen (http_conn*);
 int cacher(const char*, cache_t& );
 void error_hand(http_conn*, uint8_t);
 void handler(conn* s);
 conn* create_conn();
conn* create_conn(conn*);
 cache_mp cache_map;
 };


