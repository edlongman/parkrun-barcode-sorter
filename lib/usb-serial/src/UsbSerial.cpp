#include <stdlib.h>
#include "UsbSerial.h"
#include "UsbSerialDeviceInfo.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

namespace UsbSerial{

usbd_device *usbd_dev;

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];
bool usb_setup = false;
static enum usbd_request_return_codes cdcacm_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		union char_cdc_union
		{
			char buf[10];
			struct usb_cdc_notification cdc;
		};
		char_cdc_union notif;

		// /* We echo signals back to host as notification. */
		notif.cdc.bmRequestType = 0xA1;
		notif.cdc.bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
		notif.cdc.wValue = 0;
		notif.cdc.wIndex = 0;
		notif.cdc.wLength = 2;
		notif.buf[8] = req->wValue & 3;
		notif.buf[9] = 0;
		usbd_ep_write_packet(usbd_dev, 0x83, buf, 10);
		return USBD_REQ_HANDLED;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return USBD_REQ_NOTSUPP;
		return USBD_REQ_HANDLED;
	}
	usb_setup = true;
	return USBD_REQ_NOTSUPP;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;
	(void)usbd_dev;

	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);
	
	for(int i=0; i<len;i++){
		buf[i]++;
	}

	if (len) {
		usbd_ep_write_packet(usbd_dev, 0x82, buf, len);
	}
}

static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;
	(void)usbd_dev;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}

static void cdcacm_suspend(){
	usb_setup = false;
}

static void cdcacm_resume(){
	usb_setup = true;
}

void reenumerate(){
	// Reenumerate USB
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_clear(GPIOA, GPIO12);
	
	for (unsigned int i = 0; i < 0x20000; i++)
		__asm__("nop");

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12);
}

usbd_device* init(){

	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);
	usbd_register_suspend_callback(usbd_dev, cdcacm_suspend);
	usbd_register_resume_callback(usbd_dev, cdcacm_resume);
    
	if(7 > *(usb_strings[0])){
	}
    unsigned int i;
	for (i = 0; i < 0x800000; i++)
		__asm__("nop");

    return usbd_dev;
}

bool isConnected(){
    return usb_setup;
}

void poll(){
	usbd_poll(usbd_dev);
}

bool writeString(const char* string, uint16_t len){
    if(isConnected() || len<64){
		// Set string length to before null char or length
		unsigned int i;
		for(i=0; i<len; i++){
			if(string[i]==0)break;
		}
        usbd_ep_write_packet(usbd_dev, 0x82, string, i);
        return true;
    }
    return false;
}

}