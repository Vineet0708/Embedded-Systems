#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include <time.h>

#define SDA_PIN PC_9
#define SCL_PIN PA_8
#define EEPROM_ADDR 0xA0

LCD_DISCO_F429ZI LCD;

I2C i2c(SDA_PIN, SCL_PIN);

void WriteEEPROM(int address, unsigned int ep_address, char *data, int size);
void ReadEEPROM(int address, unsigned int ep_address, char *data, int size);

int main() {

  // Read and write a byte from/to EEPROM
  unsigned int eeprom_addr = 0;
  
  char data_read[10];
  ReadEEPROM(EEPROM_ADDR, eeprom_addr, data_read, 10);
  printf("Previous value in EEPROM: %s\n", data_read);

  char data_write[10] = "Hi!";
  WriteEEPROM(EEPROM_ADDR, eeprom_addr, data_write, 10);
  printf("Wrote value to EEPROM: %s\n", data_write);

  // set RTC to January 1, 2024, 00:00:00 (HH:MM:SS)
  tm t;
  t.tm_year = 124; // years since 1900
  t.tm_mon = 0;
  t.tm_mday = 0;
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 0;
  //set_time(mktime(&t));

  while (1) {
    // // Read RTC and display on LCD
    // time_t rawtime;
    // time(&rawtime);
    // char timebuf[20];
    // strftime(timebuf, 20, "%H:%M:%S", localtime(&rawtime));

    // LCD.Clear(LCD_COLOR_WHITE);
    // LCD.SetFont(&Font24);
    // LCD.SetBackColor(LCD_COLOR_ORANGE);  // highlight the text
    // LCD.SetTextColor(LCD_COLOR_BLACK);
    // LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE);

    // thread_sleep_for(1000);
  }
}

// This function has 63 bytes write limit
void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[size + 2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

  for (int i = 0; i < size; i++) {
    i2cBuffer[i + 2] = data[i];
  }

  int result = i2c.write(address, i2cBuffer, size + 2, false);
  thread_sleep_for(6);
}

void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

  // Reset eeprom pointer address
  int result = i2c.write(address, i2cBuffer, 2, false);
  thread_sleep_for(6);

  // Read eeprom
  i2c.read(address, data, size);
  thread_sleep_for(6);
}

