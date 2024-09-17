//Use g++ -std=c++11 -o Lab2EX1 Lab2EX1.cpp -lwiringPi

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point t1, t2;
int pulseWidth;
double tInMax = 18500;

int main(){
	//Set up wiringPi
	wiringPiSetup();
	
	while(true){
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
		while(digitalRead(1))
		{
			// 2. define a varable to get the current time t2.
			t2 = high_resolution_clock::now();
			// 3. calculate the time duration: t2 - t1
			pulseWidth = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
			// 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
			if(pulseWidth > tInMax){
				break;
			}
		}



		/*Calculate the distance by using the time duration that you just obtained.*/ //Speed of sound is 340m/s
		double distancecm = (340.0 * pulseWidth * 0.0001) / 2.0;


		/*Print the distance.*/

		cout << distancecm << endl;

		/*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
		usleep(50000);

        }
}
