#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <chrono>
using namespace std;

int pos;
 vector<int> m;


int part(int len, int p){
  int l=0;
  int r=len;
     int mid=m[pos+p];

     while(l!=r){

     while(mid>m[pos+l]){ l++; if(l==r){ return l;}; };


        do{ r--; if(l==r){ return l;}; }while(m[pos+r]>mid);

           swap(m[pos+l],m[pos+r]);
     l++;
    };
  return l;
  };

 
  void sort(int len){
  int j=0;
   int l=0;
  switch(len){
   case 1 : pos++; return;
   case 2 : if(m[pos]>m[pos+1]){ swap(m[pos], m[pos+1]); }; pos+=2; return;  
   default:
          
     while(l==0){ if(j==len){ pos+=len; return; }; 
  
                   l=part(len, j); j++;
                   }; break;
               };
 
  sort(l);
   sort(len-l);

    return;
  };

 int main(int arg, char * s[]){
  
 int num=stoi(s[1]);
 minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
   m.resize(num);

 generate(m.begin(), m.end(), gen);
 
pos=0;
 sort(num);

 for_each(m.begin() ,m.begin()+stoi(string(s[2])), [](int i){ cout<<"NUM=="<<i<<endl; });
 
   };
