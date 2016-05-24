
#include <sender.hpp>

int main()
  {
  
 typedef sender<sox_packet> sender_t;
 
  sender_t snd(sender_t::interactive_tag);
  snd.thread();
 
};
 
