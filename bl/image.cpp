#include <image.h>

int image::ret_image(const char* file, int type){

  int  hi, wi, count;

 unsigned char* buf1; unsigned char* buf2;
  struct jpeg_decompress_struct cinfo;
  
  struct _error_mgr jerr;

  FILE * infile;
  JSAMPARRAY buffer;
  int row_len;

  if ((infile = fopen(file, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", file);
    return -1;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = _error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
   
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -1;
  }
 
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, infile);

  (void) jpeg_read_header(&cinfo, TRUE);

  (void) jpeg_start_decompress(&cinfo);
  
  wi=cinfo.output_width;

  hi=cinfo.output_height;
  count=0;
  row_len = wi * cinfo.output_components;
 
 
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_len, 1);

  if((buf1=(unsigned char*)malloc(sizeof(unsigned char)*row_len* hi))==NULL){ fclose(infile); return -1; }; 
   
  if((buf2=(unsigned char*)malloc(sizeof(unsigned char)*wi * 4 * hi))==NULL){ free(buf1); fclose(infile); return -1; }; 

  while (cinfo.output_scanline < cinfo.output_height) {
   
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    
    memcpy(&buf1[count], buffer[0], row_len);

   count+=row_len;
 
   };

  (void) jpeg_finish_decompress(&cinfo);
 
  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

 int y, x=0;

 switch(type){

case TP_WIN:
for(y=0; y<wi*hi*3; y+=3){

 buf2[x]=buf1[y+2]; buf2[x+1]=buf1[y+1];buf2[x+2]=buf1[y]; buf2[x+3]=0xff; x+=4; }; break;
case TP_TEX:
for(y=0; y<wi*hi*3; y+=3){

 buf2[x]=buf1[y]; buf2[x+1]=buf1[y+1];buf2[x+2]=buf1[y+2]; buf2[x+3]=0xff; x+=4; }; break;
default : break;
   };
  buf=buf2;
  h=hi;
 w=wi;

  free(buf1);

 return 0;

 };


