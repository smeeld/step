#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
static MX_Music ms=NULL;

int main(int argv, char* s[]){

SDL_Init(SDL_INIT_EVERYTHING);

 if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) { exit(-1); };

if((ms=MIX_LoadMUS(s[1]))==NULL){ printf("Error load mus\n"); exit(-1); }; 


Mix_PlayMusic( ms, -1 ); 

Mix_FreeMusic( music );

Mix_CloseAudio();

SDL_Quit()p;
