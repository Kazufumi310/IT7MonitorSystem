// Test Teensy SDIO with write busy in a data logger demo.
//
// The driver writes to the uSDHC controller's FIFO then returns
// while the controller writes the data to the SD.  The first sector
// puts the controller in write mode and takes about 11 usec on a
// Teensy 4.1. About 5 usec is required to write a sector when the
// controller is in write mode.

//#include "SdFat.h"
#include <ADC.h>
#include <ADC_util.h>

// Use Teensy SDIO
//#define SD_CONFIG SdioConfig(FIFO_SDIO)

// Size to log 10 byte lines at 25 kHz for more than ten minutes.
#define LOG_FILE_SIZE 200 * 1024 //200 * 25000 * 600  // 150,000,000 bytes.

const uint8_t DaqTime=10;

//#define LOG_FILENAME "SdioLogger.csv"
// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "data/Data"

//const uint32_t RecordTime= 1000000 * 120; //in micros
const uint32_t RecordTime= 1000000 * 10; //for test. in micros


const int nGauges = 4;
const int readPin[] = {A0,A1,A2,A3};

//SdFs sd;
FsFile file;
ADC *adc = new ADC(); // adc object;

//Buffer reserve
//const int bufferSize = 1024; // Size of the buffer in bytes
String buffer;


uint32_t logTime;
uint32_t samplingTime;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[32] = FILE_BASE_NAME "00.csv";

void logData() {

    logTime = micros() + DaqTime;
    
    file.print(micros()-samplingTime);
    for(int i=0;i<nGauges;i++){
      file.write(",");
      file.print(adc->adc0->analogRead(readPin[i]));
    }
    file.println("");
 
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    while (micros() < logTime) {
      ;
    }
    
  }



void initADC() {
    for(int i=0;i<nGauges;i++){
       pinMode(readPin[i],INPUT);
    }

    adc->adc0->setAveraging(0); // set number of averages
    adc->adc0->setResolution(10); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed   
}

void takeADCData(int RecordTime){

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

   while (SD.exists(fileName)) {
      if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } 
  }
  file.open(fileName, O_WRONLY | O_CREAT | O_EXCL);
  file.preAllocate(LOG_FILE_SIZE);

  samplingTime = micros();
  Serial.print("sampling start time: ");
  Serial.println(samplingTime);

  while ((micros()-samplingTime)<=RecordTime) {
    logData();
  }
  Serial.print("sampling end time: ");
  Serial.println(micros());
  
  Serial.println("finishing... ");
  file.truncate();
  file.close();
  Serial.println("finish all. ");
  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}


#include "pressurestrainAction.hpp"

void doAction(){
  doPressureStrainAction();
}

