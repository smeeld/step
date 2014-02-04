#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int* m;
int* tmp;
int pos;
  void sort(int len){
   int i;
   int j=0;
   int l=0;
   int mid;
   int r;
   switch(len){
  case 2 : if(m[pos]>m[pos+1]){ mid=m[pos]; m[pos]=m[pos+1]; m[pos+1]=mid; }; pos+=2; return;  
  
  case 1 : pos++; return;

  default : 
            
   while(l==0){ if(j==len){ pos+=len; return; };
        r=1;
  for(i=0;i<len;i++){ 
   
   if(m[pos+i]<m[pos+j]){ tmp[l]=m[pos+i]; l++; }else{ tmp[len-r]=m[pos+i]; r++; };
    
     }; j++;
    };
  memcpy(&m[pos],tmp,len*4); break;
   };
  sort(l);
   sort(r-1);

    return;
  };

 int main(int arg, char * s[]){
  int i;
  int num=atoi(s[1]);
  int out=atoi(s[2]);
 pos=0;
   m=malloc(sizeof(int)*num);
  
  tmp=malloc(sizeof(int)*num);
 
 for(i=0;i<num;i++)
  {
   m[i]=random();
    };

 sort(num);
 
for(i=0;i<out;i++)
 {
  printf("NUM==%d\n",m[i]);
  };
free(tmp);
 free(m);
  };
