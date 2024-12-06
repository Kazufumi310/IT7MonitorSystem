#include <t41-ptp.h>
//#include <QNEthernet.h>

/*

*/

#define USINGPTP

//int const TTLPIN = A10;

bool p2p=false;
bool master=false;
bool slave=true;

l3PTP ptp(master,slave,p2p);
//l2PTP ptp(master,slave,p2p);

FsFile timingFile;
char timingFileName[32]={"data/timing.csv"};

void recordDAQTime(const char* info, bool fileCheck=false){
  if(!timingFile.isOpen()){
      timingFile.open(timingFileName, O_WRONLY | O_CREAT | O_APPEND);
      timingFile.println("micros,ptp_nano,ptp_date");
  }else if(fileCheck){
    if(!SD.exists(timingFileName)){
      timingFile.open(timingFileName,O_WRONLY | O_CREAT | O_EXCL);
      Serial.printf("new timing file was created.");
    }
  }
  double curTime=micros();
  timespec ts;
  qindesign::network::EthernetIEEE1588.readTimer(ts);
  NanoTime ptp_nano=timespecToNanoTime(ts);
  timingFile.print(info);
  timingFile.write(" ");
//  timingFile.print(fileName);
//  timingFile.write(" ");
  timingFile.print(curTime);
  timingFile.write(",");
  timingFile.print(ptp_nano);
  timingFile.write(",");
  char tbuffer[128];
  printTime(ptp_nano,tbuffer);
  timingFile.print(tbuffer);
  timingFile.sync();
}

void logPTP(){
  uint32_t curTime = micros();
  static uint32_t logTime = curTime;

//  if(curTime>logTime){
  if(curTime>logTime && !isDAQRunning ){
    while(curTime>logTime){
      logTime=logTime+5*1000000;
    }
    
    recordDAQTime("PTP check: ",true);

    timespec ts;
    qindesign::network::EthernetIEEE1588.readTimer(ts);
    NanoTime ptp_nano=timespecToNanoTime(ts);
    Serial.print("currentTime: ");
    Serial.println(curTime);
    printTime(ptp_nano);
  }
}

void doPTP(bool init = false){
  ptp.update();
  logPTP();
}
/* // checking file existence is too slow. 
void doPTPAction(){
   char ptpReqFile[] = "data/ptp.request";
   bool init = false;
   if(SD.exists(ptpReqFile) ){
  	Serial.println(F("PTP update request."));
  	SD.remove(ptpReqFile);
    init = true;
   }
  doPTP(init);
}
*/

void initPTP()
{
  Serial.printf("ptp start");
  //  Serial.begin(2000000);
  //pinMode(TTLPIN, OUTPUT);
  //pinMode(13, OUTPUT);

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
  
}





