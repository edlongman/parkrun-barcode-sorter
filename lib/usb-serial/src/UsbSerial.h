#include <libopencm3/usb/usbd.h>

#ifndef _USB_SERIAL_H
#define _USB_SERIAL_H

namespace UsbSerial{
    usbd_device* init();
    bool isConnected();
}

#endif /* _USB_SERIAL_H */
