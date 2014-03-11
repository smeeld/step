#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <chrono>
using namespace std;

typedef vector<int>::iterator it;

it part(it fr, it lst, it pos){
  

     while(*fr!=*lst){

     while(*pos>*fr){ ++fr; if(*fr==*lst){ return fr; }; };


        do{ --lst; if(*fr==*lst){ return fr;}; }while(*lst>*pos);

           swap(*fr, *lst);
     ++fr;
    };
  return fr;
  };

 
  void sort(it fr, it lst){ 
  int j=0;
  it tmp;
   if(fr==lst){ return; };
   if((fr+1)==lst){ if(*fr>*lst){ swap(*fr, *lst); }; return; };
  
         tmp=fr; 
     while(tmp==fr){ if((fr+j)==lst){ return; }; 
                   
                   tmp=part(fr, lst, fr+j); j++;
                   };
             
  sort(fr, tmp);
   sort(tmp, lst);

    return;
  };

 int main(int arg, char * s[]){
  
 int num=stoi(s[1]);
 ranlux24 gen(chrono::system_clock::now().time_since_epoch().count());
   
   vector<int> m(num);
  generate(m.begin(), m.end(), gen);
  for_each(m.begin(),m.end(),[](int i){ return i*2;});
 sort(m.begin(),m.end());

 for_each(m.begin() ,m.begin()+stoi(string(s[2])), [](int i){ cout<<"NUM=="<<i<<endl; });
 
   };
