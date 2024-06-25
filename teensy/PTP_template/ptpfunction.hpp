#include <t41-ptp.h>
//#include <QNEthernet.h>

/*

*/

int const TTLPIN = A10;

bool p2p=false;
bool master=false;
bool slave=true;

l3PTP ptp(master,slave,p2p);
//l2PTP ptp(master,slave,p2p);

FsFile timingFile;
char timingFileName[32]={"data/timing.csv"};

bool ptp_notYet = true;
void doPTP(bool init = false){
  static double endTime=-1;
  static double logTime=-1;
  static double startTime=-1;

  if(init){
    endTime = logTime = startTime = -1;
  }

  double curTime=micros();
  if(endTime<0){
    startTime = curTime +5.*1000000;
    endTime = startTime+10.*1000000;
    logTime = startTime+1.*1000000;
  }

  if(curTime>endTime){
    if(timingFile.isOpen()){ timingFile.close(); }
    return;
  }
  

  ptp.update();
  if(startTime>curTime) return;

/*
  {
    Serial.print("currentTime before starting ptp: ");
    Serial.println(curTime);
    timespec ts;
    qindesign::network::EthernetIEEE1588.readTimer(ts);
    printTime(timespecToNanoTime(ts));
  }
*/

 
  
    //digitalWrite(13, ptp.getLockCount() > 5 ? HIGH : LOW);

  if(curTime>logTime){
    logTime=micros()+1.*1000000;
    Serial.print("currentTime: ");
    Serial.println(curTime);
    Serial.println(ptp.getLockCount());
    timespec ts;
    qindesign::network::EthernetIEEE1588.readTimer(ts);
    NanoTime ptp_nano=timespecToNanoTime(ts);
    printTime(ptp_nano);
    if(!timingFile.isOpen()){
      timingFile.open(timingFileName, O_WRONLY | O_CREAT | O_APPEND);
      timingFile.println("micros,ptp_nano,ptp_date");
    }
    timingFile.print(curTime);
    timingFile.write(",");
    timingFile.print(ptp_nano);
    timingFile.write(",");
    char tbuffer[128];
    printTime(ptp_nano,tbuffer);
    timingFile.print(tbuffer);

  }
}



void initPTP()
{
  Serial.printf("ptp start");
  //  Serial.begin(2000000);
  //pinMode(TTLPIN, OUTPUT);
  pinMode(13, OUTPUT);

  byte mac[6];
  //IPAddress subnetMask{255, 255, 255, 0};
  //IPAddress gateway{192, 168, 11, 1};

  // Setup networking
  qindesign::network::Ethernet.setHostname("t41ptpslave");
  qindesign::network::Ethernet.macAddress(mac);
  qindesign::network::Ethernet.begin(myIP, myNetmask, myGW);
  qindesign::network::EthernetIEEE1588.begin();
  
  qindesign::network::Ethernet.onLinkState([](bool state) {
    Serial.printf("[Ethernet] Link %dMbps %s\n", qindesign::network::Ethernet.linkSpeed(), state ? "ON" : "OFF");
    if (state) {
      ptp.begin();
    }
  });

  Serial.print("Slave:");
  Serial.printf("Mac address:   %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print( "IP:            "); Serial.println(qindesign::network::Ethernet.localIP());
  Serial.println();

  // PPS Out
  // peripherial: ENET_1588_EVENT1_OUT
  // IOMUX: ALT6
  // teensy pin: 24
  IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12 = 6;
  qindesign::network::EthernetIEEE1588.setChannelCompareValue(1, NS_PER_S-60);
  qindesign::network::EthernetIEEE1588.setChannelMode(1, qindesign::network::EthernetIEEE1588.TimerChannelModes::kPulseHighOnCompare);
  qindesign::network::EthernetIEEE1588.setChannelOutputPulseWidth(1, 25);

/*
  double curTime=micros();
  timespec ts;
  qindesign::network::EthernetIEEE1588.readTimer(ts);

  Serial.print("currentTime before starting ptp: ");
  Serial.println(curTime);
  printTime(timespecToNanoTime(ts));
  double startTime = curTime;
  double logTime=curTime+5*1000000;
  while(1){
    ptp.update();
    digitalWrite(13, ptp.getLockCount() > 5 ? HIGH : LOW);

    curTime=micros();
    if(curTime>logTime){
      logTime=micros()+5.*1000000;
      Serial.print("currentTime: ");
      Serial.println(curTime);
      Serial.println(ptp.getLockCount());
      timespec ts;
      qindesign::network::EthernetIEEE1588.readTimer(ts);
      printTime(timespecToNanoTime(ts));
    }
    if(curTime>startTime+60.*1000000){
      break;
    }
  }
  */
}




