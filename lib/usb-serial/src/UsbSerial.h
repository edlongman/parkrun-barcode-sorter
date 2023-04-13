#include <libopencm3/usb/usbd.h>

#ifndef _USB_SERIAL_H
#define _USB_SERIAL_H

namespace UsbSerial{
    void reenumerate();
    usbd_device* init();
    bool isConnected();
    void poll();
    bool writeString(const char* str, uint16_t len);
    void pollDelay(unsigned int us);
}

#endif /* _USB_SERIAL_H */
