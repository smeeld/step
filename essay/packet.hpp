
#include <cstdint>

template <size_t N>
struct packet
 {
 char array[N];
 char* ptr;
 size_t msize;
 packet(): ptr(array), msize(N){};

 template <typename T>
  T& get(size_t n)
  {
    if(n<msize) return reinterpret_cast<T&>(*(ptr+n));
   else return reinterpret_cast<T&>(*ptr); 
  };

template <typename T>
  void set(const T& val, size_t n)
  {
   get<T>(n)=val;
  };

char* pointer(){ return ptr; };
size_t max_size(){ return msize; };

 };

struct sox_packet: packet<256>
 {
    enum types{
  HDR_POINT,
  HDR_LINE,
  HDR_ELLIPSE
  };

 uint8_t type()
  { 
    return get<uint8_t>(1);
  };

 void set_type(uint8_t val)
  {
   get<uint8_t>(1)=val;
  };

 uint16_t length()
   {
     return get<uint16_t>(2);
   };

  uint16_t set_length(uint16_t val)
   {
     get<uint16_t>(2)=val;
   };

  uint16_t checksum(){ return get<uint16_t>(6+length()); };
  void set_checksum(uint16_t val){  set(val, 6+length()); };
  
 uint16_t build_checksum()
   {
         uint16_t chk=soh();
         size_t i=1, sz=length()+6;
         while(i<sz)
          chk^=get<uint8_t>(i++);
        return chk;
    };
  
 void make_checksum()
     {
       set_checksum(build_checksum());
     };

  uint8_t soh(){ return get<uint8_t>(0); };
  void set_soh(uint8_t val){  set(val, 0); };
  uint8_t stx(){ return get<uint8_t>(4); };
  void set_stx(uint8_t val){  set(val, 4); };
  uint8_t eot(){ return get<uint8_t>(8+length()); };
  void set_eot(uint8_t val){ set(val, 8+length()); };
   uint8_t etx(){ return get<uint8_t>(5+length()); };
  void set_etx(uint8_t val){  set(val, 5+length()); };
 
 int32_t& operator [](size_t n)
   { 
      return get<int32_t>(4*n+5); 
   };

  void  make_common(uint8_t type)
    { 
       uint8_t code; uint16_t ln;
        switch(type)
        {
         case HDR_POINT: code=0x41;   ln=8;  break;
         case HDR_LINE: code=0x42;  ln=16; break;
         case HDR_ELLIPSE: code=0x43; ln=24; break;
        }; 
         set_type(code);
         set_length(ln);
         set_soh(0x01);
         set_stx(0x02);
         set_eot(0x04);
         set_etx(0x03);
     };
template <typename U>
 void make_from_istream(uint8_t type, U& ist)
  {
     
    make_common(type);  
  int cnt=length()/4, i=0;
   while(i<cnt) ist>>operator[](i++);
   make_checksum();
  };

  bool good()
  { 
   
      if((size()<max_size())
          && (soh()==0x01)
           && (stx()==0x02)
             && (eot()==0x04) 
               && (etx()==0x03)
                && (build_checksum()==checksum()))
        return 1;
     else return 0;
 };


size_t size(){ return length()+9; };


};
