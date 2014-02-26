#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <chrono>
using namespace std;

 vector<int> m;

 typedef vector<int>::iterator itr;

 itr it, itmpm;

 bool cmp(int a){ return a < *itmpm; };

  void sort(itr r){
  
   itr itmp;

  
   if(r==(it+2)){
   if(*it>*(it+1)){ swap(*it, *(it+1)); }; it+=2; return;  
  };
  if((it+1)==r){ it++; return; };

    itmp=it;
   itmpm=it;

   while(itmp==it){ if(itmpm==r){ it=itmpm; return; };
        
  itmp=partition(it, r, cmp);
    
       itmpm++;
    
      };
 
  sort(itmp);
   sort(r);

    return;
  };

 int main(int arg, char * s[]){
  
  m.resize(stoi(string(s[1])));

minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
   
 generate(m.begin(), m.end(), gen); 

 it=m.begin();

 sort(m.end());

 for_each(m.begin(),m.begin()+stoi(string(s[2])), [](int i){ cout<<"NUM=="<<i<<endl; });
 
   };
