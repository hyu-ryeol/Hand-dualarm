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

const std::string hostname = "192.168.0.39"; //STEP2 IP Address 

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

	float float6dArr[9];

};

 

int main()

{

	StreamSocket ss;

	int cnt = 0;

	int flag_return = 0;

	int rpt_cnt = 1;

	clock_t start, check, end, init;

	int motion_buf = 0;

	double glob_time, loop_time;

	Data data_rev, data;

	unsigned char readBuff[1024];

	unsigned char writeBuff[1024];

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

 

		cout << "=========== Please enter the packet ============" << endl;

		cout << "Packet scheme\n| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |" << endl;

		cout << "0 : Motion number, 1~6 : Desired data, 7 : Null" << endl;

		cout << "================================================" << endl;

 

		while (true)

		{

			cout << rpt_cnt << " : ";

			try {

				cin >> data.float6dArr[0] >> data.float6dArr[1] >> data.float6dArr[2] >> data.float6dArr[3] >> data.float6dArr[4] >>

					data.float6dArr[5] >> data.float6dArr[6] >> data.float6dArr[7];

				memcpy(writeBuff, data.byte, SIZE_DATA_MAX);

				if ((int)data.float6dArr[0] < 100)

				{

					throw (int)data.float6dArr;

				}

				ss.sendBytes(writeBuff, 1024);

			}

			catch (int expn) {

				cout << "[ERROR] : Please check the Motion" << endl;

				return 0;

			}

			if (data.float6dArr[6] < 1.5 && (int)data.float6dArr[0]==106)

				cout << "[WARNING] : Please enter a trajectory time longer than 1.5s" << endl;

			if ((int)data.float6dArr[0] == 101 && motion_buf == 101)

			{

				cout << "[WARNING] : Homming has already done" << endl;

			}

			motion_buf = (int)data.float6dArr[0];

 

			while (ss.poll(timeout, Poco::Net::Socket::SELECT_READ) == false)

			{

				if (cnt >= 5) {

					flag_return = 1;

					break;

				}

				cout << "Waiting to receive data..." << endl;

				cnt++;

			}

			if (flag_return == 0)

			{

				ss.receiveBytes(readBuff, 1024);

				memcpy(data_rev.byte, readBuff, SIZE_DATA_MAX);

				if ((int)data_rev.float6dArr[0] == 101 && (int)data_rev.float6dArr[1] == 1)

				{

					cout << "Homing is done" << endl;

				}

				else if ((int)data_rev.float6dArr[0] == 102 && (int)data_rev.float6dArr[1] == 1)

				{

					cout << "Limit is occured" << endl;

				}

 

			}

			else

			{

				cout << "No response from server..." << endl;

				break;

			}

			rpt_cnt++;

		}

		ss.close();

	}

	catch (Poco::Exception& exc)

	{

		cout << "Fail to connect server..." << exc.displayText() << endl;

	}

	return 0;

}

 
