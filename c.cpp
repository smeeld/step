#include <iostream>
#include <string>
#include <vector>
#include <iterator>


template<typename Pos, typename  Cmp> 
  
  void srt(Pos pt, Pos pm, Cmp cmp){
  
   Pos tmp=pt, md=pt, sp;
   
   do{
       if(md==pm)  return;
         sp=pm;
          while(sp!=tmp){
          if(cmp(*tmp, *md)){ ++tmp; continue; }; 
              
          while(--sp!=tmp){   
          if(cmp(*md, *sp)) continue;
          std::swap(*tmp, *sp);
         ++tmp; break;
         };
       };
     ++md;
    }while(tmp==pt);
      srt(pt, tmp, cmp);
      srt(tmp, pm, cmp);
 };

template<typename Pos> 
  
  void srt(Pos pt, Pos pm){
  
   Pos tmp=pt, md=pt, sp;
   
   do{
       if(md==pm)  return;
         sp=pm;       
         while(sp!=tmp){
          if(cmp(*tmp, *md)){ ++tmp; continue; }; 
              
          while(--sp!=tmp){   
          
          if(cmp(*md, *sp)) continue;
          std::swap(*tmp, *sp);
         ++tmp; break;
         };
       };
     ++md;
    }while(tmp==pt);
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
