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

void initPTP()
{
  Serial.printf("ptp start");
  //  Serial.begin(2000000);
  pinMode(TTLPIN, OUTPUT);

  byte mac[6];
  IPAddress subnetMask{255, 255, 255, 0};
  IPAddress gateway{192, 168, 11, 1};

  // Setup networking
  qindesign::network::Ethernet.setHostname("t41ptpslave");
  qindesign::network::Ethernet.macAddress(mac);
  qindesign::network::Ethernet.begin(myIP, subnetMask, gateway);
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
    if(curTime>logTime){
      logTime=micros()+1.*1000000;
      Serial.print("currentTime: ");
      Serial.println(curTime);
      Serial.println(ptp.getLockCount());
      timespec ts;
      qindesign::network::EthernetIEEE1588.readTimer(ts);
      printTime(timespecToNanoTime(ts));
    }
    if(curTime>startTime+10.*1000000){
      break;
    }
  }
}
