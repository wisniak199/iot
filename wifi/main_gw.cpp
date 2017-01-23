#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include "mbed.h"
#include "nRF24L01P.h"
#include "protocol.h"
#include "RCSwitch.h"

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

#define RX_ADDRESS ((unsigned long long)0xABCDEF00)
#define TX_ADDRESS ((unsigned long long)0xABCDEF01)
#define ECHO_DELAY 0.0

#define TRANSFER_SIZE sizeof(SensorData)

//***************************************************************************//

Serial pc(USBTX, USBRX);  // tx, rx
Serial wifi(PA_9, PA_10);

// mosi, miso, sck, csn, ce, irq
nRF24L01P radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2);

DigitalOut myled1(LED1);
// InterruptIn button(USER_BUTTON);

RCSwitch sw(PC_3);

#define SW_ADDR "11111"
#define SW_C "00100"

const char RC_ON = '1';

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

    radio.setReceiveMode();
    radio.enable();
}

const std::string http_header =
    std::string("POST /write?db=test HTTP/1.1\r\n") +
    "User-Agent: curl/7.35.0\r\n" + "Host: localhost:9090\r\n" +
    "Content-Type: application/x-www-form-urlencoded\r\n" +
    "Accept: */*\r\n";


char char_from_PC = '\0';
bool rc_enabled = false;

void rc(bool on) {
    // pc.printf("RC(%d), state = %d\r\n", (int) on, rc_enabled);
    /* if (rc_enabled == on) {
        return;
    }*/
    if (on) {
        // pc.printf("SWITCHING ONNNNNNNN\r\n");
        sw.switchOn(SW_ADDR, SW_C);
    } else {
        // pc.printf("SWITCHING OFF\r\n");
        sw.switchOff(SW_ADDR, SW_C);
    }
    rc_enabled = on;
}

void wifi_callback() {
    char_from_PC = wifi.getc();
    char newline = wifi.getc();
}

int main() {
    pc.baud(115200);
    init_radio();

    wifi.baud(115200);
    wifi.attach(&wifi_callback);

    SensorDataStream stream;

    while (true) {
        if (char_from_PC != '\0') {
            pc.printf("GOT CHAR '%c' '%c' '%d'\r\n", char_from_PC, RC_ON, char_from_PC == RC_ON);
            rc(char_from_PC == RC_ON);
            char_from_PC = '\0';
        }
        pc.printf("TRYING NEXT FROM SENSOR\r\n");
        wait_ms(100);
        if (stream.next(&radio, NRF24L01P_PIPE_P1) == -1) {
            // pc.printf("error in next \r\n");
            continue;
        }
        SensorData data = stream.get();

        std::string http_data =
          "iot,type=" + sensor_type_to_string(data.sensor_type) +
          " value=" + int_to_string(data.value) + "\n";
        std::string http =
          http_header +
          "Content-Length: " +  int_to_string(http_data.size()) +
          "\r\n\r\n" + http_data + "\r\n\r\n";


        pc.printf("RECEIVED FROM SENSOR = %d %d\r\n", data.sensor_type,
                  data.value);
        pc.printf("RECEIVED FROM SENSOR = %f %f\r\n", (float) data.sensor_type,
                            (float) data.value);
        pc.printf("Sending to HTTP SERVER %s\r\n", http.c_str());
        wifi.printf("%s", http.c_str());
        wait_ms(100);
    }
}
