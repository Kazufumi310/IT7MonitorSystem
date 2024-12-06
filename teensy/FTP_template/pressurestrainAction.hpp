
int isDAQRequest = 0;

#ifdef WATERLEAK_SENSOR
int const waterLeakPin = A16;
#endif

void initADC() {
 
    for(int i=0;i<nGauges;i++){
       pinMode(readPin[i],INPUT);
    }

    #ifdef WATERLEAK_SENSOR
    pinMode(waterLeakPin,INPUT);
    #endif

    adc->adc0->setAveraging(0); // set number of averages
    adc->adc0->setResolution(10); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADCSPEED); // change the sampling speed   


  if(SD.exists(runFlag)){
    isDAQRequest = 2;
  }
}

bool takeADCData(int RecordTime){
  static double endTime = -1;

  if(endTime<0){ // initialize
#ifdef THIS_IS_KELLERPRESSURE
    dmaCount = 0;
    maxBytesUsed = 0;
    overrun = false;
#else
    maxUsed = 0;
    DAQError = 0;
#endif

    Serial.print(F("data taking for "));
    Serial.print(RecordTime/1e6);
    Serial.println(F(" sec"));
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    // /*   // flag file
  if(isDAQRequest==2){
     if(!SD.exists(runFlag)){
      flagFile.open(runFlag, O_WRONLY | O_CREAT | O_EXCL);
//      flagFile.println("test");
      flagFile.close();
      delay(100);
     }
  }
  
   /////////////*/



    while (SD.exists(fileName)) {
      if(fileName[BASE_NAME_SIZE + 1] != '9') {
       fileName[BASE_NAME_SIZE + 1]++;
      }else if(fileName[BASE_NAME_SIZE] != '9') {
       fileName[BASE_NAME_SIZE + 1] = '0';
       fileName[BASE_NAME_SIZE]++;
      } 
    }
    
    file.open(fileName, O_WRONLY | O_CREAT | O_EXCL);
    if (!file.preAllocate(LOG_FILE_SIZE)) {
      SD.errorHalt("file.preAllocate failed");
    }

    rb.begin(&file);
  Serial.println(micros());
#ifdef USINGPTP
    String title = "0.5 sec before DAQ_start: ";
    title += fileName;
    title += " ";
    uint32_t startTime = micros()+500000;
    recordDAQTime(title.c_str());
    while(startTime>micros()){
      ;
    }
#endif
  Serial.println(micros());
#ifdef THIS_IS_KELLERPRESSURE
    startDma();
#endif
  Serial.println(micros());
  }

  uint32_t samplingTime = micros();
  if( endTime < 0 ){
    endTime = samplingTime + RecordTime;
  }

  if(endTime > samplingTime ){
    if(logData()) return true;
    else{
      endTime = samplingTime; // File might be full, or writing was failed with some reason. The data taking will stop in next cycle. 
      return true;
    }
  }else{ // finish data taking
#ifdef THIS_IS_KELLERPRESSURE
    stopDma();
#endif

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    endTime = -1;
    Serial.print("sampling end time: ");
    Serial.println(micros());
    Serial.println(samplingTime);
  
    Serial.println("DAQ end ");
#ifdef USINGPTP
    String title = "DAQ_end: ";
    title += fileName;
    title += " ";
    recordDAQTime(title.c_str());
#endif
    if (!rb.sync()) {
     file.close();
    }
    file.truncate();
    file.close();
    Serial.println(F("close file."));
#ifdef THIS_IS_KELLERPRESSURE
  if (overrun) {
    Serial.println("Overrun ERROR!!");
  }
  Serial.print("dmsCount ");
  Serial.println(dmaCount);
  Serial.print("RingBufSize ");
  Serial.println(RING_BUF_SIZE);
  Serial.print("maxBytesUsed ");
  Serial.println(maxBytesUsed);
  char comment[128];
  sprintf(comment, "isOverRun:%d, dmsCount:%d, maxBytesUsed:%d ", overrun,dmaCount,maxBytesUsed);
  recordDAQTime(comment);
#else
  char comment[128];
  sprintf(comment, "DAQError:%d, maxBytesUsed:%d ", DAQError,maxUsed);
  recordDAQTime(comment);
#endif
////////// remove flag
    if(SD.exists(runFlag)){
      SD.remove(runFlag);
    }
////////////////////

    return false;
  }
  return false;
}
/*
bool takeADCData(int RecordTime){
  static double endTime = -1;

 if(endTime<0){ // initialize
    Serial.print(F("data taking for "));
    Serial.print(RecordTime/1e6);
    Serial.println(F(" sec"));
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  // flag file
  if(isDAQRequest==2){
     if(!SD.exists(runFlag)){
      flagFile.open(runFlag, O_WRONLY | O_CREAT | O_EXCL);
//      flagFile.println("test");
      flagFile.close();
      delay(100);
     }
  }
   /////////////
#ifdef USINGPTP
    String title = "DAQ_start: ";
    title += fileName;
    title += " ";
    recordDAQTime(title.c_str());
#endif


    while (SD.exists(fileName)) {
      if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
     fileName[BASE_NAME_SIZE]++;
    } 
   }
   file.open(fileName, O_WRONLY | O_CREAT | O_EXCL);
   if (!file.preAllocate(LOG_FILE_SIZE)) {
      SD.errorHalt("file.preAllocate failed");
    }

   rb.begin(&file);


  }

  uint32_t samplingTime = micros();
  if(endTime<0){
    endTime = samplingTime + RecordTime;
  }

  if( endTime > samplingTime ) {
    if(logData()) return true;
    else{
      endTime = samplingTime; // File might be full, or writing was failed with some reason. The data taking will stop in next cycle. 
      return true;
    }
  }else{

///////////////
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    endTime = -1;
    Serial.print("sampling end time: ");
    Serial.println(micros());
  
    Serial.println("DAQ end ");
    
    if (!rb.sync()) {
     file.close();
    }
    file.truncate();
    file.close();
    Serial.println("close file. ");

#ifdef USINGPTP
    String title = "DAQ_end: ";
    title += fileName;
    title += " ";
    recordDAQTime(title.c_str());
#endif
   
////////// remove flag
    if(SD.exists(runFlag)){
      SD.remove(runFlag);
    }
////////////////////

    return false;
  }
  return false;
}
*/

double adcMon[nGauges]={0};
char dateMon[128];
uint32_t timeMon=0;
#ifdef WATERLEAK_SENSOR
double wleakMon=0;
#endif


void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String req_str = "";
    String post_str = "";
    boolean skip = true;
    bool finish = false;

    double recentAvailableTime = micros();
    while (client.connected()) {
        if (client.available()) {
          recentAvailableTime = micros();
          char c = client.read();
         // if you've gotten to the end of the line (received a newline
         // character) and the line is blank, the http request has ended,
         // so you can send a reply
          req_str += c;
          if(!skip && c != '\n' && c != '\r'){ 
            post_str += c;
          }
          if ( c == '\n' && currentLineIsBlank  ){
            if ( req_str.startsWith("GET") || !skip){
              finish = true;
            }else{
              Serial.println("This is POST request");
              skip = false;
            }
          }
          if (c == '\n') {
          // you're starting a new line
             currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }

        if(micros() - recentAvailableTime > 1*1000000) finish = true;
        if ( finish ){
          Serial.print("post_data:");
          Serial.println(post_str);
          Serial.println(req_str);
          // send a standard http response header
          if(post_str==""){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println ("Refresh: 5");
            client.println();
            // print the current readings, in HTML format:
            client.println ("<!DOCTYPE HTML>");
            client.println ("<html>");
            client.print ("<Title>Teensy Reading </Title>");
            client.println ("<h1>Strain sensors </h1>");
            client.print(dateMon);
            client.print("<br>teensy clock time: ");
            client.print(timeMon);
            client.print("<br>ADC: ");
            for(int i=0;i<nGauges;i++){
              client.print(adcMon[i]);
              client.print(" ");
            }
            #ifdef WATERLEAK_SENSOR
            client.print(" wLeak ");
            client.print(wleakMon);
            #endif
            client.println("");
          }else{
            //Serial.print("test");
            if(post_str=="test"){
              client.println("test run is requested");
              isDAQRequest = 1;
            }else if(post_str=="run"){
              client.println("2-min run is requested");
              isDAQRequest = 2;
            }else if(post_str=="stat"){
              if(isDAQRunning){
                client.println("DAQ is RUNNING now.");
              }else{
                client.println("DAQ is NOT running now.");
              }
            }else{
              isDAQRequest = 0;
              client.println("DAQ will be stopped.");
            }
          }
          break;
        }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}



void continuousMonitoring() {
  static uint32_t logTime = micros();
  static const uint32_t monitorTime = 1*1000000; // 1 sec
  uint32_t curTime = micros();
  if(curTime>logTime){
    Serial.println("read ADC for constant monitoring");
    for(int i=0;i<nGauges;i++){
       adcMon[i] = adc->adc0->analogRead(readPin[i]);
    }

    #ifdef WATERLEAK_SENSOR
    wleakMon = adc->adc0->analogRead(waterLeakPin);
    Serial.print("wleak:");
    Serial.print(wleakMon);
    #endif

    timespec ts;
    qindesign::network::EthernetIEEE1588.readTimer(ts);
    NanoTime ptp_nano=timespecToNanoTime(ts);
    printTime(ptp_nano,dateMon);
    timeMon=curTime;

    while( logTime < curTime ){
      logTime += monitorTime;
    }
  }
}


void doAction(){

  static int RecordTime = 0;

  listenForEthernetClients(); // DAQ request should be sent by "wget --post-data" command.

  if(!isDAQRunning){
    //continuousMonitoring();
    if(isDAQRequest==1){
      RecordTime = 1000000*5;// in micro. i.e., = 5 sec
    }else if(isDAQRequest==2){
    //  RecordTime = 1000000*120;// in micro. i.e., = 120 sec
      RecordTime = 1000000*60; 
    }
  }
  if(isDAQRequest!=0 || isDAQRunning ){
    isDAQRunning = takeADCData(RecordTime);
  }

  if(!isDAQRunning){
    continuousMonitoring();
  }
  
//  return isDAQRunning;
}
