/*
Calculation for the angular resolution of the motor:
Angular resolution = 360 degrees / Number of steps per revolution
                   = 360 degrees / 48 steps per revolution
                   ≈ 7.5 degrees per step

Student 1:
Name: Marcus DeMaria
Number: 400455619
Time period between two steps for half-stepping: ~0.542 seconds
Time period between two steps for full-stepping: ~1.083 seconds

Student 2:
Name: Vineet Aggarwal
Number: 400432692
Time period between two steps for half-stepping: ~0.615 seconds
Time period between two steps for full-stepping: ~1.229 seconds
*/
#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include <math.h>
#include "DebouncedInterrupt.h"
#include <string>

LCD_DISCO_F429ZI LCD;

DigitalOut In1(PB_7); //PD_8
DigitalOut In2(PC_3); //PD_10
DigitalOut In3(PF_6); //PD_14
DigitalOut In4(PC_8);

InterruptIn User_Button(BUTTON1);
DebouncedInterrupt StepperMode(PA_7);
DebouncedInterrupt SetDirection(PA_5);
DebouncedInterrupt SpeedUp(PE_4);
DebouncedInterrupt SpeedDown(PE_3);

Timeout timeout;
Ticker ticker;

int speed = 0;
// Define motor parameters
const float ANGULAR_RESOLUTION = 360.0 / 48; // Degrees per step
string s1Name = "Marcus";
int s1Num = 400455619;
float s1full = 1083; // Initialize time period for student 1
float s1half = 542;
string s2Name = "Vineet";
int s2Num = 400432692;
float s2full = 1229; // Initialize time period for student 2
float s2half = 615;

bool isStudent1 = false; // Flags to keep track of what is being displayed and ran
bool StepMode = true; // To abide by initial condition
bool direction = true; // To abide by initial condition
int initial = 1; // Initial condition

void step1f();
void step2f();
void step3f();
void step4f();

void step1h();
void step2h();
void step3h();
void step4h();
void step5h();
void step6h();
void step7h();
void step8h();

void displayStudentInfo() {
    // LCD Initial Display
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);
    LCD.DisplayStringAt(0, 35, (uint8_t *)"Name:", CENTER_MODE);
    //timeout.detach();
    ticker.detach();
    if (initial = 1){ // For initial press of user
        LCD.DisplayStringAt(0, 125, (uint8_t *)"Step Mode: Full Step", CENTER_MODE);
        LCD.DisplayStringAt(0, 140, (uint8_t *)"Direction: C-W", CENTER_MODE);
        initial = 0;
    }
    else {} // Will go back to 0 permantly after initial press and not execute again
    if (isStudent1) {
        LCD.DisplayStringAt(0, 50, (uint8_t *)s1Name.c_str(), CENTER_MODE);
        LCD.DisplayStringAt(0, 65, (uint8_t *)"Num:", CENTER_MODE);
        char s1NumBuff[20];
        sprintf(s1NumBuff, "%d", s1Num);
        LCD.DisplayStringAt(0, 80, (uint8_t *)s1NumBuff, CENTER_MODE);
        int s1full_int = (int)(s1full * 10);
        char s1full_buff[20];
        snprintf(s1full_buff, 95, "Full Step (ms): %d.%d", s1full_int / 10, s1full_int % 10); 
        LCD.DisplayStringAt(0, 95, (uint8_t *)s1full_buff, CENTER_MODE);
        int s1half_int = (int)(s1half * 10);
        char s1half_buff[20];
        snprintf(s1half_buff, 110, "Half Step (ms): %d.%d", s1half_int / 10, s1half_int % 10); 
        LCD.DisplayStringAt(0, 110, (uint8_t *)s1half_buff, CENTER_MODE);
        ticker.detach();
        if (StepMode){
            speed = s1full;
            ticker.attach(&step1f, speed*0.004);
        }
        if(!StepMode){
            speed = s1half;
            ticker.attach(&step1h, speed*0.008);
        }
    } else {
        LCD.DisplayStringAt(0, 50, (uint8_t *)s2Name.c_str(), CENTER_MODE);
        LCD.DisplayStringAt(0, 65, (uint8_t *)"Num:", CENTER_MODE);
        char s2NumBuff[20];
        sprintf(s2NumBuff, "%d", s2Num);
        LCD.DisplayStringAt(0, 80, (uint8_t *)s2NumBuff, CENTER_MODE);
        int s2full_int = (int)(s2full * 10);
        char s2full_buff[20];
        snprintf(s2full_buff, 95, "Full Step (ms): %d.%d", s2full_int / 10, s2full_int % 10); 
        LCD.DisplayStringAt(0, 95, (uint8_t *)s2full_buff, CENTER_MODE);
        int s2half_int = (int)(s2half * 10);
        char s2half_buff[20];
        snprintf(s2half_buff, 110, "Half Step (ms): %d.%d", s2half_int / 10, s2half_int % 10); 
        LCD.DisplayStringAt(0, 110, (uint8_t *)s2half_buff, CENTER_MODE);
        ticker.detach();
        if (StepMode){
            speed = s2full;
            ticker.attach(&step1f, speed*0.004);
        }
        if(!StepMode){
            speed = s2half;
            ticker.attach(&step1h, speed*0.008);
        }
    }
}

void studentButtonPressed() {
    isStudent1 = !isStudent1; // Toggle between student 1 and student 2
    displayStudentInfo(); // Update the LCD with the new student's information
}

void displayStepInfo() {
    if (StepMode) {
        LCD.DisplayStringAt(0, 125, (uint8_t *)"Step Mode: Full Step", CENTER_MODE);
    } else {
        LCD.DisplayStringAt(0, 125, (uint8_t *)"Step Mode: Half Step", CENTER_MODE);
    }
}

void stepButtonPressed() {
    StepMode = !StepMode;
    displayStepInfo();
    ticker.detach();
    if (StepMode){
        ticker.attach(&step1f, speed*0.004);
    }
    if(!StepMode){
        ticker.attach(&step1h, speed*0.008);
    }
}

void displayDirectionInfo() {
    if (direction) {
        LCD.DisplayStringAt(0, 140, (uint8_t *)"Direction: C-W", CENTER_MODE);
    } if (!direction) {
        LCD.DisplayStringAt(0, 140, (uint8_t *)"Direction: CCW", CENTER_MODE);
    }
}

void dirButtonPressed() {
    direction = !direction;
    displayDirectionInfo();
}

void incrementSpeed() {
    speed = speed-100;
    LCD.DisplayStringAt(0, 155, (uint8_t *)"Motor Speed Up!", CENTER_MODE);
}

void decrementSpeed() {
    speed = speed+100;
    LCD.DisplayStringAt(0, 155, (uint8_t *)"Motor Speed Down!", CENTER_MODE);
}

void step1f(){
    In1 = 1;
    In2 = 0;
    In3 = 1;
    In4 = 0;
    if (direction==0){
        timeout.attach(&step2f, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step4f, speed*0.001);
    }
}

void step2f(){
    In1 = 1;
    In2 = 0;
    In3 = 0;
    In4 = 1;
    if (direction==0){
        timeout.attach(&step3f, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step1f, speed*0.001);
    }
}

void step3f(){
    In1 = 0;
    In2 = 1;
    In3 = 0;
    In4 = 1;
    if (direction==0){
        timeout.attach(&step4f, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step2f, speed*0.001);
    }
}

void step4f(){
    In1 = 0;
    In2 = 1;
    In3 = 1;
    In4 = 0;
    // if (direction==0){
    //     timeout.attach(&step1f, speed*0.001);
    // }
    if (direction==1){
        timeout.attach(&step3f, speed*0.001);
    }   
}

void step1h(){
    In1 = 1;
    In2 = 0;
    In3 = 1;
    In4 = 0;
    if (direction==0){
        timeout.attach(&step2h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step8h, speed*0.001);
    }
}

void step2h(){
    In1 = 1;
    In2 = 0;
    In3 = 0;
    In4 = 0;
    if (direction==0){
        timeout.attach(&step3h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step1h, speed*0.001);
    }
}

void step3h(){
    In1 = 1;
    In2 = 0;
    In3 = 0;
    In4 = 1;
    if (direction==0){
        timeout.attach(&step4h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step2h, speed*0.001);
    }
}

void step4h(){
    In1 = 0;
    In2 = 0;
    In3 = 0;
    In4 = 1;
    if (direction==0){
        timeout.attach(&step5h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step3h, speed*0.001);
    }
}

void step5h(){
    In1 = 0;
    In2 = 1;
    In3 = 0;
    In4 = 1;
    if (direction==0){
        timeout.attach(&step6h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step4h, speed*0.001);
    }
}

void step6h(){
    In1 = 0;
    In2 = 1;
    In3 = 0;
    In4 = 0;
    if (direction==0){
        timeout.attach(&step7h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step5h, speed*0.001);
    }
}

void step7h(){
    In1 = 0;
    In2 = 1;
    In3 = 1;
    In4 = 0;
    if (direction==0){
        timeout.attach(&step8h, speed*0.001);
    }
    if (direction==1){
        timeout.attach(&step6h, speed*0.001);
    }   
}

void step8h(){
    In1 = 0;
    In2 = 0;
    In3 = 1;
    In4 = 0;
    // if (direction==0){
    //     timeout.attach(&step1h, speed*0.001);
    // }
    if (direction==1){
        timeout.attach(&step7h, speed*0.001);
    }   
}

int main() {
    User_Button.rise(&studentButtonPressed);
    StepperMode.attach(&stepButtonPressed, IRQ_FALL, 50, false);
    SetDirection.attach(&dirButtonPressed, IRQ_FALL, 50, false);
    SpeedUp.attach(&incrementSpeed, IRQ_FALL, 50, false);
    SpeedDown.attach(&decrementSpeed, IRQ_FALL, 50, false);
    
    while (true) {
    }
}