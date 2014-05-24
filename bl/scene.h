
#include <models.h>

  class scene{

 typedef std::list<std::shared_ptr<model> >  Sc;

public:
 scene(){};
 scene(const char*);
 ~scene(){};
void draw();
void add_model(const char*);
private:
template <typename P=parser >
char* parse(char* a,const char* b, char* c){P p; return p.parse(a, b, c); };
 Sc sc;
 };


