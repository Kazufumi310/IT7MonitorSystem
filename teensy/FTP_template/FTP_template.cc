/**********************************************************************************************************************
  FTP_Server_SDFAT2.ino
   
  FTP_Server_Teensy41 is an FTP Server for Teensy 4.1 using SD, FS, etc. with QNEthernet or NativeEthernet
  
  Based on and modified from Arduino-Ftp-Server Library (https://github.com/gallegojm/Arduino-Ftp-Server)
  Built by Khoi Hoang https://github.com/khoih-prog/FTP_Server_Teensy41
 ***********************************************************************************************************************/

#include "defines.h"
#include <SdFat.h>
#include <SPI.h>

#define PASV_RESPONSE_STYLE_NEW       true
#define FTP_FILESYST                  FTP_SDFAT2

// Default 2048
#define FTP_BUF_SIZE                  8192

#define FTP_USER_NAME_LEN             64        // Max permissible and default are 64
#define FTP_USER_PWD_LEN             128        // Max permissible and default are 128

#include <FTP_Server_Teensy41.h>

// Object for FtpServer
FtpServer ftpSrv; // Default command port is 21 ( !! without parenthesis !! )

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Teensy 3.5 & 3.6 & 4.1 on-board: BUILTIN_SDCARD
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
// const int chipSelect = BUILTIN_SDCARD;    //10; // com out by Sato
#define chipSelect SdioConfig(FIFO_SDIO)

#include "sensors.hpp"


// SDClass myfs; // com out by Sato

// set up variables using the SD utility library functions:
//Sd2Card card; // com out by Sato
// SdVolume volume; // com out by Sato
//SdFile root;

/*
bool getModifyTime(FsFile &fil, DateTimeFields &tm) {
  uint16_t fat_date, fat_time;
  if (!fil.getModifyDateTime(&fat_date, &fat_time)) return false;
  if ((fat_date == 0) && (fat_time == 0)) return false;
  tm.sec = FS_SECOND(fat_time);
  tm.min = FS_MINUTE(fat_time);
  tm.hour = FS_HOUR(fat_time);
  tm.mday = FS_DAY(fat_date);
  tm.mon = FS_MONTH(fat_date) - 1;
  tm.year = FS_YEAR(fat_date) - 1900;
  return true;
}
*/

/*******************************************************************************
**                                                                            **
**                               INITIALISATION                               **
**                                                                            **
*******************************************************************************/

#define FTP_ACCOUNT       "teensy4x"
#define FTP_PASSWORD      "ftp_test"

void initEthernet()
{
#if USE_QN_ETHERNET
  Serial.println(F("=========== USE_QN_ETHERNET ==========="));
#elif USE_NATIVE_ETHERNET
  Serial.println(F("======== USE_NATIVE_ETHERNET ========"));
#elif USE_ETHERNET_GENERIC
  Serial.println(F("======== USE_ETHERNET_GENERIC ========"));
#else
  Serial.println(F("======================================="));
#endif

#if USE_NATIVE_ETHERNET

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  Ethernet.begin(thismac, myIP);
  //Ethernet.begin(mac[index]);

  Serial.print(F("Using mac index = "));
  Serial.println(index);

  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());

#elif USE_QN_ETHERNET

#if USING_DHCP
  // Start the Ethernet connection, using DHCP
  Serial.print(F("Initialize Ethernet using DHCP => "));
  Ethernet.begin();
  // give the Ethernet shield minimum 1 sec for DHCP and 2 secs for staticP to initialize:
  delay(1000);
#else
  // Start the Ethernet connection, using static IP
  Serial.print(F("Initialize Ethernet using static IP => "));
  Ethernet.begin(myIP, myNetmask, myGW);
  Ethernet.setDNSServerIP(mydnsServer);
#endif

  if (!Ethernet.waitForLocalIP(5000))
  {
    Serial.println(F("Failed to configure Ethernet"));

    if (!Ethernet.linkStatus())
    {
      Serial.println(F("Ethernet cable is not connected."));
    }

    // Stay here forever
    while (true)
    {
      delay(1);
    }
  }
  else
  {
    Serial.print(F("IP Address = "));
    Serial.println(Ethernet.localIP());
  }

  // give the Ethernet shield minimum 1 sec for DHCP and 2 secs for staticP to initialize:
  //delay(2000);

#else

  FTP_LOGWARN(F("Default SPI pinout:"));
  FTP_LOGWARN1(F("MOSI:"), MOSI);
  FTP_LOGWARN1(F("MISO:"), MISO);
  FTP_LOGWARN1(F("SCK:"),  SCK);
  FTP_LOGWARN1(F("SS:"),   SS);
  FTP_LOGWARN(F("========================="));
  
  // unknown board, do nothing, use default SS = 10
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   10    // For other boards
  #endif

  #if defined(BOARD_NAME)
    FTP_LOGWARN3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);
  #else
    FTP_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
  #endif

  // For other boards, to change if necessary 
  Ethernet.init (USE_THIS_SS_PIN);

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  Ethernet.begin(mac[index]);

  Serial.print(F("IP Address = "));
  Serial.println(Ethernet.localIP());
  
#endif
}

void cardInit()
{
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
//  if (!card.init(SPI_HALF_SPEED, chipSelect))
 if (!SD.begin(chipSelect))
  {
    Serial.println(F("Initialization failed. Things to check:"));
    Serial.println(F("* is a card inserted?"));
    Serial.println(F("* is your wiring correct?"));
    Serial.println(F("* did you change the chipSelect pin to match your shield or module?"));
    return;
  }
  else
  {
    Serial.println(F("Wiring is correct and a card is present."));
  }

  // print the type of card
  Serial.print(F("\nCard type: "));
  
//  switch (card.type())
switch (SD.card()->type())
  {
    case SD_CARD_TYPE_SD1:
      Serial.println(F("SD1"));
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println(F("SD2"));
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println(F("SDHC"));
      break;
    default:
      Serial.println(F("Unknown"));
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
//  if (!volume.init(card))
if (SD.vol()==nullptr) 
  {
    Serial.println(F("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card"));
    
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;

//  Serial.print("\nVolume type is FAT"); Serial.println(volume.fatType(), DEC); Serial.println();
 Serial.print(F("\nVolume type is FAT")); Serial.println(SD.vol()->fatType(), DEC); Serial.println();

//  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
//  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
//  volumesize = SD.vol()->blocksPerCluster();    // clusters are collections of blocks
  volumesize = SD.vol()->sectorsPerCluster();    // clusters are collections of blocks
  volumesize *= SD.vol()->clusterCount();       // we'll have a lot of clusters


  if (volumesize < 8388608ul)
  {
    Serial.print(F("Volume size (bytes): "));
    Serial.println(volumesize * 512);        // SD card blocks are always 512 bytes
  }

  Serial.print(F("Volume size (Kbytes): ")); volumesize /= 2;    Serial.println(volumesize);
  Serial.print(F("Volume size (Mbytes): ")); volumesize /= 1024; Serial.println(volumesize);
}

/*
void printDirectory(FsFile dir, int numSpaces)
{
  while (true)
  {
    FsFile entry = dir.openNextFile();

    if (! entry)
    {
      //Serial.println("** no more files **");
      break;
    }

    printSpaces(numSpaces);
//    Serial.print(entry.name());
    char name[21];
    entry.getName(name,40);
    Serial.print(name);

    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numSpaces + 2);
    }
    else
    {
      // files have sizes, directories do not
      unsigned int n = log10(entry.size());

      if (n > 10)
        n = 10;

//      printSpaces(50 - numSpaces - strlen(entry.name()) - n);
      char name[21];
      entry.getName(name,40);
      printSpaces(50 - numSpaces - strlen(name) - n);
      Serial.print("  "); Serial.print(entry.size(), DEC);
      
      DateTimeFields datetime;

//      if (entry.getModifyTime(datetime))
    if (getModifyTime(entry, datetime))
      {
        printSpaces(4);
        printTime(datetime);
      }

      Serial.println();
    }
    
    entry.close();
  }
}

void printSpaces(int num)
{
  for (int i = 0; i < num; i++)
  {
    Serial.print(" ");
  }
}

void printTime(const DateTimeFields tm)
{
  const char *months[12] =
  {
      "January",  "February", "March",      "April",    "May",        "June",
      "July",     "August",   "September",  "October",  "November",   "December"
  };

  if (tm.hour < 10)
    Serial.print('0');

  Serial.print(tm.hour);
  Serial.print(':');

  if (tm.min < 10)
    Serial.print('0');

  Serial.print(tm.min);
  Serial.print("  ");
  Serial.print(months[tm.mon]);
  Serial.print(" ");
  Serial.print(tm.mday);
  Serial.print(", ");
  Serial.print(tm.year + 1900);
}
*/

void SDCardTest()
{
  Serial.println(F("\n==============================="));
  Serial.print(F("SDCard Initialization : "));

  
  if (!SD.begin(chipSelect))
  {
    Serial.println(F("failed!"));
    return;
  }

  Serial.println(F("done."));

  //  FsFile root = SD.open("/");
  //printDirectory(root, 0); // omit to save RAM space
  //  Serial.println(F("done!"));
}


void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(500);

  Serial.print(F("\nStarting FTP_Server_SDFAT2 on ")); Serial.print(BOARD_NAME);
  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE);
  Serial.println(FTP_SERVER_TEENSY41_VERSION);

// Uncomment this if Teensy 4.0 has SD card
#if (defined(ARDUINO_TEENSY41))

  Serial.println(F("Initializing SD card..."));

  //////////////////////////////////////////////////////////////////

  cardInit();

  ////////////////////////////////////////////////////////////////

  SDCardTest();

  //////////////////////////////////////////////////////////////////
#endif

  SD.mkdir("data");

  initEthernet();


  //////////////////////
  initADC();
  //////////////////////

  // Initialize the FTP server
  ftpSrv.init();
  ftpSrv.credentials( FTP_ACCOUNT, FTP_PASSWORD );

  Serial.print(F("FTP Server Credentials => account = ")); Serial.print(FTP_ACCOUNT);
  Serial.print(F(", password = ")); Serial.println(FTP_PASSWORD);
  
  pinMode(LED_BUILTIN, OUTPUT);

//  sd.begin(SD_CONFIG); <- written in CardInit

   
  Serial.println(F("finish initialization"));

}


/*******************************************************************************
**                                                                            **
**                                 MAIN LOOP                                  **
**                                                                            **
*******************************************************************************/

void loop()
{
  ftpSrv.service();

  char testReqFile[] = "data/test.request";
  char runReqFile[] = "data/run.request";
  char testFinFile[] = "data/test.finish";
  char runFinFile[] = "data/run.finish";
  for(int ipat=0;ipat<2;ipat++){
    char* reqFile;
    char* finFile;
    int RecordTime;
    if(ipat==0){
      reqFile = testReqFile;
      finFile = testFinFile;
      RecordTime = 1000000*5;// in micro. i.e., = 5 sec
    }
    if(ipat==1){
      reqFile = runReqFile;
      finFile = runFinFile;
      RecordTime = 1000000*120;// in micro. i.e., = 120 sec
    }

    if(SD.exists(reqFile) ){
      Serial.println(F("A request file is found."));
      SD.remove(reqFile);
      takeADCData(RecordTime);
      FsFile dataFile = SD.open(finFile, FILE_WRITE);
      dataFile.close();
    }
  }   

  
}


