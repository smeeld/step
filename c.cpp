#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <chrono>
using namespace std;
 vector<int> m;
 vector<int> tmp;
 vector<int>::iterator it;
 int pos;
  void sort(int len){
   int i;
   int j=0;
   int l=0;
   int r;
   switch(len){
  case 2 : if(*it>*(it+1)){ swap(*it, *(it+1)); }; it+=2; return;  
  
  case 1 : it++; return;

  default : 
            
   while(l==0){ if(j==len){ it+=len; return; };
        r=1;
  for(i=0;i<len;i++){ 
   
   if(*(it+i)<*(it+j)){ tmp[l]=*(it+i); l++; }else{ tmp[len-r]=*(it+i); r++; };
    
     }; j++;
    };
  move(tmp.begin(), tmp.begin()+len, it); break;
   };
  sort(l);
   sort(r-1);

    return;
  };

 int main(int arg, char * s[]){
  int i;
  int num=stoi(string(s[1]));
  int out=stoi(string(s[2]));
  
  tmp.resize(num);

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
 
minstd_rand0 gen (seed); 
 for(i=0;i<num;i++)
  {
   m.push_back(gen());
    };

 it=m.begin();

 sort(num);

for_each(m.begin(),m.begin()+out, [](int i){ cout<<"NUM=="<<i<<endl; });
 
  
  };
