// rysuje wykres wejścia w konsoli
void draw_plot(Sensor& sensor, int len)
{
    float val = sensor.getRawInput();
    
    pc.printf("|");
    int i = 1;
    for(;i < (int) (val * len); i++) pc.printf(" ");
    pc.printf(".");
    i++;
    for(; i < len-1; i++) pc.printf(" ");
    pc.printf("|");
    pc.printf("%f, %f\r\n", sensor.getRawInput(), sensor.getScaledInput());
}        
 
 Serial pc(SERIAL_TX, SERIAL_RX); // tx, rx
int main() 
{
    pc.baud(115200);
    pc.printf("Hello world!\r\n");
    const int len = 120;
    while(true)
    {
        draw_plot(sensor[0], len);
        wait(0.1);
        }
}