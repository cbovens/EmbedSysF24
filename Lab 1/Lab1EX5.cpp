// g++ -std=c++11 -o Lab1EX5 Lab1EX5.cpp -lwiringPi
// Evelyn Wilson and Collin Bovenschen
// Lab 1 Exercise 5
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <iostream>
#include <softPwm.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <signal.h>
#include <iomanip>
#include <unistd.h>

#define buttonPin 4
#define SERVO_MIN_MS 5
#define SERVO_MAX_MS 25
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
using namespace std;

int adcVal(); //was int originally??
int angM;
bool countClockula = true;
/* signal pin of the servo*/
#define servoPin 1 // GPIO18

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, int angle){ 
    long time = 0;
    time = SERVO_MIN_MS + (int)10*((float)angle/(SERVO_MAX_ANGLE/2)); /* map the desired angle to time*/
    softPwmWrite(pin,time);   
}


/* Define your callback function to handout the pressing button interrupts. */
void press_button()
{
    countClockula = !countClockula;
}




int main(void)
{
    wiringPiSetup();
    /* Use wiringPiISR() to setup your interrupts. Refer to document WiringPi_ Interrupts.pdf. */
    wiringPiISR(buttonPin, INT_EDGE_RISING, &press_button);
    softPwmCreate(servoPin,  0, 200);
    
    while(1){

        /* read ADS1015 value */
	//cout << adcVal()<<endl;
	
        /* convert the obtained ADS1015 value to angle 0 - 180*/
	angM = int(180*(1.0*adcVal() / 1613)); //convert max 6.144 to angles
	cout << angM <<endl;
    if(countClockula) {
        servoWrite(servoPin, angM);
    } else {
        servoWrite(servoPin, 180 - angM);
    }
        /* use the angle to control the servo motor*/

        usleep(100000);

    }
    return 0;
}

//This function is used to read data from ADS1015
int adcVal(){

	uint16_t low, high, value;
	// Refer to the supplemental documents to find the parameters. In this lab, the ADS1015
	// needs to be set in single conversion, single-end mode, FSR (full-scale range)is 6.144, you can choose 
	// any input pin (A0, A1, A2, A3) you like.
	int adc = wiringPiI2CSetup(0x48);	//double right?? Or int???
	wiringPiI2CWriteReg16(adc, 0x01, 0xC5C1); //check codes!!!
	usleep(1000);
    uint16_t data = wiringPiI2CReadReg16(adc, 0x00);


    low = (data & 0xFF00) >> 8;
    high = (data & 0x00FF) << 8;
    value = (high | low)>>4;
	return value;

}
