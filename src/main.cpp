#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "FlapGovenor.h"
#include "MotorController.h"

int main(void){
  FlapGovenor::init();
  FlapGovenor::enable();
  MotorController::init();
  MotorController::enable();
  sei();
  while(1){}
}