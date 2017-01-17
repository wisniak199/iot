#include "mbed.h"
#include "RCSwitch.h"

RCSwitch sw(PC_3);
DigitalOut led(LED1);

#define SW_ADDR "11111"
#define SW_D "00010"
#define SW_C "00100"

int main() {
    while(1){
        wait(1);
        led = 1;
        sw.switchOn(SW_ADDR, SW_C);
        
        wait(1);
        led = 0;
        sw.switchOff(SW_ADDR, SW_C);
    }
}
