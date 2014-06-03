#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <condition_variable>
#include <queue>
#include <map>
#include <omp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

  class reactor{

typedef std::map<uint64_t, uint64_t> map_t;
typedef std::pair<uint64_t, uint64_t> pair;
typedef std::queue<uint64_t> que_t;

struct mp_t{
map_t mp;
std::mutex mt;
std::condition_variable cv;
que_t que;
uint8_t run, fl;

uint64_t get(){
 uint64_t val;
 
 std::unique_lock<std::mutex> lck( mt );

do{

  if(!que.empty()) break;
 
  if(fl) { run=0; throw 1; };

     cv.wait(lck); continue; 

      }while(1);

 val=que.front(); que.pop(); return val; };

 };

 struct pth{
  reactor* rp;
  uint8_t index;
 };

 public:
 reactor();
 ~reactor(){ delete [] mp_p; }; 
  void run();
private:
 uint8_t num_th;
 map_t res;
 mp_t* mp_p;
 uint8_t rfl;
void th(uint8_t);
void step(void);
void stop();
static void prl(pth);
 };

reactor::reactor(){ 
 
int i;
  
 if((i=omp_get_max_threads())<2){ rfl=0; return; };
 num_th=i-1;
  rfl=1;
 
try{ 

  mp_p=new mp_t[i-1];

  }catch(std::bad_alloc& c){ rfl=0; return; };

 };

void reactor::run(){ 

 if(rfl==0) return;

  uint8_t i=num_th;
  pth pt;
 pt.rp=this;
   while(i>0){
    pt.index=--i;
 std::thread(prl, pt).detach();

    };
 step();
  };

void reactor::stop(){
 uint8_t i=num_th;
  while(i>0){ mp_p[--i].fl=1; mp_p[i].cv.notify_one(); };
 i=1;
 while(i){ usleep(10); i=num_th;
 while(i>0){ if(mp_p[--i].run){ i=1; break; };
  };
 };
}; 
   
void reactor::step(void){
 
 uint8_t i=0;
 uint64_t val, tmp;
 /* 
std::fstream f("storage.file", std::fstream::in);

if(!f.good()){ std::cout<<"Can't read storage file\n"; stop(); this->~reactor(); return; };

while(!f.eof()){hhlkkkiiibhhkk...
 f >> val;
if(f.fail()) break;*/

  while(!std::cin.eof() && std::cin.good()){

  std::cin>>val;

  if(std::cin.fail()) break; 
 if(i==num_th) i=0; 

  mp_p[i].mt.lock();

 mp_p[i].que.push(val);

 mp_p[i].mt.unlock();
 mp_p[i].cv.notify_one();
++i;
 };

 stop(); 
i=num_th;
map_t::iterator it;
  do{ --i;
      it=mp_p[i].mp.begin(); 

      while(it!=mp_p[i].mp.end()){
      
         val=it->first; tmp=it->second;

        if(!res.insert(pair(val, tmp)).second){ if( tmp > res[val] ){ res[val]=tmp; }; }; it++; 

       }

     }while(i>0);

  using boost::property_tree::ptree;
  ptree ptr;

   it=res.begin();
  i=1;
 std::fstream fs("res.xml", std::fstream::out);
 
 while( it!=res.end()){ 
 
  ptree& nd=ptr.add("value", i++); 
  nd.put("number", it->first);

  nd.put("degree", it->second);
 it++;
};

write_xml(fs, ptr);

 std::cout<<"Result writed to file res.xml in current directory\n"; 
fs.close();
  };

void reactor::th(uint8_t a){
 
  pair pr;
 map_t::iterator it;
  uint64_t val, s;
 mp_t& mp=mp_p[a];
 mp.run=1; mp.fl=0;
 uint64_t count;

  while(1){

count=1; s=2;

 try{
  
 val=mp.get();
}catch(int c){ break; };

if(val==1){ std::cout<<"hey it's not fair, value must be greater then 1\n"; continue; };
  if(val%s==0) continue; 

 while(s<val) { if(val%s){ ++s; continue; };
 
   break;  };
 
while( val > s){  if( val%s){ count=0; break; }; val/=s; ++count;  }

 if(count==0 || val < s ){  continue; };

   if((it=mp.mp.find(s)) != mp.mp.end()){ if((it->second)<count){ it->second=count; }; continue; };
  
   mp.mp.insert(pair(s, count));
  
   };
 };

void reactor::prl(pth p){
    p.rp->th(p.index);  
 };

  int main(){

   reactor r;
    r.run();

   };


