#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* m;
typedef unsigned in;
in pos;

  inline int part(in len, in p){
  in l=0;
  in r=len;
  in mdl;
     int mid=m[pos+p];

     while(l!=r){

     while(mid>m[pos+l]){ l++; if(l==r){ return l;}; };


        do{ r--; if(l==r){ return l;}; }while(m[pos+r]>mid);

           mdl=m[pos+l]; m[pos+l]=m[pos+r]; m[pos+r]=mdl; 
     l++;
    };
  return l;
  };


  void srt(in len){

   in j;
   in l;


   switch(len){
  case 2 : if(m[pos+1]<m[pos]){ l=m[pos]; m[pos]=m[pos+1]; m[pos+1]=l; }; pos+=2; return;  
  
  case 1 : pos++; return;

  default : 
         l=0; j=0;   
   while(l==0){ if(j==len){ pos+=len; return; };
         

              l=part(len,j); j++;
       }; break;
    };
  
  srt(l);
   srt(len-l);

    return;
  };

 int main(int arg, char * s[]){
  int i;
  in num=atoi(s[1]);
  in out=atoi(s[2]);
 pos=0;
   m=malloc(sizeof(int)*num);
  
 for(i=0;i<num;i++)
  {
   m[i]=random();
    };

 srt(num);
 
for(i=0;i<out;i++)
 {
  printf("NUM==%d\n",m[i]);
  };

 free(m);
  };
