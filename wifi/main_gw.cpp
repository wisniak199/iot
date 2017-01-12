#include "mbed.h"
#include "nRF24L01P.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include "protocol.h"

//***************************************************************************//
// !!! SETTINGS !!!

#define DATA_RATE       NRF24L01P_DATARATE_250_KBPS
//#define DATA_RATE       NRF24L01P_DATARATE_1_MBPS
//#define DATA_RATE       NRF24L01P_DATARATE_2_MBPS

#define POWER           NRF24L01P_TX_PWR_ZERO_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_6_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_12_DB
//#define POWER           NRF24L01P_TX_PWR_MINUS_18_DB

#define CHANNEL         7 // 0 - 30

#define RX_ADDRESS      ((unsigned long long) 0xABCDEF00)
#define TX_ADDRESS      ((unsigned long long) 0xABCDEF01)
#define ECHO_DELAY      0.0

#define TRANSFER_SIZE   sizeof(SensorData)

//***************************************************************************//

Serial pc(USBTX, USBRX); // tx, rx
Serial wifi(PA_9, PA_10);
nRF24L01P radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2);    // mosi, miso, sck, csn, ce, irq

DigitalOut myled1(LED1);
// InterruptIn button(USER_BUTTON);

std::string int_to_string(int x) {
    std::stringstream s;
    s << x;
    std::string result;
    s >> result;
    return result;
}

void init_radio() {
    radio.powerDown();
    radio.powerUp();

    radio.setAirDataRate(DATA_RATE);
    radio.setRfOutputPower(POWER);
    radio.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * CHANNEL);

    radio.setCrcWidth(NRF24L01P_CRC_8_BIT);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
    radio.enableAutoAcknowledge(NRF24L01P_PIPE_P1);
    radio.enableAutoRetransmit(0x0F, 0x0F);

    radio.setTxAddress(TX_ADDRESS, 4);
    radio.setRxAddress(TX_ADDRESS, 4, NRF24L01P_PIPE_P0);
    radio.setRxAddress(RX_ADDRESS, 4, NRF24L01P_PIPE_P1);

    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  radio.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  radio.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", radio.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", radio.getTxAddress() );
    pc.printf( "nRF24L01+ RX0 Address   : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P0) );
    pc.printf( "nRF24L01+ RX1 Address   : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P1) );

    radio.setTransferSize(TRANSFER_SIZE, NRF24L01P_PIPE_P0);
    radio.setTransferSize(TRANSFER_SIZE, NRF24L01P_PIPE_P1);

    radio.setReceiveMode();
    radio.enable();
}

const std::string http_header =
        std::string("POST /write?db=test HTTP/1.1\r\n") +
        "User-Agent: curl/7.35.0\r\n" +
        "Host: localhost:9090\r\n" +
        "Accept: */*\r\n" +
        "Content-Length: 78\r\n" +
        "Content-Type: application/x-www-form-urlencoded\r\n" +
        "\r\n";

int main() {
    pc.baud(115200);
    init_radio();
    wifi.baud(115200);

    SensorDataStream stream;
    while (1) {
        pc.printf("TRYING NEXT FROM SENSOR\r\n");
        if (stream.next(&radio, NRF24L01P_PIPE_P1) == -1) {
            pc.printf("error in next \r\n");
            continue;
        }
        SensorData data = stream.get();
        
        std::string http = http_header +
            "iot,type=" + sensor_type_to_string(data.sensor_type) +
            " value=" + int_to_string(data.value) + "\r\n";
        
        pc.printf("RECEIVED FROM SENSOR = %d %d\r\n", data.sensor_type, data.value);
        pc.printf("Sending to HTTP SERVER %s\r\n", http.c_str());
        wifi.write((const uint8_t *)http.c_str(), http.size(), 0, 0);
        wait_ms(500);
    }
}
