#include "mbed.h"

DigitalOut led3(PG_13);
DigitalOut led4(PG_14);

Ticker tick;
Ticker ticker;
Timeout timeout;

InterruptIn userButton(BUTTON1);

int state = 2; // default state on startup

void mode0()
{
    led3=0;
    led4=!led4;
}

void mode1()
{   
    if(led3==led4)
    {
        led3= 0;
        led4= 1;
    }
    else 
    {
        led3=!led3;
        led4=!led4;
    }
}


int counter = 0;  // Declare the counter variable outside the function

void toggleLed3()
{
    led3 = !led3;
}

void toggleLed4()
{
    led4 = !led4;
}

void mode2()
{
    led3=0;
    led4=0;
    if (counter % 2 == 0)
    {
        toggleLed3();
        timeout.attach(&toggleLed3, 1000ms);  // Schedule to turn off led3 after 1 second
    }
    else
    {
        toggleLed4();
        timeout.attach(&toggleLed4, 1000ms);  // Schedule to turn off led4 after 1 second
    }
    counter += 1;
    
}


void buttonPressed()
{
    switch (state)
    {
    case 0:
        tick.attach(&mode0, 1s);
        break;

    case 1:
        tick.attach(&mode1, 1s);
        break;

    case 2:
        tick.attach(&mode2, 3s);
        break;

    default:
        break;
    }

    state = (state + 1) % 3;
}

int main()
{
    userButton.fall(&buttonPressed);
    while (true) 
    {   
    }
}