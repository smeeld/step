#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

int cmp(const void* v1, const void* v2){  return strcmp(*(char* const *)v1, *(char* const *)v2) < 0 ? 1 : 0;  };

void fill(char* g, int s){  do{ g[--s]=' '; }while(s>0); };

char** rel(char** s, size_t old, size_t new){
 
 char** p=malloc(sizeof(char*)*new);
 
  if(p==NULL){ return NULL; };

  memcpy(p, s, sizeof(char*)*old);

  free(s);
 
 return  p; }; 

int main(int c, char** s){

DIR* dp;
struct stat st;
struct dirent* fl;
struct passwd* pws;
struct group* grp;
char buf[90]; 
char** fp;
char**  fn;
char* tmp;
char* end;
unsigned num=0, max=10;
unsigned count=1024;
unsigned gen=0;
unsigned b_len=count*64; 
size_t len;
buf[89]='\0';
 buf[3]=' '; buf[7]=' '; 

 while((len=getopt(c, s, "l:"))!=-1){

 switch(len){

 case 'l' :

  if(chdir(optarg)!=0){ printf("Non such directory\n"); exit(-1); }; break;

 default : printf("Usage prog -l /path_to_dir\n"); exit(-1);

 };
};

  
if((dp=opendir("."))==NULL){ printf("Error Open Directory\n"); exit(-1); }


  if((fp=malloc(sizeof(char*)*count))==NULL){ printf("Error Allocate Memory\n"); exit(-1); };
  if((fn=malloc(sizeof(char*)*max))==NULL){ printf("Error Allocate Memory\n"); free(fp); exit(-1); };
  if((fn[num++]=malloc(sizeof(char)*b_len))==NULL){ printf("Error Allocate Memory\n"); free(fp); free(fn); exit(-1); };
 
 tmp=fn[0];

 end=fn[0]+b_len;

 while((fl=readdir(dp))!=NULL){ 
  
  if(fl->d_name[0]=='.'){ continue; }; 

 len=strlen(fl->d_name)+1;

if((end-tmp)<len){ 
   
  if(num == max){ 
 
    if((fn= rel(fn, max, max*2))==NULL){ break; };
     max*=2; };

   if((fn[num]=( char*) malloc(sizeof(char) * b_len))==NULL){  break; };
    
     tmp=fn[num];
     end=fn[num]+b_len;
      ++num; 
  };

 if(gen==count){

  if((fp=rel(fp, count, count*2))==NULL){ break; }; count*=2;
  };

  strcpy(tmp, fl->d_name); 
  
  fp[gen++]=tmp;

  tmp+=len;
   
  };

 qsort( fp, gen, sizeof(char*), cmp);

  while(gen>0){
 
 tmp=fp[--gen];


 if(stat(tmp, &st)!=0){ continue; };

  fill(buf, 89);

if((st.st_mode&S_IRUSR)==S_IRUSR){ buf[0]='r';}else{buf[0]='-'; };
if((st.st_mode&S_IWUSR)==S_IWUSR){ buf[1]='w';}else{buf[1]='-'; };
if((st.st_mode&S_IXUSR)==S_IXUSR){ buf[2]='x';}else{buf[2]='-'; };
if((st.st_mode&S_ISUID)==S_ISUID){ buf[2]='s';};
if((st.st_mode&S_IRGRP)==S_IRGRP){ buf[4]='r';}else{buf[4]='-'; };
if((st.st_mode&S_IWGRP)==S_IWGRP){ buf[5]='w';}else{buf[5]='-'; };
if((st.st_mode&S_IXGRP)==S_IXGRP){ buf[6]='x';}else{buf[6]='-'; };
if((st.st_mode&S_IROTH)==S_IROTH){ buf[8]='r';}else{buf[8]='-'; };
if((st.st_mode&S_IWOTH)==S_IWOTH){ buf[9]='w';}else{buf[9]='-'; };
if((st.st_mode&S_IXOTH)==S_IXOTH){ buf[10]='x';}else{buf[10]='-'; };


 sprintf(buf+52,"%d",st.st_size);  *(buf+52+strlen(buf+52))=' ';
 
 sprintf(buf+12,"%d",st.st_nlink); *(buf+12+strlen(buf+12))=' ';

 if((pws=getpwuid(st.st_uid))==NULL){ continue; };
  
 grp=getgrgid(pws->pw_gid);
 
if((len=strlen(pws->pw_name))<16){ strncpy(buf+20, pws->pw_name, len); }else{ strncpy(buf+20, pws->pw_name, 15); };

if((len=strlen(grp->gr_name))<16){ strncpy(buf+36, grp->gr_name, len); }else{ strncpy(buf+36, grp->gr_name, 15); };
 
strncpy(buf+64, ctime(&st.st_mtime), 16); *(buf+64)=' '; *(buf+65)=' '; *(buf+66)=' ';


 if(S_ISDIR(st.st_mode)){ printf(" d"); }else{ printf(" -"); };

  printf(" %s %s\n", buf, tmp );

   };
 
 do{ free(fn[--num]); }while(num>0);

free(fp);

closedir(dp);

 

};
