#include <stdio.h>
#include <usb.h>
#include <stdlib.h>
#include <string.h>
struct usb_dev_handle* hand;

char message2[]={0x55, 0x53, 0x42, 0x43, 0x12, 0x34, 0x56, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
char message[]={0x55, 0x53, 0x42, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

char message3[]={0x55, 0x53, 0x42, 0x43, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x06, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
int main(){
struct usb_bus* bus;
struct usb_device* dev;

usb_init();
usb_find_busses();
usb_find_devices();
bus=usb_get_busses();
 if(bus==NULL){ exit(0); };
  int i=0;int j=0; int s=0;int c;
 for(;bus; bus=bus->next){
   printf("NUM BUS=%d\n",i);
  for(dev=bus->devices; dev; dev=dev->next){
    printf("NUM DEV=%d IN BUS==%d\n",j,i);
      if(dev->descriptor.idVendor==0x12d1 || dev->descriptor.idProduct==0x14fe)
        { printf("FIND IN BUS==%d DEV %d\n",i,j); s=1; break; };
         j++; }; if(s){ break; }; i++; };
 
    
        printf("DEVICE CONFIGURATION=========\n");
    for(i=0;i<dev->config->bNumInterfaces;i++){

    printf("INTERFACES==%d INTERFACE NUM==%d \n",dev->config->bNumInterfaces,i); 
          printf("POWER %d mA \n",dev->config->MaxPower);
             
          for(j=0;j< dev->config->interface[i].num_altsetting; j++){
                 
               printf("INTERFACE ALSETTINGS==%d INTERFACE ALSETTINGS INTERFACE DESC NUM==%d \n",dev->config->interface[i].num_altsetting ,j); 
                   for(c=0;c<dev->config->interface[i].altsetting[j].bNumEndpoints;c++){
                  printf("ENDPOINTS==%d ENDPOINT NUM==%d \n",dev->config->interface[i].altsetting[j].bNumEndpoints,c);
                    printf("INTERFACE CLASS==%x\n",dev->config->interface[i].altsetting[j].bInterfaceClass);
            
int ad=dev->config->interface[i].altsetting[j].endpoint[c].bEndpointAddress;
 if((ad>>4)==0){
      

  };
     
      printf("DESC TYPE==%d\n",dev->config->interface[i].altsetting[j].endpoint[c].bDescriptorType);
      printf("CLASS CODE==%d\n",dev->config->interface[i].altsetting[j].endpoint[c].wMaxPacketSize);
      printf("INTERVAL==%d\n",dev->config->interface[i].altsetting[j].endpoint[c].bInterval);
      };
    };
   };
if( (hand=usb_open(dev))==NULL){ printf("ERROR OPEN DEV\n"); exit(0); };
  
 if(usb_detach_kernel_driver_np(hand, 0)<0){  printf("ERROR DETACH\n"); exit(0); }; 
   printf("SUCCES DETACHED\n");
usb_claim_interface(hand,0);
  
    if(usb_control_msg(hand, USB_TYPE_STANDARD | USB_RECIP_DEVICE, USB_REQ_SET_FEATURE,00000001,0,0,0,0)<0){ printf("ERROR SEND MESSAGE\n"); }else{ printf("OK\n"); };
   
   if((i=usb_bulk_write(hand, 0x01,message3,31,0))<0){ printf("NO SUCCES\n"); }else{printf("SUCCESS SEND %d\n",i); };


 usb_close(hand);
   };


