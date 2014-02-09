#include "stdio.h"
#include "stdlib.h"
#include "string.h"
double step();
int len;
double divel(double);
double mull(double);
double sym();
char* pos;
char* gen_str;
double sym(){
int count=0;
char* temp;
if(*pos=='('){
pos++;
return step();};
char* str=(char*)malloc(sizeof(char));
while((*pos!='+') && (*pos!='/') && (*pos!='*') && (*pos!='-') && (*pos!='\0') && (*pos!=')')){
temp=(char*)realloc((char*)str,sizeof(char)*(count+1));if(temp==NULL){printf("UNCNOWN ERROR\n");exit(-1);};
str=temp;
str[count]=*pos;
count++;
if(*pos!=')'){pos++;};
};
temp=(char*)realloc((char*)str,sizeof(char)*(count+1));if(temp==NULL){printf("UNCNOWN ERROR\n");exit(-1);};
str=temp;
str[count]='\0';
double db=atof(str);
free(str);
return db;
};
double step(){
double prev=0;
double temp=0;
char* s;
while(pos-gen_str<len && *pos!=')'){
s=pos++;
switch(*s){
case '+':temp=sym();if(*pos=='*'){pos++;prev+=mull(temp);break;};if(*pos=='/'){pos++;prev+=divel(temp);break;};prev+=temp;break;
case '-':temp=sym();if(*pos=='*'){pos++;prev-=mull(temp);break;};if(*pos=='/'){pos++;prev-=divel(temp);break;};prev-=temp;break;
case '*': prev*=sym();break;
case '/':  prev/=sym();break;
default: if(*s==')'){break;}else{pos--;prev=sym();break;};
   };
  };
pos++;
 return prev;
 };
double mull(double temp){
double middle=sym();
char* s=pos++;
switch(*s){
case '*':  return temp*mull(middle);
case '/':  return temp*divel(middle);
default: pos--; return temp*middle;
 };
};
double divel(double temp){
double middle=sym();
char * s=pos++;
switch(*s){
case '*':  return temp*mull(middle);
case '/':  return temp*divel(middle);
default: pos--;return temp/middle;
  };
}
int main(int c,char* s[]){
gen_str=s[1];
len=strlen(s[1]);
pos=gen_str;
printf("RES=%f\n",step());
}



