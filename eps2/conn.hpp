#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#define REQ_HEAD 1
#define REQ_READ 2
#define REQ_WRITE 3
#define REQ_SHUT 4
#define REQ_WAIT 5
#define REQ_SENDFILE 1
#define REQ_SENDMSG  2
class conn{
public:
conn()
 : keep(1), hand(0), size_read(0),
    size_tr(0), size_recv(0), state(REQ_READ),
   keep_count(0), q_nm(0)
   { msg.msg_name=NULL;
     msg.msg_namelen=0;
     msg.msg_iov=iov;
     msg.msg_iovlen=0;
     msg.msg_control=NULL;
     msg.msg_controllen=0; };
~conn(){};
int fd;
int index;
uint8_t state;
int keep;
int type;
uint8_t hand;
char buf_recv[1024];
int file_fd; 
std::ostringstream ost;
int8_t keep_count;
size_t size_tr;
size_t size_recv;
size_t size_read;
char* buf_send;
size_t buf_size;
uint8_t q_nm;
struct msghdr msg;
struct iovec iov[16];
 int msg_add(char* bf, size_t sz){

  struct iovec* pvc=msg.msg_iov;
  size_t nm=msg.msg_iovlen;
  if(nm==16) return 0;
  pvc+=nm;
  msg.msg_iovlen++;
  pvc->iov_base=bf;
  pvc->iov_len=sz;
  return 1;
  };

          
 int msg_check(size_t tr){
  size_t sz=tr, tmp=0;;
  struct iovec* pvc=msg.msg_iov;
  unsigned int cnt=msg.msg_iovlen, res; res=cnt;
  do{ tmp+=pvc->iov_len;
  if(tmp>sz) break;
  --cnt; pvc++;
   }while(cnt);
 if(cnt==0) pvc=iov;
 msg.msg_iov=pvc;
 msg.msg_iovlen=cnt;
 return cnt;
 };

  }; 

