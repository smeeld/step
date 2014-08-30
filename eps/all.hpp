#include <iostream>

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
 int type;
 size_type count;
 size_type lim;
 T* mas;
 T** pointer_stack;

  template <class U> struct rebind { typedef all<U> other; };
 
  pointer address (reference value) const { return &value; }
       
  const_pointer address (const_reference value) const { return  &value; }

 all(size_t num) throw(): count(num),lim(num){
             type=1;
       pointer_stack=new T*[sizeof(T*)*num]; 
  
       try{ mas=new T[num]; }catch(std::bad_alloc& a){  delete[] pointer_stack; throw std::bad_alloc(); };

         for(int x=0;x<num;x++){ pointer_stack[x]= &mas[x]; } 
       };
        
          all(const all& s) throw():count(s.count), type(0) {
             pointer_stack=s.pointer_stack; mas=s.mas; };

       template <class U>  all (const all<U>& s) throw():count(s.count), type(0){};
        

       ~all() throw() { if(type){ delete[] pointer_stack; delete[] mas; }; };
      
    size_t  max_size () const throw() { return 1; }

   inline  pointer allocate (size_type n, const void* = 0) {
       pointer p; if(n==1){ p=pointer_stack[count];  }else{ p=NULL; }; return p; };

    inline  void construct (pointer p, T& t) {  new (p) T(t); };
   
   template <typename... ARG> inline  void construct (pointer p, ARG... arg) {  new (p) T(arg...); };
    
    inline  void destroy (pointer p) {};

 inline   void deallocate (pointer p, size_type n) { 
            size_type x=n;
          do{  pointer_stack[count++]=static_cast<T*>(p+x); --x; }while(x>0); };  
 
    };


