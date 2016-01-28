#include <EtherCard.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x32 };
byte Ethernet::buffer[700];

unsigned long timer;
static uint32_t mTimerTmp;
static uint32_t mTimerDBG;
static const char website[] PROGMEM = "dns1234.com";
String mTimeStr="";
int mTempNum =0;

//
void send_7seg(String src){
  Serial.print("tmp=");
  Serial.print(mTempNum);
  delay(100);
  Serial.println(src);
}

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  delay(500);
  
  Ethernet::buffer[off+300] = 0;
  //Serial.print((const char*) Ethernet::buffer + off);
  String response =(const char*) Ethernet::buffer + off;
  int iSt=0;
  iSt = response.indexOf("res=");
  if(iSt >= 0){
    iSt = iSt+ 4;
    String sDat = response.substring(iSt );
    if(sDat.length() >= 4){
      mTimeStr = sDat.substring(0, 4);
      //send_7seg(sDat);
    }
  }
//Serial.println("...");
//Serial.println("response="+response);
//mTimeStr
}

//
void setup () {
  mTimeStr="0000";
  Serial.begin( 9600);
  dht.begin();
  Serial.println(("#Start-[webClient]"));

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

//  ether.printIp("IP:  ", ether.myip);
//  ether.printIp("GW:  ", ether.gwip);  
//  ether.printIp("DNS: ", ether.dnsip);  
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
//  ether.printIp("SRV: ", ether.hisip);
  delay( 5000 );
}

void loop () {
    float iTemp = dht.readTemperature();
//    iTemp=12.0;
    if (  isnan(iTemp) ) {
      Serial.println("Failed to read from DHT sensor!");
      delay(500);
      return;
    }
    if (millis() > mTimerTmp) {
      mTimerTmp = millis()+5000;
      mTempNum =(int) iTemp;
      char sTmp[2+1];
      sprintf(sTmp, "%02d", (int) iTemp);
      Serial.print("tmp=");
      Serial.print(sTmp);   
//      delay(100);
      Serial.println(mTimeStr);   
    }    
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer) {
      timer = millis()+ 60000;
      Serial.println();
      Serial.print("<<< REQ ");
      char buff[32+1]="";
      int iNum=(int) iTemp;
      sprintf(buff, "?mc_id=1&tmp=%d",  iNum);
//Serial.print("buff=");
//Serial.println(buff);
      ether.browseUrl(PSTR("/api-1234.php"), buff, website, my_callback);  
    }
}






