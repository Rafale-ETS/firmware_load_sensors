#include "npt_time.h"

// NTP server to request epoch time
unsigned int localPort = 2390;      // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
//IPAddress timeServer(162, 159, 200, 123); // pool.ntp.org NTP server
IPAddress timeServer(192, 168, 105, 1); // local SE NTP server (PI: 192.168.105.1)
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// Variable to save current epoch time
unsigned long epochTime;
unsigned long last_setRTC_epoch = 0;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
// rtc object
RTCZero rtc;

// returns if no epoch aquired. 
// Note: This function blocks the system for 1 sec to wait for NTP paquet.
unsigned long get_ntp_epoch(){
    sendNTPpacket(timeServer); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
    if (Udp.parsePacket()) {
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;

        // now convert NTP time into everyday time:
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        unsigned long epoch = secsSince1900 - seventyYears;
        // print Unix time:
        Serial.print("Unix time = ");
        Serial.println(epoch);
        return epoch;
    }
    return 0;
}

void setRTC() { // get the time from Internet Time Service
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 10;
  do {
    epoch = get_ntp_epoch();
    numberOfTries++;
  }
  while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries < maxTries) {
    rtc.setEpoch(epoch);
    last_setRTC_epoch = epoch;
  } else {
    Serial.println("RTC not setup/updated, failed to reach NTP server... ");
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void setup_time(){
    if(WiFi.status() != WL_CONNECTED){
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(HUB_SSID);
        while(WiFi.begin(HUB_SSID, HUB_PASS) != WL_CONNECTED) {
            // failed, retry.
            Serial.print(".");
            delay(5000);
        }
        WiFi.lowPowerMode();
        Serial.println("Connection success!\n");
    } else {
        Serial.print("Already connected to WiFi: ");
        Serial.println(WiFi.SSID());
    }

    Serial.println("Starting connection to NTP server.");
    Udp.begin(localPort);

    rtc.begin();
    setRTC();
}
    
unsigned long get_timestamp_now(){
    if(epochTime > last_setRTC_epoch + MAX_TIME_BETWEEN_RTC_SYNC){
        setRTC(); // sync the RTC with NTP time from the PI
    }

    epochTime = rtc.getEpoch();
    return epochTime;
}