#include <base.h>
#define TP_WIN 1
#define TP_TEX 2
#define GL_WIN 3
#define SIM_WIN 4

struct image{

struct _error_mgr {

  struct jpeg_error_mgr pub;

  jmp_buf setjmp_buffer;
};

typedef struct _error_mgr * _error_ptr;

image(){ buf=NULL; };
image(unsigned char* p):buf(p){};
image(const image&)=delete;
~image(){ free(buf); };

METHODDEF(void)
_error_exit (j_common_ptr cinfo)
{
  _error_ptr err = (_error_ptr) cinfo->err;

  (*cinfo->err->output_message) (cinfo);

  longjmp(err->setjmp_buffer, 1);
}

 int w;
 int h;
 unsigned char* buf;
int ret_image(const char*, int);
 };



