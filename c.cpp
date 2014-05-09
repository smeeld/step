#include <iostream>
#include <string>
#include <vector>
#include <iterator>


template<typename Pos, typename  Cmp> 
  
inline  void srt(Pos pt, Pos pm, Cmp cmp){
  
   Pos tmp=pt, md=pt, sp;
   int n=0;
   while(tmp==pt){ if(md==pm){  return; };
         
           sp=pm;
          
         while(sp!=tmp){
         
     while(cmp(*tmp, *md)){ ++tmp; if(tmp==sp){ n=1; break; }; };
           if(n){ break; };
     do{ --sp; if(tmp==sp){ n=1; break; }; }while(cmp(*md, *sp));
              if(n){ break; };
           std::swap(*tmp, *sp);
     ++tmp;
    };  ++md;
  };
   
      srt(pt, tmp, cmp);
      srt(tmp, pm, cmp);
 };

template<typename Pos> 
  
  void srt(Pos pt, Pos pm){
  
   Pos tmp=pt, md=pt, sp, lim=pm;
   int n=0;
   
   while(tmp==pt){ if(md==lim){  return; };
         
           sp=pm;
          
         while(sp!=tmp){
         
     while(*tmp < *md){ ++tmp; if(tmp==sp){ n=1; break; }; };
           if(n){ break; };
     do{ --sp; if(tmp==sp){ n=1; break; }; }while(*md < *sp);
              if(n){ break; };
           std::swap(*tmp, *sp);
     ++tmp;
     };  ++md;
    };
 srt(pt, tmp);
 srt(tmp, pm);
 };


int cmp(int a, int b){ return a<b; }; 
int main(int a , char** s){

 std::vector<int> v;
 int sz=std::stoi(s[1]);
 while(--sz>0){

 v.push_back(random());
};
srt(v.begin(), v.end(), cmp);
 std::copy(v.begin(), v.begin()+std::stoi(s[2]), std::ostream_iterator<int>(std::cout, "\n"));
};
