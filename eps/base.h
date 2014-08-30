#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unordered_map>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <conn.hpp>

typedef struct epoll_event epv;

template <typename T, typename U>
class pr: public std::pair<T, U>{
public:
 pr(const T& a, const U& b): std::pair<T, U>(a, std::move(b)){};
 pr(pr&& r): std::pair<T, U>(r.first, std::move(r.second)){};
 ~pr(){};
};
 
 
