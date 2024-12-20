#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include "DebouncedInterrupt.h"

#define SDA_PIN PC_9
#define SCL_PIN PA_8
#define EEPROM_ADDR 0xA0

InterruptIn user_button(BUTTON1);
DebouncedInterrupt external_button3(PC_4);
DebouncedInterrupt external_button1(PA_5);
DebouncedInterrupt external_button2(PA_7);

DigitalOut led(PG_13);
DigitalOut led1(PG_14);

LCD_DISCO_F429ZI LCD;
I2C i2c(SDA_PIN, SCL_PIN);

Ticker ticker;
Ticker ticker2;

volatile int hours = 0;
volatile int minutes = 0;
volatile int seconds = 0;

void updateTime() {
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours++;
            if (hours >= 24) {
                hours = 0;
            }
        }
    }
}

void displayTime() {
    char timebuf[9]; 
    snprintf(timebuf, 9, "%02d:%02d:%02d", hours, minutes, seconds);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetBackColor(LCD_COLOR_WHITE);
    LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
    LCD.DisplayStringAt(0, 80, (uint8_t *)timebuf, CENTER_MODE);
}

void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size) {
    char i2cBuffer[size + 2];
    i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
    i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

    for (int i = 0; i < size; i++) {
        i2cBuffer[i + 2] = data[i];
    }

    int result = i2c.write(address, i2cBuffer, size + 2, false);
}

void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size) {
    char i2cBuffer[2];
    i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
    i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

    // Reset eeprom pointer address
    int result = i2c.write(address, i2cBuffer, 2, false);
    // Read eeprom
    i2c.read(address, data, size);
}

char currentTime[9];
char text[30];
int state=1;
int counter02=0;
int counter12=1;
int counter3;

void TimeAdjust(){
    ticker.detach();
    ticker2.detach();
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetBackColor(LCD_COLOR_WHITE);
    sprintf(text, "Hours:%d", hours);
    LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
    LCD.SetBackColor(LCD_COLOR_WHITE);
    sprintf(text, "Minutes:%d", minutes);
    LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
}

unsigned int eeprom_addr = 0;
char data_write[8];

void Displaystoredtime(){
    sprintf(data_write, "%02d:%02d:%02d", hours, minutes, seconds);
    WriteEEPROM(EEPROM_ADDR, eeprom_addr, data_write, 8);
    printf("Wrote value to EEPROM: %s\n", data_write);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetBackColor(LCD_COLOR_WHITE);
    LCD.DisplayStringAt(0, 40, (uint8_t *)"Stored Time", CENTER_MODE);
}

int timer=0;
void Button2();

void Button1() {
    if (counter12==1){
        if(state==1){
            state=0;
            Button2();
        }
        else{
            ticker.attach(&updateTime, 1.0);
            ticker2.attach(&displayTime,1.0);
            state=1;
        }
    }
}

void Button2() {
    if (state==0){
        ticker.detach();
        ticker2.detach();
        LCD.Clear(LCD_COLOR_WHITE);
        if (counter02==0){
            LCD.SetBackColor(LCD_COLOR_ORANGE);
            sprintf(text, "Hours:%d", hours);
            LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
            LCD.SetBackColor(LCD_COLOR_WHITE);
            sprintf(text, "Minutes:%d", minutes);
            LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
            counter02=1;
        }
        else{
            LCD.SetBackColor(LCD_COLOR_WHITE);
            sprintf(text, "Hours:%d", hours);
            LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
            LCD.SetBackColor(LCD_COLOR_ORANGE);
            sprintf(text, "Minutes:%d", minutes);
            LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
            counter02=0; 
        }
    }
    else{
        if(counter12==1){
            ticker.detach();
            ticker2.detach();
            Displaystoredtime();
            counter12=0;
        }
        else{
            counter12=1;
            ticker.attach(&updateTime, 1.0);
            ticker2.attach(&displayTime,1.0);
        }
    }
}

void Button3(){
    if(state==0){
        if (counter02==0){
            minutes++;
            led1=0;
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetBackColor(LCD_COLOR_WHITE);
            sprintf(text, "Hours:%d", hours);
            LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
            LCD.SetBackColor(LCD_COLOR_ORANGE);
            sprintf(text, "Minutes:%d", minutes);
            LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
        } 
        else{
            hours++;
            led1=1;
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.SetBackColor(LCD_COLOR_ORANGE);
            sprintf(text, "Hours:%d", hours);
            LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
            LCD.SetBackColor(LCD_COLOR_WHITE);
            sprintf(text, "Minutes:%d", minutes);
            LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
        }
    }
    led=0;
}


void UserButton(){
    if (state==0){
        ticker2.detach();
        WriteEEPROM(EEPROM_ADDR, eeprom_addr, data_write, 10);
        printf("Wrote value to EEPROM: %s\n", data_write);
        WriteEEPROM(EEPROM_ADDR, 0, currentTime, 8);
        ticker2.attach(&updateTime,1.0);
    }
}

int main() {
    LCD.Init();
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetFont(&Font24);
    LCD.SetTextColor(LCD_COLOR_BLACK);
    
    ticker.detach();
    ticker2.detach();
    
    ticker.attach(&updateTime, 1.0);
    ticker2.attach(&displayTime,1.0);

    user_button.rise(&UserButton);attach(&Button1, IRQ_FALL, 50, false);
    external_button1.
    external_button2.attach(&Button2, IRQ_FALL, 50, false);
    external_button3.attach(&Button3, IRQ_FALL, 50, false);

    while (1) {
    }
}
