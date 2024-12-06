#include "ADC.h"
#include "DMAChannel.h"
#include "FreeStack.h"
#include "RingBuf.h"

#define THIS_IS_KELLERPRESSURE
#define WATERLEAK_SENSOR



// 400 sector RingBuf - could be larger on Teensy 4.1.
const size_t RING_BUF_SIZE = 400 * 512;
// Preallocate 1GiB file.
const uint64_t LOG_FILE_SIZE = 1ULL << 28;
//const uint64_t PRE_ALLOCATE_SIZE = 300000;

#define ADCSPEED ADC_SAMPLING_SPEED::VERY_HIGH_SPEED



const int nGauges = 1;
const int readPin[] = {A0,A1,A2,A3,A4,A5,A6,A7}; //only use readPin[0]

#define FILE_BASE_NAME "data/Shk"
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[32] = FILE_BASE_NAME "00.bin";

//data file
FsFile file;
// RingBuf for File type FsFile.
RingBuf<FsFile, RING_BUF_SIZE> rb;



ADC *adc = new ADC();

DMAChannel dma(true);
// Ping-pong DMA buffer.
DMAMEM static uint16_t __attribute__((aligned(32))) dmaBuf[2][256];
// Count of DMA interrupts.
volatile size_t dmaCount;
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
  adc->adc0->enableDMA();
//  adc->adc0->startContinuous(ADC_PIN);
  adc->adc0->startContinuous(readPin[0]);
}

///////////////////////////////////


//------------------------------------------------------------------------------
void stopDma() {
  adc->adc0->disableDMA();
  dma.disable();
}

/////////////////////////////////////

bool logData(){
  size_t n = rb.bytesUsed();
  if ((n + file.curPosition()) >= (LOG_FILE_SIZE - 512)) {
    Serial.println("File full - stopping");
    return false;
   //   file.close();
  }
  if (n >= 512) {
    if (rb.writeOut(512) != 512) {
      Serial.println("writeOut() failed");
      return false;
     // file.close();
    }
  }
    return true;
}


#include "pressurestrainAction.hpp"


