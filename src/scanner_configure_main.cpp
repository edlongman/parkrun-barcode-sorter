#include <libopencm3/stm32/rcc.h>
#include <UsbSerial.h>
#include <Scanner.h>
#include <MotorController.h>
#include <string.h>

const uint8_t max_read = 8;
char token_read[max_read] = {0};

int main(){

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

    UsbSerial::reenumerate();
	usbd_device* usbd_dev{UsbSerial::init()};

    Scanner::init();
    Scanner::enable();
    //MotorController::init();
    //MotorController::enable();
    const char no_scan_text[] = "No scan seen.\r\n";
    while(1){
		UsbSerial::poll();
        Scanner::scanLine(token_read, max_read);
        Scanner::startScan();
        for(int i=0;i<1000;i++){
		    UsbSerial::poll();
            // Approx 250us delay
            while (i<16000){
                __asm__("nop");
                i++;
            }
            if(Scanner::isScanComplete()){
                break;
            }
        }
        Scanner::endScan();
        if(Scanner::isScanComplete()){
            char read_report[20] = "Read code: ";
            strncat(read_report, token_read, max_read);
            UsbSerial::writeString(read_report,20);
        }
        else{
            UsbSerial::writeString(no_scan_text,20);
        }
        int i=0;
        while (i<16000){
            UsbSerial::poll();
            for(int j=0;j<1000;j++){
                __asm__("nop");
            }
            i++;
        }
    }
}