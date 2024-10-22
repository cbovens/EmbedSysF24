// Use g++ -std=c++11 -o Lab4EX3 Lab4EX3.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

unsigned int bumper;
unsigned int drop;
unsigned int cliff;
unsigned int button;
unsigned int read;

int defSpeed = 100;
int kobukiRate = 20000;
int baud = 20;
int r = 500;
int bias = 230;
float packageParam = 1.4f;
void movement(int, int);
void readData();
void reposFunc(bool);
void rotateFunc(bool);
void linMoveFunc();

int main()
{
	// Create connection to the Kobuki
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);

	while (serialDataAvail(kobuki) != -1)
	{
		/*Read the initial data. If there are no flags,
		the default condition is forward.*/
		movement(defSpeed, 0);
		/*Move slowly to give the sensors enough time to read data,
		the recommended speed is 100mm/s*/
		if ((cliff < 8) && (bumper < 8))
		{
			if (((bumper & 0x01) == 0x01) || ((cliff & 0x01) == 0x01)) { //Cliff & bumper Right w/ bitmask
				reposFunc(false);
			}
			else if (((bumper & 0x02) == 0x02) || ((cliff & 0x02) == 0x02)) { //Cliff & bumper Center w/ bitmask
				reposFunc(true);
			}
			else if (((bumper & 0x04) == 0x04) || ((cliff & 0x04) == 0x04)) { //Cliff & bumper Left w/ bitmask
				reposFunc(true);
			}		
		}
		/*Create different states as to satisfy the conditions above.
		Remember, a single press of a bumper may last longer
		than one data cycle.*/

		/*Cleanly close out of all connections using Button 1.*/
		if (button == 2)
		{
			movement(0, 0);
			serialClose(kobuki);
		}
		/*Use serialFlush(kobuki) to discard all data received, or waiting to be send down the given device.*/
		usleep(kobukiRate);
		serialFlush(kobuki);
		readData();
	}
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
	usleep(kobukiRate);
}

void readData()
{
	// you can reuse your code from EXE1, Lab 4
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	unsigned int bumper;
	unsigned int drop;
	unsigned int cliff;
	unsigned int button;
	unsigned int read;
	unsigned int left1;
	unsigned int left2;
	unsigned int right1;
	unsigned int right2;

	while (true)
	{
		// If the bytes are a 1 followed by 15, then we are
		// parsing the basic sensor data packet
		read = serialGetchar(kobuki);
		if (read == 1)
		{
			if (serialGetchar(kobuki) == 15)
				break;
		}
	}

	// Read past the timestamp
	serialGetchar(kobuki);
	serialGetchar(kobuki);

	/*Read the bytes containing the bumper, wheel drop,
		and cliff sensors. You can convert them into a usable data type.*/
	bumper = serialGetchar(kobuki);
	/* if (bumper >= 4)
   {
	   cout << "Left bumper ";
	   bumper -= 4;
   }
   if (bumper >= 2)
   {
	   cout << "Central bumper ";
	   bumper -= 2;
   }
   if (bumper == 1)
   {
	   cout << "Right bumper ";
	   bumper -= 1;
   } */

	drop = serialGetchar(kobuki);
	/* if (drop == 3)
	{
		cout << "Right and left wheel off ";
	}
	else if (drop == 2)
	{
		cout << "Left wheel off ";
	}
	else if (drop == 1)
	{
		cout << "Right wheel off ";
	} */

	cliff = serialGetchar(kobuki);
	/*if (cliff >= 4)
	{
		cout << "Left cliff ";
		cliff -= 4;
	}
	if (cliff >= 2)
	{
		cout << "Central cliff ";
		cliff -= 2;
	}
	if (cliff == 1)
	{
		cout << "Right cliff ";
		cliff -= 1;
	}
	cout << endl;
	*/
	/*Print the data to the screen.*/
	// printf("Bumper: %d, Drop: %d, Cliff: %d\n", bumper, drop, cliff);

	/*Read through 6 bytes between the cliff sensors and
	the button sensors.*/

	for (int i = 0; i < 6; i++)
	{
		serialGetchar(kobuki);
	}

	/*Read the byte containing the button data.*/
	button = serialGetchar(kobuki);
	/*Close the script and the connection to the Kobuki when
	Button 1 on the Kobuki is pressed. Use serialClose(kobuki);*/
	if (button == 2)
	{
		serialClose(kobuki);
		exit(0);
	}

	cout << "bumper: " << bumper << " drop: " << drop << " cliff: " << cliff << " button: " << button << endl;
	return (0);
}

void rotateFunc(bool clockwise) {
	int turnCond = clockwise ? 1: -1;
	float turnSpeed = 3.14159f / 2;
	turnSpeed *= turnCond * packageParam;
	for (int i = 0; i < (1000 / baud), i++) {
		movement(turnSpeed, 1);
	}
	movement(0,0);
}

void linMoveFunc() {
	int linMoveSpeed = (int)(-1 * defSpeed * packageParam);
		for (int i = 0; i < 500 / baud; i++) {
		movement(linMoveSpeed, 0);
		}
	movement(0,0);
}

void reposFunc(bool clockwise) {
	linMoveFunc();
	rotateFunc(clockwise);
}
