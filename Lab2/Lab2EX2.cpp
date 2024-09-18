// Use g++ -std=c++11 -o Lab2EX2 Lab2EX2.cpp -lwiringPi

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <chrono>
#include <cmath>

using namespace std::chrono;
using namespace std;

// functions
void sigroutine(int);
int adcVal();
void PID(float, float, float);
float read_potentiometer();
float read_sonar();

#define sonarPin 1 // Sonar
#define sdaPin 8   // SDA
#define sclPin 9   // SCL
#define fanPin 26  // fan

// variables
float distance_previous_error, distance_error, cumError, rateError = 0;
float obj_value, measured_value; // potentiometer reading, sonar reading
int adc;
float PID_p, PID_d, PID_total, PID_i = 0;
int time_inter_ms = 23; // time interval, you can use different time interval

// From EX1
high_resolution_clock::time_point t1, t2;
int pulseWidth;
double tInMax = 18500;

/*set your pin numbers and pid values*/
int motor_pin = fanPin;
int sonar_pin = sonarPin;
float kp = 20;	 // try 20
float ki = .001; // try 5
float kd = 10;	 // try 10

int main()
{
	wiringPiSetup();
	adc = wiringPiI2CSetup(0x48);

	/*Set the pinMode (fan pin)*/
	pinMode(fanPin, PWM_OUTPUT);

	// This part is to set a system timer, the function "sigroutine" will be triggered
	// every time_inter_ms milliseconds.
	struct itimerval value, ovalue;
	signal(SIGALRM, sigroutine);
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = time_inter_ms * 1000;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = time_inter_ms * 1000;
	setitimer(ITIMER_REAL, &value, &ovalue);

	while (true)
	{
		cout << "obj_value: " << obj_value << " measured_value: " << measured_value << endl;
		cout << "PID_p: " << PID_p << endl;
		cout << "PID_i: " << PID_i << endl;
		cout << "PID_d: " << PID_d << endl;
		cout << "PID_total: " << PID_total << endl;
		usleep(100000);
	}
}

void sigroutine(int signo)
{
	PID(kp, ki, kd);
	return;
}

/* based on the obj distance and measured distance, implement a PID control algorithm to
the speed of the fan so that the Ping-Pang ball can stay in the obj position*/
void PID(float kp, float ki, float kd)
{
	/*read the objective position/distance of the ball*/
	obj_value = read_potentiometer();
	/*read the measured position/distance of the ball*/
	measured_value = read_sonar();							// Change adcVal to a distance!
															/*calculate the distance error between the obj and measured distance */
	distance_error = (obj_value - measured_value) / 100.0f; // changed to meters
	cumError += (distance_error * (time_inter_ms / 1000.0f));
	rateError = (distance_error - distance_previous_error) / (time_inter_ms / 1000.0f);
	// distance_error = objDist - measured_value; Evelyn wrote this maybe?
	/*calculate the proportional, integral and derivative output */

	PID_p = kp * distance_error;
	PID_i = ki * cumError;	// make sure time_inter_ms is elapsedTime
	PID_d = kd * rateError; // How do I get last error???
	PID_total = PID_p + PID_d + PID_i;

	// cout << "P float:" << PID_total << endl;

	/*assign distance_error to distance_previous_error*/
	distance_previous_error = distance_error;
	/*use PID_total to control your fan*/
	pwmWrite(fanPin, (int)PID_total);
	// pwmWrite(fanPin, (int)1000);
}

/* use a sonar sensor to measure the position of the Ping-Pong ball. you may reuse
your code in EX1.*/
float read_sonar()
{
	/*Set the pinMode to output and generate a LOW-HIGH-LOW signal using "digitalWrite" to trigger the sensor.
	Use a 2 us delay between a LOW-HIGH and then a 5 us delay between HIGH-LOW. You can use
	the function "usleep" to set the delay. The unit of usleep is microsecond. */
	pinMode(1, OUTPUT);
	digitalWrite(1, LOW);
	usleep(2);
	digitalWrite(1, HIGH);
	usleep(5);
	digitalWrite(1, LOW);

	/*Echo holdoff delay 750 us*/
	usleep(750);

	/*Switch the pinMode to input*/
	pinMode(1, INPUT);

	/*Get the time it takes for signal to leave sensor and come back.*/

	// 1. define a varable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
	t1 = high_resolution_clock::now();
	while (digitalRead(sonarPin))
	{
		// 2. define a varable to get the current time t2.
		t2 = high_resolution_clock::now();
		// 3. calculate the time duration: t2 - t1
		pulseWidth = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
		// 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
		if (pulseWidth > tInMax)
		{
			break;
		}
	}

	/*Calculate the distance by using the time duration that you just obtained.*/ // Speed of sound is 340m/s
	float distancecm = (340.0 * pulseWidth * 0.0001) / 2.0;

	// cout << distancecm << endl;
	return distancecm;

	/*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
}

/* use a potentiometer to set an objective position (10 - 90 cm) of the Ping-Pang ball, varying the potentiometer
can change the objective distance. you may reuse your code in Lab 1.*/
float read_potentiometer()
{
	float objPos = ((adcVal() / 1613.0f) * 80.0f) + 10.0f; // 1613 - max dc	80 - Max - min vals,	10 - min val
	return objPos;										   // in cm
}

int adcVal()
{

	uint16_t low, high, value;
	// Refer to the supplemental documents to find the parameters. In this lab, the ADS1015
	// needs to be set in single conversion, single-end mode, FSR (full-scale range)is 6.144, you can choose
	// any input pin (A0, A1, A2, A3) you like.
	wiringPiI2CWriteReg16(adc, 0x01, 0xC5C1);
	usleep(1000);
	uint16_t data = wiringPiI2CReadReg16(adc, 0x00);

	low = (data & 0xFF00) >> 8;
	high = (data & 0x00FF) << 8;
	value = (high | low) >> 4;
	return value;
}
