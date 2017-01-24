#include "mbed.h"
#include <string>

const PinName analog[] = {PA_0, PA_1, PA_4, PB_0, PC_1, PC_0};

// Sposrod zestawu ktory dostalismy nieskalibrowane sa: 
// liquid level (nie widze zastosowania),
// mq-7 gas sensor (wymaga nieco więcej czasu i trochę gazu)

enum SensorType {Temperature, Grayscale, UV, Moisture, Push, Flame, Movement, Unknown};

const int LIGHT_VERY_DARK = 0; // w plecaku
const int LIGHT_DARK = 1; // pod stołem
const int LIGHT_MEDIUM = 2; // typowa jasność w labach w pochmurny dzień
const int LIGHT_BRIGHT = 3; // sensor skierowany w stronę okna lub zapalone lampy
const int LIGHT_VERY_BRIGHT = 4; // sensor na zewnątrz

const int PUSH_OFF = 0;
const int PUSH_ON = 1;

const int MOVEMENT_NOT_DETECTED = 0;
const int MOVEMENT_DETECTED = 1;

const int FLAME_DETECTED = 1;
const int FLAME_NOT_DETECTED = 0;

const int SOIL_DRY = 0; // ziemia to wióry, nie ma czego zbierać
const int SOIL_OPTIMAL =  1; // jest ok, chyba ze to kaktus :)
const int SOIL_TOO_HUMID = 2; // błoto / woda

std::string sensor_type_to_string(SensorType type) {
    switch (type) {
        case Temperature:
            return "temperature";
        case Grayscale:
            return "grayscale";
        case UV:
            return "ultraviolet";
        case Moisture:
            return "moisture";
        case Push:
            return "push";
        case Flame:
            return "flame";
        case Movement:
            return "movement";    
        default:
            return "unknown";
    }
}
struct Sensor
{
    AnalogIn input;
    SensorType type;
    const char* name;
    
    float getRawInput()
    {
        return input.read();
    }
   
    int getScaledInput()
    {
        float val = input.read();
        switch(type)
        {
            case Temperature:
                return static_cast<int> (100 * val * 330); // stopnie celsjusza
            case Grayscale:                
                if(val < 0.1f) return LIGHT_VERY_BRIGHT;
                if(0.1f <= val && val < 0.25f) return LIGHT_BRIGHT;
                if(0.25f <= val && val < 0.4f) return LIGHT_MEDIUM;
                if(0.4f <= val && val < 0.55f) return LIGHT_DARK;
                if(0.55f <= val) return LIGHT_VERY_DARK;
            case Push:
                if(val < 0.5f) return PUSH_OFF;
                else return PUSH_ON;
            case Movement:
                if(val < 0.5f) return MOVEMENT_NOT_DETECTED;
                else return MOVEMENT_DETECTED;
            case Flame:
                if(val < 0.7f) return FLAME_NOT_DETECTED;
                else return FLAME_DETECTED;
            case Moisture:
                val *= 1024;
                if(val < 300) return SOIL_DRY;
                if(300 <= val && val < 700) return SOIL_OPTIMAL;
                if(700 < val) return SOIL_TOO_HUMID;
            case UV:
                val *= 1024;
                return static_cast<int> (100 * val / 20); //zwraca krotność normalnego promieniowania
            default:
                return static_cast<int> (100 * val);
        }
    }
    Sensor(PinName pin, SensorType sensorType) : input(pin), type(sensorType), name(sensor_type_to_string(sensorType).c_str()) {}
    Sensor(PinName pin, SensorType sensorType, const char* sensorName) : input(pin), type(sensorType), name(sensorName) {}
};
#include <sstream>
#include <string>
#include "nRF24L01P.h"

#define BUFFER_SIZE 256
#define SLEEP_TIME 1

struct SensorData {
    SensorType sensor_type;
    int value;
};

/*
struct SensorDataStream {
    char buffer[BUFFER_SIZE];
    unsigned begin, end;
    SensorData now;
    SensorDataStream() : begin(0), end(0) {}
    SensorData get() { return now; }
    int read_data(nRF24L01P *radio, int pipe);
    int next(nRF24L01P *radio, int pipe);
};

int SensorDataStream::read_data(nRF24L01P *radio, int pipe) {
    if (begin != 0) {
        for (unsigned i = begin; i < end; ++i) {
            buffer[i - begin] = buffer[i];
        }
        end -= begin;
        begin = 0;
    }
    int res = 0;
    if (radio->readable(pipe)) {
        res = radio->read(pipe, buffer + end, sizeof(SensorData));
    }
    if (res < 0) return -1;
    end += res;
    return res;
}

int SensorDataStream::next(nRF24L01P *radio, int pipe) {
    if (!radio->readable(pipe)) {
        return -1;
    } 
    while (end - begin < sizeof(SensorData)) {
        int res = read_data(radio, pipe);
        if (res < 0) return -1;
    }

    SensorData *temp = (SensorData *)(buffer + begin);
    now.sensor_type = temp->sensor_type;
    now.value = temp->value;
    begin += sizeof(SensorData);
    return 0;
}
*/

