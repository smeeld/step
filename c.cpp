#include <iostream>
#include <string.h>

template<typename Pos, typename Cmp> 
  inline Pos prt(Pos fr, Pos md, Pos lt, Cmp cmp){
   
     while(lt!=fr){

     while(cmp(*fr, *md)){ ++fr; if(lt==fr){ return fr; }; };

        do{ --lt; if(lt==fr){ return fr; }; }while(cmp(*md, *lt));

         std::swap(*fr, *lt);
     ++fr;
    };
  return fr;
  };

template<typename Pos, typename  Cmp> 
  
inline  void sort(Pos pt, Pos pm, Cmp cmp){
  
   Pos tmp, m;
  if((pt+1)==pm){ 
   if( cmp( *pm , *pt) ){  std::swap(*pt, *pm); }; return; }; 
  
  if(pt==pm)  return;

         m=pt;  
        tmp=pt;
       
   while(tmp==pt){ if(m==pm){  return; };
         
     tmp=prt(pt, m++, pm, cmp); 
       
      }; 
  
      sort(pt, tmp, cmp);
      sort(tmp, pm, cmp);

   };
 

