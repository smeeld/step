#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
int* m;
int p[1024];
int* tmp;
int pos;
int left;
int i;
int j;
int l;
 int r;
int count;
void sort(){
int i;
int j=0;
int l=0;
int mid;
 int r;
if(left>2){
 while(l==0){ if(j==left){ pos+=left; left=p[--count]; return; };
        r=1; i=left;
  for(i=0;i<left;i++){ 
   if(m[pos+i]<m[pos+j]){ tmp[l]=m[pos+i]; l++; }else{ tmp[left-r]=m[pos+i]; r++; };
    }; j++;
  };
memcpy(&m[pos],tmp,left*sizeof(int));
 p[count++]=r-1;
 left=l;
  };
  if(left==2){ if(m[pos]>m[pos+1]){ mid=m[pos]; m[pos]=m[pos+1]; m[pos+1]=mid; }; pos+=2; left=p[--count]; return; }; 
  if(left==1){pos++; left=p[--count]; return; };

return;
 };
int main(int arg, char * s[]){
int i;
int num=atoi(s[1]);
int out=atoi(s[2]);
pos=0;
count=1;
m=malloc(sizeof(int)*num);
tmp=malloc(sizeof(int)*num);
for(i=0;i<num;i++){
 m[i]=(int)random();
 };
 p[0]=num;
left=p[0];
while(pos<num){
 sort();
  };
for(i=0;i<out;i++)
 {
  printf("NUM==%d\n",m[i]);
  };
free(tmp);
 free(m);
 };
