// use g++ -std=c++11 -o Lab4EX2 Lab4EX2.cpp -lwiringPi

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define sonarPin 1 // Sonar

int kobuki;
float distance_previous_error, distance_error, cumError, rateError = 0;
float obj_value, measured_value; // potentiometer reading, sonar reading
int time_inter_ms = 23;			 // time interval, you can use different time interval
high_resolution_clock::time_point t1, t2;
int pulseWidth;
int sonar_pin = sonarPin;
double tInMax = 18500;

float read_sonar();
float read_sonar();
void movement(int, int);

int main()
{
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);

	/*Move from a random point within the area designated "X" to the
	point B as shown on the diagram. Use a sonar sensor to navigate through the channel.
	You can reuse your code from Lab 2 and 3*/

	/*Note: the Kobuki must completely pass point B as shown to receive full credit*/
	while (read_sonar() >= 30)
	{
		movement(150, 0);
		cout << "IN LOOP!!! YAY!!!" << endl;
	}
	movement(80, -1);
	sleep(3);
	while (read_sonar() >= 30)
	{
		movement(200, 0);
		cout << "SECOND LOOP??? HUH???" << endl;
	}
	movement(80, 1);
	sleep(3);
	while (read_sonar() >= 20)
	{
		movement(200, 0);
	}
	movement(0, 0);
	/*while (1)
	{
		cout << read_sonar() << endl;
	}*/
}

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
}

void movement(int sp, int r)
{
	// Reused from Lab 3
	//  Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/
	unsigned char b_3 = 0x01; /*Byte 3: Sub-Payload Header*/
	unsigned char b_4 = 0x04; /*Byte 4: Length of Sub-Payload*/

	unsigned char b_5 = sp & 0xff;		  // Byte 5: Payload Data: Speed(mm/s)
	unsigned char b_6 = (sp >> 8) & 0xff; // Byte 6: Payload Data: Speed(mm/s)
	unsigned char b_7 = r & 0xff;		  // Byte 7: Payload Data: Radius(mm)
	unsigned char b_8 = (r >> 8) & 0xff;  // Byte 8: Payload Data: Radius(mm)
	unsigned char checksum = 0;			  // Byte 9: Checksum

	// Checksum all of the data
	char packet[] = {b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7, b_8};
	for (unsigned int i = 2; i < 9; i++)
		checksum ^= packet[i];

	/*Send the data (Byte 1 - Byte 9) to Kobuki using serialPutchar (kobuki, );*/
	for (int i = 0; i < 9; i++)
	{
		serialPutchar(kobuki, packet[i]);
	}
	serialPutchar(kobuki, checksum);

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);
}