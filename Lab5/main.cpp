#include "mbed.h"

DigitalOut d_out(LED1);

int main(void)
{
    while (1) {
        // Change the voltage on the digital output pin by 0.01 * VCC
        // and print what the measured voltage should be (assuming VCC = 3.3v)
        for (float i = 0.0f; i < 1.0f; i += 0.01f) {
            a_out.write(i);
            printf("aout = %1.2f volts\n", a_out.read() * 3.3f);
            // turn on the led if the voltage is greater than 0.5f * VCC
            d_out = (a_out.read() > 0.5f) ? 1 : 0;
            ThisThread::sleep_for(100ms);
        }
    }
}

