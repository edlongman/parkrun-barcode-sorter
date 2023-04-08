#include <libopencm3/stm32/rcc.h>
#include <UsbSerial.h>
#include <Scanner.h>
#include <MotorController.h>
#include <string.h>

const unsigned int starting_scanner_baud = 9600;
const uint8_t max_read = 15;
char token_read[max_read] = {0};

int main(){

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

    UsbSerial::reenumerate();
	usbd_device* usbd_dev{UsbSerial::init()};

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set(GPIOC, GPIO13);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    Scanner::init(starting_scanner_baud);
    Scanner::enable();
    //MotorController::init();
    //MotorController::enable();
    const char no_scan_text[20] = "No scan seen.\r\n";
    while(1){
		UsbSerial::poll();
        Scanner::scanLine(token_read, max_read);
        Scanner::startScan();
        for(int i=0;i<20000;i++){
		    UsbSerial::poll();
            // Approx 25us delay
            for(volatile int j=0;j<160;j++){
                __asm__("nop");
            }
            if(Scanner::isScanComplete()){
	            gpio_clear(GPIOC, GPIO13);
                break;
            }
        }
        if(Scanner::isScanComplete()){
            char read_report[20] = "Read code: ";
            strncat(read_report, token_read, 20);
            UsbSerial::writeString(read_report,16);
        }
        else{
            UsbSerial::writeString(no_scan_text,15);
        }
        Scanner::endScan();
        int i=0;
        while (i<10000){
            UsbSerial::poll();
            for(volatile int j=0;j<160;j++){
                __asm__("nop");
            }
            i++;
        }
        gpio_set(GPIOC, GPIO13);
    }
}