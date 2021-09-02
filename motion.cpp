/* 
* TCP Example For KIST Monkey Experiment 
* TCP_HYUSPA.cpp
* Created on: Mar 2, 2020
*     Author: Sunhong Kim
*/

#include "Poco/Net/Net.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Exception.h"
#include "Poco/Timer.h"
#include "Poco/Stopwatch.h"
#include "Poco/Thread.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"
#include "Poco/NumericString.h"
#include <iostream>
#include <time.h>
#include <unistd.h>
using namespace Poco;
using namespace Poco::Dynamic;
using Poco::Net::SocketAddress;
using Poco::Net::StreamSocket;
using Poco::Net::Socket;
using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;
using Poco::Stopwatch;
using namespace std;

//const std::string hostname = "127.0.0.1"; //localhost IP Address
//const std::string hostname = "192.168.0.39"; //STEP2 IP Address 
const std::string hostname = "192.168.0.100"; //STEP2 IP Address 

//const std::string hostname = "192.168.0.100"; //STEP2 IP Address Monkey
//const std::string hostname = "192.168.1.18"; //STEP2 IP Address Tensegrity
//const std::string hostname = "192.168.0.122"; //STEP2 IP Address Tensegrity
const Poco::UInt16 PORT = 9911;

enum {
	SIZE_HEADER = 52,
	SIZE_COMMAND = 4,
	SIZE_HEADER_COMMAND = 56,
	SIZE_DATA_MAX = 200,
	SIZE_DATA_ASCII_MAX = 32
};


union Data
{
	unsigned char byte[SIZE_DATA_MAX];
	float value[9];
};

int main(int argc, char **argv)
{

    
    
	StreamSocket ss;
	float box_trajectory_y[8]={0.5,0.5,0.5,0,-0.5,-0.5,-0.5,0};
	float box_trajectory_x[8]={2,2.5,3,3,3,2.5,2,2};
	
	Data data;
	unsigned char writeBuff[1024];
	while(true){

		int motion_number =1;
		//cout<<"Motion Number : 1 2 3 4 5 "<<endl;
		//cin >>motion_number;
		switch(motion_number){
			case 1:
				for(int j =0;j<8;j++){
					float base_x = box_trajectory_x[j];
					float base_y = box_trajectory_y[j];
					
					try
					{
						cout << "Trying to connect server..." << endl;
						ss.connect(SocketAddress(hostname, PORT));

						Timespan timeout(1, 0);
						while (ss.poll(timeout, Poco::Net::Socket::SELECT_WRITE) == false)
						{
							cout << "Connecting to server..." << endl;
						}
						cout << "Complete to connect server" << endl;
						cout << "-----Send Data------" << endl;
						try {
							data.value[0]=base_x;
							data.value[1]=base_y;
							data.value[2]=0;
							for(int j = 0;j<6;j++)
								data.value[3+j]=0;
					
							
							for(int j = 0;j<9;j++)
							       cout<<data.value[j]<<endl;
						       cout << "-------------------" << endl;
							memcpy(writeBuff, data.byte, SIZE_DATA_MAX);
							ss.sendBytes(writeBuff, 1024);
						}
						catch (int expn) {
							cout << "[ERROR] : Please check the Motion" << endl;
							return 0;
						}
						cout << endl;
						ss.close();
					}
					catch (Poco::Exception& exc)
					{
						cout << "Fail to connect server..." << exc.displayText() << endl;
					}
					usleep(3000000);
				}
					
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
		
		}
		usleep(5000000);
	}
	return 0;
}

