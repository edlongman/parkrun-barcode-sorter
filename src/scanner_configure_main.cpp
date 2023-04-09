#include <libopencm3/stm32/rcc.h>
#include <UsbSerial.h>
#include <Scanner.h>
#include <MotorController.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

const unsigned int starting_scanner_baud = 9600;
const uint8_t max_read = 15;
char token_read[max_read] = {0};

void scanForToken(){
    const char no_scan_text[20] = "No scan seen.\r\n";
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

char* bufferAsHex(const uint8_t* input, const unsigned int length, char* output){
    const char lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    unsigned int i;
    unsigned int o_pos = 0;
    for(i = 0; i<length; i++){
        const char val = input[i];
        output[o_pos++] = lookup[val >> 4];
        output[o_pos++] = lookup[val & 0x0F];
    }
    return &output[o_pos];
}

void commandToScanner(){
    constexpr unsigned int cmd_fb_len = 20;
    char command_feedback[cmd_fb_len];
    uint16_t baud_address = 0x002A;
    constexpr uint8_t baud_reg_size = 2;
    const uint8_t send_data[baud_reg_size] = {00, 34}; // 57600 Baud
    uint8_t data_response[baud_reg_size] = {0};
    Scanner::sendCommandForResponse(Scanner::CommandType::READ, baud_address, baud_reg_size, send_data, data_response);
    constexpr unsigned int fb_str_l = 20;
    constexpr unsigned int prefix_l = 10;
    char feedback_str[fb_str_l] = "Reg value:";
    uint8_t data_limit = std::min((unsigned int)(fb_str_l-prefix_l)/2, (unsigned int) baud_reg_size);
    auto buffer_end = bufferAsHex(data_response, data_limit, &feedback_str[prefix_l]);
    buffer_end[0] = '\0';
    UsbSerial::writeString(feedback_str, fb_str_l);
}

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
    unsigned int loop_count = 0;
    while(1){
		UsbSerial::poll();
        loop_count++;
        switch(loop_count%10){
            case 9: 
                commandToScanner();
                break;
            default:
                scanForToken();
                break;
        }
    }
}