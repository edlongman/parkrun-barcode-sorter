#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "FlapGovenor.h"

int main(void){
  FlapGovenor::init();
  FlapGovenor::enable();
  sei();
  while(1){}
}