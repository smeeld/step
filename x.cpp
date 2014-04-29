#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <vector>
#include <queue>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

  class reactor{

typedef  std::map<uint64_t, uint64_t> mp;
typedef std::pair<uint64_t, uint64_t> pair;
 
 public:
 reactor();
 ~reactor(){ delete [] mp_p; }; 

private:
   uint8_t num_th;
   uint8_t run;
omp_lock_t mt;
mp res;
std::queue<uint64_t> que;
mp* mp_p;
void th(uint8_t);
void step(void);
void prl(void);
 };

reactor::reactor(){ 
 
int i;

  omp_init_lock(&mt);
  
 i=omp_get_max_threads();
 
  num_th=0;

try{ 

  mp_p=new mp[i-1];

  }catch(std::bad_alloc& c){ exit(-1); };
 
#pragma omp parallel
 { 

 prl();

  }

 };

void reactor::step(void){
 
 uint8_t i;
 uint64_t val, tmp;
 mp::iterator it;
std::fstream f("storage.file", std::fstream::in);

if(!f.good()){ std::cout<<"Can't read storage file\n"; run=0; this->~reactor(); exit(-1); };

while(!f.eof()){

/*
  while(!std::cin.eof() && std::cin.good()){

 
  std::cin>>val;
 
  if(std::cin.fail()) break; 
*/
  f >> val;
if(f.fail()) break; 
  omp_set_lock(&mt);

 que.push(val);

 omp_unset_lock(&mt);

 };
f.close();

 while(!que.empty()){ usleep(10); };
 
 i=num_th-1;
 run=0;

 while( num_th>1 ){  usleep(10); };
 
  do{ --i; 

     it=mp_p[i].begin(); 

      while(it!=mp_p[i].end()){
      
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
 
 uint8_t n=a-1;
  pair pr;
 mp::iterator it;
  uint64_t val, s;

 uint64_t count;

  while(run){

count=1; s=2;

 omp_set_lock(&mt);

  if(que.empty()){ omp_unset_lock(&mt); usleep(10); continue; };

 val=que.front(); que.pop(); omp_unset_lock(&mt);
if(val==1){ std::cout<<"hey it's not fair, value must be greater then 1\n"; continue; };
  if(val%s==0){ continue; }; 

 while(s<val) { if(val%s){ ++s; continue; };
 
   break;  };
 
while( val > s){  if( val%s){ count=0; break; }; val/=s; ++count;  }

 if(count==0 || val < s ){  continue; };

   if((it=mp_p[n].find(s)) != mp_p[n].end()){ if((it->second)<count){ it->second=count; }; continue; };
  
   mp_p[n].insert(pair(s, count));
  
   };

omp_set_lock(&mt);

--num_th;

omp_unset_lock(&mt);

 };

void reactor::prl(void){

  omp_set_lock(&mt);
 uint8_t n=num_th++;
 omp_unset_lock(&mt);

 if(n>0){ th(n); }else{ step(); };

 };


 

  int main(){

   reactor();

   };


