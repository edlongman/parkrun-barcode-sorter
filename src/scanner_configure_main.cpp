#include <Scanner.h>

int main(){
    Scanner::init();

    while(1){
        Scanner::startScan();
        _delay_ms(1000);
        Scanner::endScan();
        _delay_ms(500);
    }
}