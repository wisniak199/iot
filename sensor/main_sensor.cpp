#include <cstdlib>
#include "mbed.h"
#include "nRF24L01P.h"
#include "protocol.h"

//***************************************************************************//
// !!! SETTINGS !!!

#define DATA_RATE NRF24L01P_DATARATE_250_KBPS
//#define DATA_RATE       NRF24L01P_DATARATE_1_MBPS
//#define DATA_RATE       NRF24L01P_DATARATE_2_MBPS

#define POWER NRF24L01P_TX_PWR_ZERO_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_6_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_12_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_18_DB

#define CHANNEL 7  // 0 - 30

#include "config.h"

#define TX_ADDRESS ((unsigned long long)0xABCDEF00)
#define ECHO_DELAY 0.0

#define TRANSFER_SIZE sizeof(SensorData)

//***************************************************************************//

Serial pc(USBTX, USBRX);  // tx, rx

// mosi, miso, sck, csn, ce, irq
nRF24L01P radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2);

DigitalOut myled1(LED1);
// InterruptIn button(USER_BUTTON);

void init_radio() {
    radio.powerDown();
    radio.powerUp();

    radio.setAirDataRate(DATA_RATE);
    radio.setRfOutputPower(POWER);
    radio.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * CHANNEL);

    radio.setCrcWidth(NRF24L01P_CRC_8_BIT);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P1);
    radio.enableAutoRetransmit(0x0F, 0x00);

    radio.setTxAddress(TX_ADDRESS, 4);
    radio.setRxAddress(TX_ADDRESS, 4, NRF24L01P_PIPE_P0);
    radio.setRxAddress(RX_ADDRESS, 4, NRF24L01P_PIPE_P1);

    // Display the (default) setup of the nRF24L01+ chip
    pc.printf("nRF24L01+ Frequency    : %d MHz\r\n", radio.getRfFrequency());
    pc.printf("nRF24L01+ Output power : %d dBm\r\n", radio.getRfOutputPower());
    pc.printf("nRF24L01+ Data Rate    : %d kbps\r\n", radio.getAirDataRate());
    pc.printf("nRF24L01+ TX Address   : 0x%010llX\r\n", radio.getTxAddress());
    pc.printf("nRF24L01+ RX0 Address   : 0x%010llX\r\n",
              radio.getRxAddress(NRF24L01P_PIPE_P0));
    pc.printf("nRF24L01+ RX1 Address   : 0x%010llX\r\n",
              radio.getRxAddress(NRF24L01P_PIPE_P1));

    radio.setTransferSize(TRANSFER_SIZE, NRF24L01P_PIPE_P0);
    radio.setTransferSize(TRANSFER_SIZE, NRF24L01P_PIPE_P1);
}

int main() {
    pc.baud(115200);
    init_radio();

    srand(SEED);

    for (int msg_counter = 0; true; msg_counter++) {
        SensorData data;
        data.sensor_type = SensorType::TEMPERATURE;
        data.value = msg_counter;

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

        wait(2);
    }
}
