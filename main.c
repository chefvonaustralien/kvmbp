//
//  main.c
//  kvmbp .. on the 31.12.2015 .. happy new year
//
//  because non-working KVM hotkeys are a pain in the ass.
//

#include <stdio.h>
#include <libusb-1.0/libusb.h>

/************** ADJUST THOSE TO YOUR NEEDS **************/

// KVM SWITCH IDs (Digitus DS-11900 / Omnidirectional Control Technology Inc.)
#define KVM_VENDOR_ID   0x0b39
#define KVM_PRODUCT_ID  0x0003

/*
 
 3 bits for Keyboard functionality/LEDs:
 
 SCROLL-LOCK     CAPS-LOCK   NUMPAD
 0b      0               0           0       = 0x00         // -
 0b      0               0           1       = 0x01         // NUMPAD
 0b      0               1           0       = 0x02         // CAPS-LOCK
 0b      0               1           1       = 0x03         // CAPS-LOCK+NUMPAD
 0b      1               0           0       = 0x04         // SCROLL-LOCK
 0b      1               0           1       = 0x05         // SCROLL-LOCK+NUMPAD
 0b      1               1           0       = 0x06         // SCROLL-LOCK+CAPS-LOCK
 0b      1               1           1       = 0x07         // SCROLL-LOCK+CAPS-LOCK+NUMPAD
 
 */

#define CMD_ON 0x05     // SCROLL-LOCK+NUMPAD
#define CMD_OFF 0x01    // NUMPAD

/********************************************************/

int usleep(unsigned int usecs);

int main(int argc, const char * argv[]) {
    
    libusb_context *context;
    libusb_init(&context);
    
    printf("[INF]\tdevice settings:\n");
    printf("[INF]\t----------------\n");
    printf("[INF]\tKVM: \tVID 0x%04x\n", KVM_VENDOR_ID);
    printf("[INF]\tKVM: \tPID 0x%04x\n", KVM_PRODUCT_ID);
    
    printf("[INF]\tsearching for device..\n");
    
   
    libusb_device_handle *dev_handle = libusb_open_device_with_vid_pid(context, KVM_VENDOR_ID, KVM_PRODUCT_ID);
    
    
    if(dev_handle == NULL)
    {
        printf("[ERR]\tunable to open device... check VID/PID?? (kvm switch)\n");
        return -1;
    }
    
    unsigned char PACK_ON[]     =   {CMD_ON};
    unsigned char PACK_OFF[]    =   {CMD_OFF};

    /*
     
     HID Update --> CONTROL TRANSFER (no bulk)
     
     request_type   = 0x21 (HID request)
     request        = 0x09 (HID update; host -> client)
     value          = 0x200 (don't care... default)
     index          = 0x0   (default)
     data           = PACK_ON/PACK_OFF  (set LEDs / functionality)
     length         = 0x01
     timeout        = 1000
     
     */
    
    printf("[INF]\tperforming magic\n");
    libusb_control_transfer(dev_handle, 0x21, 0x9, 0x200, 0x0, PACK_ON, 0x1, 1000);
    libusb_control_transfer(dev_handle, 0x21, 0x9, 0x200, 0x0, PACK_OFF, 0x1, 1000);

    printf("[INF]\twaiting .1 secs for timeout\n");
    usleep(1000 * 100);
    
    // Send packets again because 'somehow' it doesn't work when you only send them once..??
    libusb_control_transfer(dev_handle, 0x21, 0x9, 0x200, 0x0, PACK_ON, 0x1, 1000);
    libusb_control_transfer(dev_handle, 0x21, 0x9, 0x200, 0x0, PACK_OFF, 0x1, 1000);
    
    printf("[INF]\tcleanup\n");
    libusb_close(dev_handle);
    
    libusb_exit(context);

    return 0;
}
