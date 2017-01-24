#include "nRF24L01P.h"

#define DATA_RATE NRF24L01P_DATARATE_250_KBPS
//#define DATA_RATE       NRF24L01P_DATARATE_1_MBPS
//#define DATA_RATE       NRF24L01P_DATARATE_2_MBPS

#define POWER NRF24L01P_TX_PWR_ZERO_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_6_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_12_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_18_DB

#define CHANNEL 7  // 0 - 30

#define TX_ADDRESS ((unsigned long long)0xABCDEF00)
#define ECHO_DELAY 0.0

#define TRANSFER_SIZE sizeof(SensorData)

void init_radio() 
{
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