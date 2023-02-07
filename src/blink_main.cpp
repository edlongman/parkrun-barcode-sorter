#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <UsbSerial.h>

#define Board_LED_Pin                                GPIO13
#define Board_LED_GPIO_Port                          GPIOC
#define LED_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()

int main(void)
{
	unsigned int i;

	NULL;
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_set(Board_LED_GPIO_Port, Board_LED_Pin);
	gpio_set_mode(Board_LED_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, Board_LED_Pin);



	usbd_device* usbd_dev{UsbSerial::init()};

	i = 0;
	int j =  0;
	char count_str[10] = "Count:0";
	uint8_t status_buf[2] = {0};
	while (1){
		if(i==0x00){
			gpio_clear(GPIOC, Board_LED_Pin);
		}
		UsbSerial::poll();
		if(i==20000 && UsbSerial::isConnected() == true){
			gpio_set(GPIOC, Board_LED_Pin);
			UsbSerial::writeString(count_str, 7);
		}
		if(i==80000){
			gpio_set(GPIOC, Board_LED_Pin);
		}
		i++;
		if(i>1000000)i=0;
	}
}