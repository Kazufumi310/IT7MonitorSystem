#include "ADC.h"
#include "DMAChannel.h"
#include "FreeStack.h"
#include "RingBuf.h"

// Pin must be on first ADC.
#define ADC_PIN A0
// 400 sector RingBuf - could be larger on Teensy 4.1.
const size_t RING_BUF_SIZE = 400 * 512;
//const int RecordTime= 1000000 * 120; //in micros
//const int RecordTime= 1000000 * 10; //in micros. short time for test
// Preallocate 1GiB file.
const uint64_t PRE_ALLOCATE_SIZE = 1ULL << 30;
//const uint64_t PRE_ALLOCATE_SIZE = 300000;


ADC adc;
DMAChannel dma(true);

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "data/Shk"
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[32] = FILE_BASE_NAME "00.bin";
// Ping-pong DMA buffer.
DMAMEM static uint16_t __attribute__((aligned(32))) dmaBuf[2][256];
// Count of DMA interrupts.
volatile size_t dmaCount;
// RingBuf for 512 byte sectors.
RingBuf<FsFile, RING_BUF_SIZE> rb;

// Shared between ISR and background.
volatile size_t maxBytesUsed;

// Overrun error for write to RingBuf.
volatile bool overrun;

//------------------------------------------------------------------------------
// ISR for DMA.
static void isr() {
  if (!overrun) {
    // Clear cache for buffer filled by DMA to insure read from DMA memory.
    arm_dcache_delete((void*)dmaBuf[dmaCount & 1], 512);
    // Enable RingBuf functions to be called in ISR.
    rb.beginISR();
    if (rb.write(dmaBuf[dmaCount & 1], 512) == 512) {
      dmaCount++;
      if (rb.bytesUsed() > maxBytesUsed) {
                maxBytesUsed = rb.bytesUsed();
      }
    } else {
      overrun = true;
    }
    // End use of RingBuf functions in ISR.
    rb.endISR();
  }
  dma.clearComplete();
  dma.clearInterrupt();
#if defined(__IMXRT1062__)
  // Handle clear interrupt glitch in Teensy 4.x!
  asm("DSB");
#endif  // defined(__IMXRT1062__)
}
//------------------------------------------------------------------------------
#if defined(__IMXRT1062__)  // Teensy 4.x
#define SOURCE_SADDR ADC1_R0
#define SOURCE_EVENT DMAMUX_SOURCE_ADC1
#else
#define SOURCE_SADDR ADC0_RA
#define SOURCE_EVENT DMAMUX_SOURCE_ADC0
#endif




// SdFs sd;  <- already written in SdFat 
// FsFile file; <- move inside takeADCData


//////////////////////////

static void startDma() {
  dma.begin();
  dma.attachInterrupt(isr);
  dma.source((volatile const signed short&)SOURCE_SADDR);
  dma.destinationBuffer((volatile uint16_t*)dmaBuf, sizeof(dmaBuf));
  //dma.interruptAtHalf();
  dma.interruptAtCompletion();
  dma.triggerAtHardwareEvent(SOURCE_EVENT);
  
  dma.enable();
  adc.adc0->enableDMA();
  adc.adc0->startContinuous(ADC_PIN);
}

///////////////////////////////////


//------------------------------------------------------------------------------
void stopDma() {
  adc.adc0->disableDMA();
  dma.disable();
}

/////////////////////////////////////


void takeADCData(int RecordTime){
  Serial.print(F("data taking for "));
  Serial.print(RecordTime/1e6);
  Serial.println(F(" sec"));
  dmaCount = 0;
  maxBytesUsed = 0;
  overrun = false;

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  while (SD.exists(fileName)) {
      if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } 
  }
  FsFile file;
  file.open(fileName, O_WRONLY | O_CREAT | O_EXCL);
  if (!file.preAllocate(PRE_ALLOCATE_SIZE)) {
    SD.errorHalt("file.preAllocate failed");
  }
  rb.begin(&file);

  startDma();
  uint32_t samplingTime = micros();
  while ((micros()-samplingTime)<=RecordTime) {
    size_t n = rb.bytesUsed();
    if ((n + file.curPosition()) >= (PRE_ALLOCATE_SIZE - 512)) {
      file.close();
    }
    if (n >= 512) {
      if (rb.writeOut(512) != 512) {
        file.close();
      }
    }
  }

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  stopDma();
  if (!rb.sync()) {
    file.close();
  }

  file.truncate();

  file.close();

//  static int ncalled = 0;
//  ncalled++;
//  if(ncalled > 2) exit(1);
 Serial.println(F("finish data taking"));

}


void initADC(){
  Serial.println(F("init ADC"));
  pinMode(ADC_PIN, INPUT);
  // Try for max speed.
  adc.adc0->setAveraging(0);
  adc.adc0->setResolution(10);
  adc.adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);
  adc.adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
  
}
