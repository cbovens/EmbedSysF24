// Use g++ joystick.cc -std=c++11 -o Lab3EX3B Lab3EX3B.cpp
// Client

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include "joystick.hh"
#include <cstdlib>
#define PORT 8080
using namespace std;

int createSocket();
int num = 0;
int val = 0;
int sock = 0;
int isButton = 0;
int *data = new int[3];

int main(int argc, char const *argv[])
{

	// Open the file stream for the joystick
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	if (!joystick.isFound())
	{
		cout << "Error opening joystick" << endl;
		exit(1);
	}

	// Create the connection to the server
	createSocket();

	while (true)
	{

		/*Sample the events from the joystick*/
		if (joystick.sample(&event))
		{
			/*Convert the event to a useable data type so it can be sent*/
			// isButton = event.isButton();
			data[0] = 0;
			data[1] = 0;
			data[2] = 0;
			num = event.number;
			val = event.value;

			/*Print the data stream to the terminal*/

			if (event.isButton())
			{
				printf("Button Press: eventNum: %u | value: %d", event.number, event.value);
				/*Interpret the joystick input and use that input to move the Kobuki*/
				if (event.number == 7 && event.value == 1) // Stop program signal
				{
					data[2] = 1;
				}
				if (event.number == 6 && event.value == 1) // Stop movement signal
				{
					data[0] = 0;
					data[1] = 0;
					data[2] = 0;
				}
			}
			if (event.isAxis())
			{
				printf("Axis Press: isAxis: %u | Value: %d\n", event.number, event.value);
				if (event.number == 6)
				{
					/*Interpret the joystick input and use that input to move the Kobuki*/
					if (val == 32767)
					{
						// move right
						// movement(120, -1);
						data[0] = 120;
						data[1] = -1;
						data[2] = 0;
					}
					else if (val == -32767)
					{
						// move left
						// movement(120, 1);
						data[0] = 120;
						data[1] = 1;
						data[2] = 0;
					}
				}
				else if (event.number == 7)
				{
					if (val == -32767)
					{
						// move up
						// movement(radius, 0);
						data[0] = 500;
						data[1] = 0;
						data[2] = 0;
					}
					else if (val == 32767)
					{
						// move down
						// movement(-radius, 0);
						data[0] = -500;
						data[1] = 0;
						data[2] = 0;
					}
				}
			}
			printf("Velocity: %d | Radius: %d | Exit: %d\n", data[0], data[1], data[2]);
			send(sock, data, 16, 0);
		}

		/*Send the data to the server*/

		if (data[2] == 1)
		{
			/*Closes out of all connections cleanly*/

			// When you need to close out of the connection, please
			// close TTP/IP data streams.
			// Not doing so will result in the need to restart
			// the raspberry pi and Kobuki
			close(sock);
			exit(0);

			/*Set a delay*/
			sleep(2);
		}
	}
	return 0;
}

// Creates the connection between the client and
// the server with the controller being the client
int createSocket()
{
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\nSocket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	/*Use the IP address of the server you are connecting to*/
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{ // Will change!!!
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	return 0;
}
