#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <MotorController.h>
#include <UsbSerial.h>


int main(void)
{
	unsigned int i;

	/* Without this the timer interrupt routine will never be called. */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
    UsbSerial::reenumerate();
	usbd_device* usbd_dev{UsbSerial::init()};
    MotorController::init();
	char msg_str[] = "PWM is 0000\n";

	i = 0;
	uint8_t j =  0;
	uint8_t status_buf[2] = {0};
	MotorController::enable();
	while (1){
		UsbSerial::poll();
		if((i%80000 == 0) && UsbSerial::isConnected() == true){
            msg_str[7] = (MotorController::getTimer()/100)%10 + 48;
            msg_str[8] = (MotorController::getTimer()/100)%10 + 48;
            msg_str[9] = (MotorController::getTimer()/10)%10 + 48;
            msg_str[10] = (MotorController::getTimer())%10 + 48;
			UsbSerial::writeString(msg_str, 12);
            j++;
            
			//MotorController::setPWM(j);

		}
		i++;
		if(i==500000){
			//MotorController::setPWM(205);
		}
		if(i>1000000){
            //MotorController::setPWM(52);
            i=0;
        }
	}
}