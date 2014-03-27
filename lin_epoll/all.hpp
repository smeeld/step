#include <iostream>
extern int num;
   template <class T>
   class all {
     public:
 
 typedef T        value_type;
 typedef T*       pointer;
 typedef const T* const_pointer;
 typedef T&       reference;
 typedef const T& const_reference;
 typedef std::size_t    size_type;
 typedef std::ptrdiff_t difference_type;
 size_type count;
 size_type lim;
 T* mas;
 T** pointer_stack;

  template <class U> struct rebind { typedef all<U> other; };
 
  pointer address (reference value) const { return &value; }
       
  const_pointer address (const_reference value) const { return  &value; }

 all() throw(): count(num),lim(num){

      try { pointer_stack=new T*[sizeof(T*)*num];} catch(std::bad_alloc& a){ exit(-1); };
  
       try{ mas=new T[num]; }catch(std::bad_alloc& a){  exit(-1); };

         for(int x=0;x<num;x++){ pointer_stack[x]= &mas[x]; } 
       };
        
          all(const all& s) throw():count(s.count) {
             pointer_stack=s.pointer_stack; mas=s.mas; };
       template <class U>  all (const all<U>& s) throw():count(s.count){};
        

       ~all() throw() { delete[] pointer_stack; delete[] mas; };
       size_t  max_size () const throw() { return 1; }

   inline  pointer allocate (size_type n, const void* = 0) {
       pointer p; if((count-=n)>0){ p=pointer_stack[count];  }else{ p=NULL; }; return p; };

    inline  void construct (pointer p, T& t) {  new (p) T(t); };
   
   template <typename... ARG> inline  void construct (pointer p, ARG... arg) {  new (p) T(arg...); };
    
    inline  void destroy (pointer p) {};

 inline   void deallocate (pointer p, size_type num) { 
            
          for(int x=0;x<num;x++){  pointer_stack[count++]=static_cast<T*>(p+x); }; };  
 
    };


