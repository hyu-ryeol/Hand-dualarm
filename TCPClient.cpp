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
#include <signal.h>
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

float T_kp[4] = {10,10,10,10};
float I_kp[4] = {10,10,10,10};
float M_kp[4] = {10,10,10,10};
float R_kp[4] = {10,10,10,10};

//float T_kd[4] = {0,0,0,0};
//float I_kd[4] = {0,0,0,0};
//float M_kd[4] = {0,0,0,0};
//float R_kd[4] = {0,0,0,0};



float T_kd[4] = {1,1,1,1};
float I_kd[4] = {1,1,1,1};
float M_kd[4] = {1,1,1,1};
float R_kd[4] = {1,1,1,1};




uint16_t T_target[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t I_target[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t M_target[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t R_target[4] = {0x8000, 0x8000, 0x8000, 0x8000};

//
//uint16_t T_target[4] = {0x000, 0x000, 0x000, 0x000};
//uint16_t I_target[4] = {0x000, 0x000, 0x000, 0x000};
//uint16_t M_target[4] = {0x000, 0x000, 0x000, 0x000};
//uint16_t R_target[4] = {0x000, 0x000, 0x000, 0x000};

volatile uint16_t T_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
volatile uint16_t I_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
volatile uint16_t M_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
volatile uint16_t R_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preT_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preI_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preM_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preR_pos[4] = {0x8000, 0x8000, 0x8000, 0x8000};


uint16_t preT_err[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preI_err[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preM_err[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t preR_err[4] = {0x8000, 0x8000, 0x8000, 0x8000};
int T_Err[4] = {0,0,0,0};
int I_Err[4] = {0,0,0,0};
int M_Err[4] = {0,0,0,0};
int R_Err[4] = {0,0,0,0};
float T_torque[4] = {0x8000, 0x8000, 0x8000, 0x8000};
float I_torque[4] = {0x8000, 0x8000, 0x8000, 0x8000};
float M_torque[4] = {0x8000, 0x8000, 0x8000, 0x8000};
float R_torque[4] = {0x8000, 0x8000, 0x8000, 0x8000};
uint16_t offset = 0x8000;
uint8_t dataDivide_flag = 0; // 0: motor , 1: sensor

bool rxSuccess_flag = false;
bool prePos_flag = false;


const std::string hostname = "169.254.186.72"; //STEP2 IP Address

const Poco::UInt16 PORT = 7;
StreamSocket ss;

enum {
    SIZE_HEADER = 52,
    SIZE_COMMAND = 4,
    SIZE_HEADER_COMMAND = 56,
    SIZE_DATA_MAX = 33,
    SIZE_DATA_ASCII_MAX = 32
};

void ctrlchandler(int)
{
    unsigned char writeBuff[33];
    writeBuff[0] = 0x00;
    for(int i=0 ; i<16 ; i++){
        writeBuff[i*2+1] = 0x80;
        writeBuff[i*2+2] = 0x00;
    }
    ss.sendBytes(writeBuff,33,0);
    usleep(2000);
    ss.sendBytes(writeBuff,33,0);
    usleep(2000);
    ss.close();
    cout << "EXIT..." << endl;

    exit(EXIT_SUCCESS);
}

void killhandler(int)
{
    unsigned char writeBuff[33];
    writeBuff[0] = 0x00;
    for(int i=0 ; i<16 ; i++){
        writeBuff[i*2+1] = 0x80;
        writeBuff[i*2+2] = 0x00;
    }
    ss.sendBytes(writeBuff,33,0);
    usleep(2000);
    ss.sendBytes(writeBuff,33,0);
    usleep(2000);
    ss.close();

    cout << "EXIT..." << endl;

    exit(EXIT_SUCCESS);
}





class HYUControl: public Poco::Runnable{
    virtual void run(){
        unsigned char writeBuff[33];
        writeBuff[0] = 0x00;
        for(int i=0 ; i<16 ; i++){
            writeBuff[i*2+1] = 0x80;
            writeBuff[i*2+2] = 0x00;
        }
        ss.sendBytes(writeBuff,33,0);

        while(1){
            unsigned char receiveBuff[33];
            ss.receiveBytes(receiveBuff,33,0);
            for(int i=0 ; i<4 ; i++){ // 32/2/4 -> 32/8 -> 4
                T_pos[i] = ((receiveBuff[i*2] << 8) & 0xFF00) | (receiveBuff[i*2+1] & 0x00FF);
                I_pos[i] = ((receiveBuff[i*2+8] << 8) & 0xFF00) | (receiveBuff[i*2+9] & 0x00FF);
                M_pos[i] = ((receiveBuff[i*2+16] << 8) & 0xFF00) | (receiveBuff[i*2+17] & 0x00FF);
                R_pos[i] = ((receiveBuff[i*2+24] << 8) & 0xFF00) | (receiveBuff[i*2+25] & 0x00FF);
            }
            for(int i=0; i<4; i++){
                T_Err[i] = T_target[i] - T_pos[i];
                I_Err[i] = I_target[i] - I_pos[i];
                M_Err[i] = M_target[i] - M_pos[i];
                R_Err[i] = R_target[i] - R_pos[i];
            }

            cout<<"T_Err : "<<T_Err[0]<<","<<T_Err[1]<<","<<T_Err[2]<<","<<T_Err[3] <<endl;
            cout<<"I_Err : "<<I_Err[0]<<","<<I_Err[1]<<","<<I_Err[2]<<","<<I_Err[3] <<endl;
            cout<<"M_Err : "<<M_Err[0]<<","<<M_Err[1]<<","<<M_Err[2]<<","<<M_Err[3] <<endl;
            cout<<"R_Err : "<<R_Err[0]<<","<<R_Err[1]<<","<<R_Err[2]<<","<<R_Err[3] <<endl;

            for(int i=0; i<4; i++){
                T_torque[i] = (float)(T_kp[i]*(T_Err[i])+T_kd[i]*(-1*(T_pos[i]-preT_pos[i])/0.002))+offset; //offset = 0x8000
                I_torque[i] = (float)(I_kp[i]*(I_Err[i])+I_kd[i]*(-1*(I_pos[i]-preI_pos[i])/0.002))+offset;
                M_torque[i] = (float)(M_kp[i]*(M_Err[i])+M_kd[i]*(-1*(M_pos[i]-preM_pos[i])/0.002))+offset;
                R_torque[i] = (float)(R_kp[i]*(R_Err[i])+R_kd[i]*(-1*(R_pos[i]-preR_pos[i])/0.002))+offset;

                if(T_torque[i]<0) T_torque[i] = 0;
                if(I_torque[i]<0) I_torque[i] = 0;
                if(M_torque[i]<0) M_torque[i] = 0;
                if(R_torque[i]<0) R_torque[i] = 0;

                if(T_torque[i]>65534) T_torque[i] = 65534;
                if(I_torque[i]>65534) I_torque[i] = 65534;
                if(M_torque[i]>65534) M_torque[i] = 65534;
                if(R_torque[i]>65534) R_torque[i] = 65534;
            }

            for(int i=0 ; i<4 ; i++){
                preT_pos[i] = T_pos[i];
                preI_pos[i] = I_pos[i];
                preM_pos[i] = M_pos[i];
                preR_pos[i] = R_pos[i];
            }


            unsigned char TIMR_Duty[33];
            TIMR_Duty[0] = 0x00; // ID
            for(int i=0; i<4; i++){
                TIMR_Duty[i*2+1] = ((int)T_torque[i] >> 8) & 0x00FF;
                TIMR_Duty[i*2+2] = (int)T_torque[i] & 0x00FF;

                TIMR_Duty[i*2+9] = ((int)I_torque[i] >> 8) & 0x00FF;
                TIMR_Duty[i*2+10] = (int)I_torque[i] & 0x00FF;

                TIMR_Duty[i*2+17] = ((int)M_torque[i] >> 8) & 0x00FF;
                TIMR_Duty[i*2+18] = (int)M_torque[i] & 0x00FF;

                TIMR_Duty[i*2+25] = ((int)R_torque[i] >> 8) & 0x00FF;
                TIMR_Duty[i*2+26] = (int)R_torque[i] & 0x00FF;
            }

            ss.sendBytes(TIMR_Duty,33,0);
        }

    }

};
union Data
        {
    unsigned char byte[SIZE_DATA_MAX];
        };

int main(int argc, char **argv)
{

    Data data_rev;


    signal(SIGINT, ctrlchandler);
    signal(SIGTERM, killhandler);

    HYUControl hyu;
    Poco::Thread thread;

    try
    {
        cout << "Trying to connect server..." << endl;
        ss.connect(SocketAddress(hostname, PORT));

        Timespan timeout(1, 0);
        while (ss.poll(timeout, Poco::Net::Socket::SELECT_WRITE) == false)
        {
            cout << "Connecting to server..." << endl;
        }
        thread.start(hyu);
        int count = 0;
        while(1){
            if(count>=1){
                T_target[3] = 20816; // DIP
                T_target[2] = 16158; // PIP
                T_target[1] = 25542; // MCP
                T_target[0] = 43629; // A/A

                I_target[3] = 52519;
                I_target[2] = 11713;
                I_target[1] = 10521;
                I_target[0] = 28258;

                M_target[3] = 52519;
                M_target[2] = 52519;
                M_target[1] = 52519;
                M_target[0] = 52519;

                R_target[3] = 32768;
                R_target[2] = 32768;
                R_target[1] = 32768;
                R_target[0] = 32768;
            }

            usleep(50000);
            count++;
        }
    }
    catch (Poco::Exception& exc)
    {
        cout << "Fail to connect server..." << exc.displayText() << endl;
    }
    ss.close();
    thread.join();


    return 0;
}


