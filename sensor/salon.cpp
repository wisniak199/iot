#include <cstdlib>
#include "mbed.h"
#include "nRF24L01P.h"

#include "sensor.h"

#define HALL

#ifdef LIVING_ROOM
Sensor sensor[] = 
{ 
    Sensor(analog[5], Temperature),
    Sensor(analog[4], Grayscale)
     
};
#define RX_ADDRESS ((unsigned long long)0xABCDEF0F) // RÓŻNY dla KAŻDEJ płytki z sensorem
#define SEED 1000 // ziarno dla sranda, również RÓŻNE dla KAŻDEJ płytki
#endif //LIVING_ROOM

#ifdef KITCHEN
Sensor sensor[] = 
{
    Sensor(analog[0], UV),
    Sensor(analog[1], Flame),
    Sensor(analog[2], Moisture)
};
#define RX_ADDRESS ((unsigned long long)0xABCDEF0E) // RÓŻNY dla KAŻDEJ płytki z sensorem
#define SEED 1017 // ziarno dla sranda, również RÓŻNE dla KAŻDEJ płytki
#endif //KITCHEN

#ifdef HALL
Sensor sensor[] =
{
    Sensor(analog[0], Movement)
};
#define RX_ADDRESS ((unsigned long long)0xABCDEF0D) // RÓŻNY dla KAŻDEJ płytki z sensorem
#define SEED 144 // ziarno dla sranda, również RÓŻNE dla KAŻDEJ płytki
bool isArmed = false;
int max_value = 0;
DigitalOut armedLed(LED1);
InterruptIn button(analog[5]);
void buttonCallback()
{
    isArmed = !isArmed;
    armedLed = !armedLed;
    max_value = 0;
}
#endif //HALL

const int SENSOR_COUNT = sizeof(sensor) / sizeof(Sensor);


// mosi, miso, sck, csn, ce, irq
nRF24L01P radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2);
Serial pc(USBTX, USBRX);  // tx, rx
DigitalOut myled1(LED1);

#include "radio.h"


int main() 
{
    pc.baud(115200);
    init_radio();

    srand(SEED);
    
    pc.printf("Hello world, I have %d sensors\r\n", SENSOR_COUNT);
    
    #ifdef HALL
    button.fall(&buttonCallback);
    #endif //HALL

    for (int msg_counter = 0; true; msg_counter++) 
    {
        SensorData data;
        data.sensor_type = sensor[msg_counter % SENSOR_COUNT].type;
        data.value = sensor[msg_counter % SENSOR_COUNT].getScaledInput();
        
        #ifdef HALL
        max_value = max_value > data.value * isArmed ? max_value : data.value * isArmed;
        data.value = max_value;
        #endif //HALL
        
        while (true) {
            pc.printf("Trying to send via radio\r\n");
            int res = radio.write(NRF24L01P_PIPE_P0, (char *)&data,
                                  sizeof(SensorData));
            if (res > 0) {
                pc.printf("SEND OK res: %d\r\n", res);
                break;
            } else {
                pc.printf("SEND ERROR\r\n");
                int sleep = 2000 + rand() % 1000;
                wait_ms(sleep);
                pc.printf("WAIT_END\r\n");
            }
        }
        
        pc.printf("Board sent data from %s == %d (%f)\r\n", sensor_type_to_string(data.sensor_type).c_str(), data.value, sensor[msg_counter % SENSOR_COUNT].getRawInput());
        
        wait(0.33);
    }
}
