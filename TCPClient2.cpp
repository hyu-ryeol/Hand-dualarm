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
const std::string hostname = "169.254.186.72"; //STEP2 IP Address

//const std::string hostname = "192.168.0.100"; //STEP2 IP Address Monkey
//const std::string hostname = "192.168.1.18"; //STEP2 IP Address Tensegrity
//const std::string hostname = "192.168.0.122"; //STEP2 IP Address Tensegrity
const Poco::UInt16 PORT = 7;

enum {
	SIZE_HEADER = 52,
	SIZE_COMMAND = 4,
	SIZE_HEADER_COMMAND = 56,
	SIZE_DATA_MAX = 33,
	SIZE_DATA_ASCII_MAX = 32
};


union Data
{
	unsigned char byte[SIZE_DATA_MAX];
};

int main(int argc, char **argv)
{
	StreamSocket ss;

	Data data;
	unsigned char writeBuff[33];
	
	try
	{
		cout << "Trying to connect server..." << endl;
		ss.connect(SocketAddress(hostname, PORT));

		Timespan timeout(1, 0);
		while (ss.poll(timeout, Poco::Net::Socket::SELECT_WRITE) == false)
		{
			cout << "Connecting to server..." << endl;
		}
		writeBuff[0] =  0x00;
		    for(int i=0 ; i<16 ; i++){
			writeBuff[i*2+1] = 0x80;
			writeBuff[i*2+2] = 0x00;
		    }
		memcpy(writeBuff, data.byte, SIZE_DATA_MAX);    
	        ss.sendBytes(writeBuff, 1024);
//	        ss.sendBytes(writeBuff, 33);
		cout << "Success" << endl;
		ss.close();
	}
	catch (Poco::Exception& exc)
	{
		cout << "Fail to connect server..." << exc.displayText() << endl;
	}
	return 0;
}


