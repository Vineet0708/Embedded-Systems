#include "mbed.h"
#include "LCD_DISCO_F429ZI.h"
#include "DebouncedInterrupt.h"

LCD_DISCO_F429ZI LCD;

AnalogIn dist(PA_6);

PwmOut ledblue(PD_14);
DigitalOut buzz(PD_15);
DigitalOut ledred(PE_8);

DebouncedInterrupt button1(PC_4);
DebouncedInterrupt button2(PA_7); 
DebouncedInterrupt button3(PB_2);
DebouncedInterrupt button4(PC_8);
InterruptIn userbutton(BUTTON1);

Ticker tickerblue;
Timeout tickerbuzz;
Ticker ticker;
Ticker ticker1;

Timeout redtime;
Timeout bluetime;
Timeout timeout;

// Password sequence
const int passwordLength = 4;
const int correctPassword[passwordLength] = {4, 3, 2, 1}; // Example password sequence

// Main password variables
int enteredPassword[passwordLength] = {0}; // Current entered password
int passwordIndex = 0; // Index of the entered password
int passwordenteredlength = 0;
bool passwordEntered = false; // Flag to indicate if the correct password is entered
int wrongpasswords = 0;
bool lockstate = false;
char text[20];
bool locked = false;
bool permalock = false;

int ledbluespeed = 0;
int buzzspeed = 0;

float distan;
float currentdistance;
float previousdistance = 0; 
bool motiondetected = false;

float convertdistance(float distance){
    return 25 * pow(distance, -1.7); //calibrated using data gathered with real values 
}

void updatedist(){
    distan = dist.read();
    currentdistance = convertdistance(distan);
}

void resetMotionDetected() {
    motiondetected = false;
}

void alarm(){

    if (fabs(currentdistance - previousdistance) > 15) {
        motiondetected = true; 
        timeout.attach(&resetMotionDetected, 1.0);
    }
    previousdistance = currentdistance;
}

void buttonbuzz(){
    ledred = 0;
}

void Button1ISR(){
    if (!locked){
        enteredPassword[passwordIndex] = 1;
        passwordIndex++;
        LCD.Clear(LCD_COLOR_WHITE);
        sprintf(text, "entered:%d,%d,%d,%d", enteredPassword[0], enteredPassword[1], enteredPassword[2], enteredPassword[3]);
        LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
        ledred = 1;
        tickerbuzz.attach(&buttonbuzz, 0.1);
    }
}

void Button2ISR(){
    if (!locked){
        enteredPassword[passwordIndex] = 2;
        passwordIndex++;
        LCD.Clear(LCD_COLOR_WHITE);
        sprintf(text, "entered:%d,%d,%d,%d", enteredPassword[0], enteredPassword[1], enteredPassword[2], enteredPassword[3]);
        LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
        ledred = 1;
        tickerbuzz.attach(&buttonbuzz, 0.1);
    }
}

void Button3ISR(){
    if (!locked){
        enteredPassword[passwordIndex] = 3;
        passwordIndex++;
        LCD.Clear(LCD_COLOR_WHITE);
        sprintf(text, "entered:%d,%d,%d,%d", enteredPassword[0], enteredPassword[1], enteredPassword[2], enteredPassword[3]);
        LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
        ledred = 1;
        tickerbuzz.attach(&buttonbuzz, 0.1);
    }
}

void Button4ISR(){
    if (!locked){
        enteredPassword[passwordIndex] = 4;
        passwordIndex++;
        LCD.Clear(LCD_COLOR_WHITE);
        sprintf(text, "entered:%d,%d,%d,%d", enteredPassword[0], enteredPassword[1], enteredPassword[2], enteredPassword[3]);
        LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
        ledred = 1;
        tickerbuzz.attach(&buttonbuzz, 0.1);
    }
}

void checker();

void userbuttonISR(){
    if (locked && !permalock){
    LCD.Clear(LCD_COLOR_WHITE);
    wrongpasswords = 2;
    passwordIndex = 0;
    enteredPassword[0] = 0;
    enteredPassword[1] = 0;
    enteredPassword[2] = 0;
    enteredPassword[3] = 0;
    ticker.detach();
    ledred =0;
    ledblue=0;
    ticker1.attach(&checker, 0.5);

    sprintf(text, "Police Override");
    LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
    
    sprintf(text, "Enter Password:");
    LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);

    sprintf(text, "%d,%d,%d,%d", enteredPassword[0], enteredPassword[1], enteredPassword[2], enteredPassword[3]);
    LCD.DisplayStringAt(0, 120, (uint8_t *)text, CENTER_MODE);  
    locked = false; 
    lockstate = true;
    permalock = true;
    }
}

void bluetick(){
    if (ledbluespeed < 256){
        ledblue.pulsewidth_us(ledbluespeed);
        ledbluespeed +=1;
    }
    else{
        tickerblue.detach();
    }
}

void lightflash(){
    ledred = !ledred;
    ledblue = !ledblue;
    buzz = !buzz;
}

void wrongpassword(){
    ledred = 0;
    passwordEntered = false;
    wrongpasswords++;
    LCD.Clear(LCD_COLOR_WHITE);
    sprintf(text, "Wrong Password");
    LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
    //print wrong password
    if (!lockstate){
        //show home screen
        sprintf(text, "Welcome Home");
        LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
    }
    else{
        //show locked screen 
        sprintf(text, "Armed Mode");
        LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
    }
}

void rightpassword(){
    ledred =0;
    passwordEntered = true;
    LCD.Clear(LCD_COLOR_WHITE);
    sprintf(text, "Right Password");
    LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE);
    if (lockstate){
        //show home screen
        sprintf(text, "Welcome Home");
        LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
        lockstate = false;
    }
    else{
        //show locked screen 
        sprintf(text, "Armed Mode");
        LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE);
        lockstate = true;
    }
    wrongpasswords = 0;
}

void checkPassword(){
    if(enteredPassword[0] == correctPassword[0] && enteredPassword[1] == correctPassword[1] && enteredPassword[2] == correctPassword[2] &&enteredPassword[3] == correctPassword[3] ){
        rightpassword();
    }
    else{
        wrongpassword();
    }
    passwordIndex = 0;
    enteredPassword[0] = 0;
    enteredPassword[1] = 0;
    enteredPassword[2] = 0;
    enteredPassword[3] = 0;
}


void permanentlock(){
    LCD.Clear(LCD_COLOR_WHITE);
    sprintf(text, "Calling Cops");
    LCD.DisplayStringAt(0, 40, (uint8_t *)text, CENTER_MODE); 
    sprintf(text, "Intruder Alert");
    LCD.DisplayStringAt(0, 80, (uint8_t *)text, CENTER_MODE); 
    locked = true;
}

void checker(){
    if (lockstate){
        tickerblue.detach();
        ledbluespeed = 0;
        ledblue=0;
    }
    else{
        if (ledbluespeed < 256){
        tickerblue.attach(&bluetick, 0.01);
        }
    }
    if ((wrongpasswords > 2) || (lockstate && motiondetected)){
        ticker1.detach();
        ledred =0;
        ledblue =1;
        ticker.attach(&lightflash, 0.3);
        permanentlock();
    }
    if (passwordIndex>3){
        checkPassword();
    }
}

Ticker ticker2;

int main()
{
    ledblue.period_us(256);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);
    LCD.SetBackColor(LCD_COLOR_WHITE);
            
    button1.attach(&Button1ISR, IRQ_FALL, 50, false);
    button2.attach(&Button2ISR, IRQ_FALL, 50, false);
    button3.attach(&Button3ISR, IRQ_FALL, 50, false);
    button4.attach(&Button4ISR, IRQ_FALL, 50, false);
    userbutton.fall(&userbuttonISR);
    ledred=0;
    buzz = 0;

    ticker1.attach(&checker, 0.5);
    ticker2.attach(&alarm,0.5);
    while (true) {
        updatedist();
    }
}
