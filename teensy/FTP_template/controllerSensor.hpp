#include <ADC.h>
#include <ADC_util.h>

int TTLInputPin = A0;
int TTLOutputPin = A17;

#define FILE_BASE_NAME "data/Data"

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[32] = FILE_BASE_NAME "00.csv";

FsFile file;
ADC *adc = new ADC(); // adc object;

void initADC(){
  // ADC to check TTL
  pinMode(TTLInputPin, INPUT);

  /// TTL pulser
  pinMode(TTLOutputPin, OUTPUT);
  digitalWriteFast(TTLOutputPin,LOW);

  //// ADC setup
  adc->adc0->setAveraging(0); // set number of averages
  adc->adc0->setResolution(10); // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed
}



void checkTTL(){
  static double prevV = 0;

  int sensorValue =  adc->adc0->analogRead(TTLInputPin); // analogRead(A0);
  float voltage = sensorValue * (3.3 / 1023.0);

  if (prevV < 0.8 && voltage > 2.0){
    Serial.print("TTL detected. t = ");
    Serial.print(micros());
    Serial.print(" usec, V = ");
    Serial.print(voltage);
    Serial.print(" ( prev value = ");
    Serial.print(prevV);
    Serial.println(" )");
    if(!file.isOpen()){
      file.open(fileName, O_WRONLY | O_CREAT | O_APPEND);
    }
    file.print("TTL detected. t = ");
    file.print(micros());
    file.print(" usec, volt[V] = ");
    file.print(voltage);
    file.println(" V.");
    file.close();
  }
  prevV = voltage;
  
}

void generatePulse(){
  // generate pulse from teensy if integer is through Serial. 
  if ( Serial.available() ) { 
    String key = Serial.readString();
    key.trim();
    int length = key.toInt();
    if(length!=0){
      Serial.print(key);
      //      Serial.println("-usec-width TTL pulse");
      digitalWriteFast(TTLOutputPin,HIGH);
      
      uint32_t from = micros();
      uint32_t to = from;
      while(to-from < length){
        to = micros();
      }
      digitalWriteFast(TTLOutputPin,LOW);
      Serial.print("TTL output : t = ");
      Serial.print(from);
      Serial.print("--");
      Serial.print(to);
      Serial.println(" usec.");
    }else{
      Serial.print("ignore ");
      Serial.println(key);
      digitalWriteFast(TTLOutputPin,LOW);
    }
  }
}

void doAction(){
  checkTTL();
  generatePulse();
}
