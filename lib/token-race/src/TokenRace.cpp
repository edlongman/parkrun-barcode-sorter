#include "TokenRace.h"
#include "Token.h"
#include <WheelGauge.h>
#include <avr/io.h>

namespace TokenRace{

const uint8_t output_base = 3;

const uint8_t queue_size = 16;
Token queue[queue_size];
uint8_t queue_head = 0;
uint8_t queue_tail = 0;
uint8_t sort_index = 0;
void setSortIndex(uint8_t idx){
    sort_index = idx;
}

bool isQueueFull(){
    return (queue_head+1)%queue_size == queue_tail;
}

uint8_t getTokenCount(){
    if(queue_tail>queue_head){
        return queue_size - queue_tail + queue_head;
    }
    else{
        return queue_head - queue_tail;
    }
}

bool checkFlapOutput(uint8_t flap, int16_t wheel_position){
    int16_t open_threshold = 0;
    int16_t close_threshold = 0;
    switch(flap){
        case(1):
            open_threshold = 450;
            close_threshold = open_threshold + 100;
            break;
        default:
            return false;
    }

    for(uint8_t i=queue_tail;i!=queue_head;i++,i%=queue_size){
        if(queue[i].getOutput() == flap){
            int16_t travelledFront = queue[i].getOpenDistance(wheel_position);
            int16_t travelledBack = queue[i].getCloseDistance(wheel_position);
            if(travelledBack<close_threshold && travelledFront>open_threshold){
                return true;
            }
        }
    }
}

Token* insertToken(char* token_read){
    if(isQueueFull()){
        return nullptr;
    }
    queue[queue_head] = Token(WheelGauge::read(), token_read, output_base, sort_index);
    Token* inserted_address = &queue[queue_head];
    queue_head=(queue_head+1)%queue_size;
    return inserted_address;
}

int16_t expiryDistance = 1500;
void popExpiredTokens(int16_t wheel_position){
    for(uint8_t i=queue_tail;i!=queue_head;i++,i%=queue_size){
        int16_t travelled = queue[i].getOpenDistance(wheel_position);
        if(travelled<expiryDistance){
            return;
        }
        else{
            queue_tail=(queue_tail+1)%queue_size;
        }
    }
}

}

