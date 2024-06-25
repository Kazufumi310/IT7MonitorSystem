

void doPressureStrainAction(){
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
