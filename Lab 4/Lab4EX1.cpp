//Use g++ -std=c++11 -o Lab4EX1 Lab4EX1.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

int main(){
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

	while(serialDataAvail(kobuki) != -1){

		while(true){
		//If the bytes are a 1 followed by 15, then we are
		//parsing the basic sensor data packet
			read = serialGetchar(kobuki);
			if(read == 1){
				if(serialGetchar(kobuki) == 15) break;
			}
		}

		//Read past the timestamp
		serialGetchar(kobuki);
		serialGetchar(kobuki);

		/*Read the bytes containing the bumper, wheel drop,
			and cliff sensors. You can convert them into a usable data type.*/
		bumper = serialGetchar(kobuki);
		if(bumper >= 4){
			cout << "Left bumper ";
			bumper -= 4;
		}
		if(bumper >= 2){
			cout << "Central bumper ";
			bumper -= 2;
		}
		if(bumper == 1){
			cout << "Right bumper ";
			bumper -= 1;
		}

		drop = serialGetchar(kobuki);
		if(drop == 3){
			cout << "Right and left wheel off ";
		} else if(drop == 2){
			cout << "Left wheel off ";
		} else if(drop == 1){
			cout << "Right wheel off ";
		}

		cliff = serialGetchar(kobuki);
		if(cliff >= 4){
			cout << "Left cliff ";
			cliff -= 4;
		}
		if(cliff >= 2){
			cout << "Central cliff ";
			cliff -= 2;
		}
		if(cliff == 1){
			cout << "Right cliff ";
			cliff -= 1;
		}
		cout << endl;
		/*Print the data to the screen.*/
		//printf("Bumper: %d, Drop: %d, Cliff: %d\n", bumper, drop, cliff);

		/*Read through 6 bytes between the cliff sensors and
		the button sensors.*/
		for(int i = 0; i < 6; i++){
			serialGetchar(kobuki);
		}
		
		/*Read the byte containing the button data.*/
		button = serialGetchar(kobuki);
		/*Close the script and the connection to the Kobuki when
		Button 1 on the Kobuki is pressed. Use serialClose(kobuki);*/
		if(button == 2){
			serialClose(kobuki);
			exit(0);
		}

		

		//Pause the script so the data read receive rate is the same as the Kobuki send rate.
		usleep(20000);
			
		
	}

	return(0);
}
