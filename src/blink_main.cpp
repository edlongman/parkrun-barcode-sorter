#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <UsbSerial.h>

#define Board_LED_Pin                                GPIO13
#define Board_LED_GPIO_Port                          GPIOC
#define LED_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()

void tim2_isr(){
	if(timer_get_flag(TIM2, TIM_SR_CC1IF)){
		gpio_set(Board_LED_GPIO_Port, Board_LED_Pin);
		timer_clear_flag(TIM2, TIM_SR_CC1IF);
	}
	if(timer_get_flag(TIM2, TIM_SR_UIF)){
		gpio_clear(Board_LED_GPIO_Port, Board_LED_Pin);
		timer_clear_flag(TIM2, TIM_SR_UIF);
	}
}

int main(void)
{
	unsigned int i;

	/* Without this the timer interrupt routine will never be called. */
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
	NULL;
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_set(Board_LED_GPIO_Port, Board_LED_Pin);
	gpio_set_mode(Board_LED_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, Board_LED_Pin);

	rcc_periph_clock_enable(RCC_TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
				TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM2, 4000);

	timer_set_oc_value(TIM2, TIM_OC1, 2000);
	timer_set_period(TIM2, 6000);
	timer_enable_counter(TIM2);
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
	timer_enable_irq(TIM2, TIM_DIER_UIE);


	usbd_device* usbd_dev{UsbSerial::init()};

	i = 0;
	int j =  0;
	char count_str[10] = "Count:0";
	uint8_t status_buf[2] = {0};
	while (1){
		if(i==0x00){
			//gpio_clear(GPIOC, Board_LED_Pin);
		}
		UsbSerial::poll();
		if(i==20000 && UsbSerial::isConnected() == true){
			// gpio_set(GPIOC, Board_LED_Pin);
			UsbSerial::writeString(count_str, 7);

		}
		if(UsbSerial::isConnected() == true){
			timer_set_oc_value(TIM2, TIM_OC1, 2000);
			timer_set_period(TIM2, 6000);
		}
		else{
			timer_set_oc_value(TIM2, TIM_OC1, 14000);
			timer_set_period(TIM2, 16000);
		}
		if(i==80000){
			//gpio_set(GPIOC, Board_LED_Pin);
		}
		i++;
		if(i>1000000)i=0;
	}
}