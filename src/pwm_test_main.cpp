#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <MotorController.h>
#include <UsbSerial.h>
#include <WheelGauge.h>


int main(void)
{
	unsigned int i;
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	
    UsbSerial::reenumerate();
	usbd_device* usbd_dev{UsbSerial::init()};
    MotorController::init();
	char msg_str[] = "PWM is 0000\n";

	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set(GPIOB, GPIO13);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	i = 0;
	uint8_t j =  0;
	uint8_t status_buf[2] = {0};
	MotorController::enable();
	MotorController::setSpeed(50);
	WheelGauge::init();
	WheelGauge::enable();
	while (1){
		UsbSerial::poll();
		if((i%80000 == 0) && UsbSerial::isConnected() == true){
			uint8_t value = WheelGauge::read();
            msg_str[7] = (value/100)%10 + 48;
            msg_str[8] = (value/100)%10 + 48;
            msg_str[9] = (value/10)%10 + 48;
            msg_str[10] = (value)%10 + 48;
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