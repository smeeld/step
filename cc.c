#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int* m;
int* tmp;
int pos;
int len;
int num;
  void sort(){
   int i;
   int j=0;
   int l=0;
   int mid;
   int r;
 int stack[1024];
 int count=0;
  while (pos<num){ 
   switch(len){
 case 2 : if(m[pos]>m[pos+1]){ mid=m[pos]; m[pos]=m[pos+1]; m[pos+1]=mid; }; len=stack[count--]; pos+=2; break; 
  
  case 1 : pos++; len=stack[count--]; break;

  default :
       l=0; j=0;     
   while(l==0){ if(j==len){ pos+=len; len=stack[count--]; break; };
        r=1;
  for(i=0;i<len;i++){ 
   /*printf("LEF=%d RR=%d LEN=%d POS=%d MPOS=%d COUNT=%d\n",l,r,len,pos,m[pos],count);*/
   if(m[pos+i]<m[pos+j]){ tmp[l]=m[pos+i]; l++; }else{ tmp[len-r]=m[pos+i]; r++; };
    
     }; j++;
    };
  memcpy(&m[pos],tmp,len*4);
   len=l;
    stack[++count]=r-1;
  break;
   };
  };
 };
 

 int main(int arg, char * s[]){
  int i;
  num=atoi(s[1]);
  int out=atoi(s[2]);
 pos=0;
   m=malloc(sizeof(int)*num);
  
  tmp=malloc(sizeof(int)*num);
 
 for(i=0;i<num;i++)
  {
   m[i]=random();
    };
 len=num;
 sort();
 
for(i=0;i<out;i++)
 {
  printf("NUM==%d\n",m[i]);
  };
free(tmp);
 free(m);
  };
