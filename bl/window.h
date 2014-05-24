
#include <scene.h>

class window{


 public:

 struct shape{

 XRectangle* r;
 size_t num;
 const char* fone;
 
  };
window();
window(window*, int, int, int, int, shape*);
virtual ~window(){
  
   XDestroyWindow(dp, win);
if(rt) XCloseDisplay(dp);

   };

Display* dp;
Window win;
Window root;
window* button;
void image_load(const char*);
window* add_window(int, int, int, int, shape*);
virtual Cursor cursor();
virtual void create(int, int, int, int, shape*);
protected:
 window* prt;
 std::list<std::shared_ptr<window> > lst;
 XSetWindowAttributes attr;
 XFontStruct* font;
 GC gc;
 XGCValues gv;
 XColor col1, col2;
 XEvent e;
 uint8_t rt;
};

class glwindow: public window{
public:

glwindow(window* wp=NULL, int x=0, int y=0, int w=1400, int h=1200, shape* r=NULL);

virtual ~glwindow();
void show(scene&);
void create(int, int, int, int, shape*);
private:
GLXContext glc;
};
class glw: public glwindow{
 public:
glw(){};
~glw(){}
};

