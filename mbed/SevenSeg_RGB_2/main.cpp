#include "mbed.h"
#include <stdio.h>
#include <string>

//pin
DigitalOut mPin_1( dp1);
DigitalOut mPin_2( dp2);
DigitalOut mPin_4( dp4);
DigitalOut mPin_5( dp5);
DigitalOut mPin_6( dp6);
DigitalOut mPin_9( dp9);
DigitalOut mPin_10(dp10);

DigitalOut mPin_11(dp11);
DigitalOut mPin_13(dp13);
DigitalOut mPin_14(dp14);
DigitalOut mPin_17(dp17);

DigitalOut mDIN_H(dp25);
DigitalOut mDIN_L(dp26);


//pin
DigitalOut mAnode[4] ={mPin_6, mPin_10, mPin_11, mPin_17 };
DigitalOut mCathod[7]=      {mPin_1, mPin_2, mPin_4, mPin_5, mPin_9, mPin_13, mPin_14 };
int mCount=0;
int   mCathod_pin[7]  ={6, 4, 2, 1, 0, 5, 3 };
int   mAnode_pin[4]   ={0, 1, 2, 3  };

Serial mPc(USBTX, USBRX);

//define
const int mBit_MAX=8;
const int mLED_MAX=3;
const int mWait_US=5;

const unsigned char mColorMap[9][3] = {
  {0xff,0xa5,0x00 }, // #ffa500 Orange
  {0xd2,0x69,0x1e }, // #d2691e chocolate
  {0x80,0x00,0x00 }, // #800000 red 
  {0x80,0x00,0x80 }, // #800080 purple
  {0x00,0x80,0x00 }, // #008000 green
  {0xff,0xff,0x00 }, // #ffff00 Yellow
  {0x00,0x00,0x80 }, // #000080 navy
  {0x00,0xff,0xff }, // #00ffff aqua
  {0x40,0x40,0x40 }, // #404040 ‚—hite
};

bool Num_Array[10][8]={
// a b c d e f g dp 
{ 1,1,1,1,1,1,0,0}, //0 :0
{ 0,1,1,0,0,0,0,0}, //1 :1
{ 1,1,0,1,1,0,1,0}, //2 :2
{ 1,1,1,1,0,0,1,0}, //3 :3
{ 0,1,1,0,0,1,1,0}, //4 :4
{ 1,0,1,1,0,1,1,0}, //5 :5
{ 1,0,1,1,1,1,1,0}, //6 :6
{ 1,1,1,0,0,0,0,0}, //7 :7
{ 1,1,1,1,1,1,1,0}, //8 :8
{ 1,1,1,1,0,1,1,0}, //9 :9   
};

int mMaxAnode=4;

int mSTAT =0;
int mSTAT_RSV_START=1;
int mSTAT_DISP=3;
string mResponse="";
string mReceive="";


//
int Is_bitPosition(int src , int Position){
    int iRet=0;
        int    iLen=mBit_MAX;
        char    buff[mBit_MAX+1];
        int     bin;
        int     i1;

        bin = src;
        buff[iLen] = '\0';
        for(int j = 0; j < iLen; j++){
            buff[j]='0';
        }
        
        for(i1 = 0; i1 < iLen; i1++){
                if(i1 != 0 && bin == 0)
                        break;
                if(bin % 2 == 0)
                        buff[(iLen-1)-i1] = '0';
                else
                        buff[(iLen-1)-i1] = '1';
                bin = bin / 2;
        }
//printf(">>>%s\n", buff );
    if(strlen(buff) >= 8 ){
      if(buff[Position]=='1'){ iRet=1; }
    }
   return iRet;
}


//
void LED_Set() {
  mDIN_H=0;
  mDIN_L=1;
  wait_ms(1);
}

//
void LED_Low_Bit() {
  mDIN_H=0;
  mDIN_L=0;
  wait_us(mWait_US);
  mDIN_H=0;
  mDIN_L=1;
  wait_us(mWait_US);
}

//
void LED_Hi_Bit() {
  mDIN_H=1;
  mDIN_L=1;
  wait_us(mWait_US);
  mDIN_H=0;
  mDIN_L=1;
  wait_us(mWait_US);
}

//
void LED_Init() {
  mDIN_H=0;
  mDIN_L=1;
  wait_ms(10); 
}

//
void LED_Color_RGB(unsigned int led_r, unsigned int led_g, unsigned int led_b) {
//printf("bit_r=%d, g=%d, b=%d \n" , led_r, led_g, led_b);
  //blue
  for (int k = 0; k < mBit_MAX; k++){
      if( Is_bitPosition(led_b , k)==1){
          LED_Hi_Bit();
      }else{
          LED_Low_Bit();
      }
  }
  //green
  for (int k = 0; k < mBit_MAX; k++){
      if( Is_bitPosition(led_g , k)==1){
          LED_Hi_Bit();
      }else{
          LED_Low_Bit();
      }
  }
  //red
  for (int k = 0; k < mBit_MAX; k++){
      if( Is_bitPosition(led_r , k)==1){
          LED_Hi_Bit();
      }else{
          LED_Low_Bit();
      }
  }
}

//
void init_proc(){
  // anode
  for(int i=0;i<4; i++){
      mAnode[ i ]=0;
  }
  // cathod
  for(int j=0;j<7; j++){
      mCathod[j]=0;
  }
  //RGB
    mDIN_H =0;
    mDIN_L =1;
    LED_Init();
    wait_ms(10);
  
}
// 
void proc_uart(){
   if( mPc.readable()) {
        char c= mPc.getc();
        mResponse+= c;
   }else{
       if(mSTAT ==mSTAT_RSV_START){
//printf("mResponse=%s\n" , mResponse.c_str() );                   
//           if(mResponse.length() >= 6){
             if(mResponse.length() >= 10){               
               string sHead=mResponse.substr(0,4);
               if( sHead== "tmp="){
                   mReceive =mResponse.substr(4 );
printf("sTmp=%s\n" , mReceive.c_str() );                   
                   mSTAT =mSTAT_DISP;
               }
               mResponse="";
           }
       }
   }
}
//
void set_anode(int pinAnode){
  for(int i=0;i <mMaxAnode; i++){
    if(mAnode_pin[i]==pinAnode){
       mAnode[i]=1;
    }else{
       mAnode[i]=0;
    }
  }
}
//
void NumPrint(int Number){
  for (int i=0; i< 7; i++){
    if(Num_Array[Number][i]==1){
       mCathod[mCathod_pin[i]]=0;
//printf("0");
    }
    wait_us(500);
    if(Num_Array[Number][i]==1){
       mCathod[mCathod_pin[i]]=1;
//printf("1");
    }
//printf(" \n");    
    mCathod[mCathod_pin[i]]=1;
  }
}


//
void display_rgb(int iCt ){
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(mColorMap[iCt][0], mColorMap[iCt][1], mColorMap[iCt][2] );
        }
        LED_Set();
        wait(2);
        //reset
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(0x00, 0x00, 0x00);
        }
        LED_Set();
}

//
void proc_display(int iTemp){
  int iDeg =iTemp;
  int iDeg_10= iDeg / 10;
  int iDeg_1 = iDeg % 10;
   // dig-1
   for(int i=0;  i< 200; i++ ){
       set_anode(mAnode_pin[0]);
       NumPrint(iDeg_1);
       // dig-10
       set_anode(mAnode_pin[1]);
       NumPrint(iDeg_10);
       // dig-100
       //set_anode(mAnode_pin[2]);
       //NumPrint(0);
       // dig-1000
       //set_anode(mAnode_pin[3]);
       //NumPrint(0);
   }
}
//
void proc_dispTime(string  sTime){
    //char sBuff[4+1];
  if(sTime.length() < 4){ return ; }
  //sBuff   =sTime.c_str();
  //sprintf(sBuff ,"%s" , sTime.c_str());
  string sOne=sTime.substr(0,1);
  int iDeg_1000= atoi( sOne.c_str() );
  sOne=sTime.substr(1,1);
  int iDeg_100 = atoi( sOne.c_str());
  sOne=sTime.substr(2,1);
  int iDeg_10  = atoi( sOne.c_str());
  sOne=sTime.substr(3,1);
  int iDeg_1   = atoi( sOne.c_str());
   // dig-1
   for(int i=0;  i< 100; i++ ){
       set_anode(mAnode_pin[0]);
       NumPrint(iDeg_1);
       // dig-10
       set_anode(mAnode_pin[1]);
       NumPrint(iDeg_10);
       // dig-100
       set_anode(mAnode_pin[2]);
       NumPrint(iDeg_100);
       // dig-1000
       set_anode(mAnode_pin[3]);
       NumPrint(iDeg_1000);
   }
}
//
int main() {
   mSTAT = mSTAT_RSV_START;
   mPc.baud(9600 );
   mPc.printf("#Start-main \n");    
   init_proc();
   int iWait=200;
   int iMode=0;  //0=temp, 1=time
   int iCt=0;
   while(1){
       if(mSTAT != mSTAT_DISP){
           proc_uart();
           wait_ms(5);
       }else{
printf("#disp \n" );
            string sBuff="";
            if(iMode==0){
               sBuff=mReceive.substr(0,2);
printf("sBuff=%s \n" ,sBuff.c_str() );
               int iNum= atoi( sBuff.c_str() );
               proc_display( iNum );
               iMode=1;                       
            }else{
               sBuff=mReceive.substr(2,4);
printf("sBuff=%s \n" ,sBuff.c_str() );
               proc_dispTime(sBuff);
               iMode=0;                       
            }
           wait_ms(100);
           display_rgb(iCt );
           iCt    =iCt +1;
           if(iCt >= 9 ){ iCt=0; }
            
           mSTAT =mSTAT_RSV_START;
       }
   }
}