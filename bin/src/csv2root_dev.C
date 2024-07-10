class Header{
 public:
  Header();
  
  std::string unit;
  int nCh;
  static int const ARRSIZE = 16;
  double ADC2mV[ARRSIZE];
  double mV2val[ARRSIZE];
  double gain[ARRSIZE];
  double rawtime2sec;
  double DAQ_start_date;
  double DAQ_start_clock;
  double DAQ_end_date;
  double DAQ_end_clock;
  double PTP_calib_date;
  double PTP_calib_clock;

  void readSettingFile(std::string headerfile,std::string timingfile){
    readHeaderFile(headerfile);
    readTimingFile(timingfile);
    if(trout!=0){
      trout->Fill();
    }
  }
  void setTree(TTree* trheader){
    trout = trheader;
    branch();
  }
  

 private:
  void readHeaderFile(std::string headerfile);
  void readTimingFile(std::string timingfile);
  void branch();
  TTree* trout;
};

Header::Header(){
  nCh = 0;
  trout = 0;
  for(int i=0;i<ARRSIZE;i++){
    gain[i] = 1.;
    ADC2mV[i] = mV2val[i] = 0;
  }
  rawtime2sec = 1;
  DAQ_start_date = 0;
  DAQ_start_clock = 0;
  DAQ_end_date = 0;
  DAQ_end_clock = 0;
  PTP_calib_date = 0;
  PTP_calib_clock = 0;
}

void Header::readHeaderFile(std::string headerfile){
  std::ifstream ifs(headerfile);
  if(!ifs){
    std::cout<<"ERROR : can't find header file named "<<headerfile<<std::endl;
    exit(1);
  }
  std::string line;
  while(std::getline(ifs,line)){
    if(line.empty()) continue;
    if(line[0]=='#') continue; // comment
    std::stringstream ss(line);
    std::string type;
    ss>>type;
    if(type == "nCh"){
      ss >> nCh;
    }else if( type == "gain" || type == "ADC2mV" || type == "mV2val" ){
      int ich = 0;
      double *out;
      if( type == "gain" ){
	out = gain;
      }else if( type == "ADC2mV" ){
	out = ADC2mV;
      }else if( type == "mV2val" ){
	out = mV2val;
      }
      while(ss){
	ss>>out[ich];
	if(ss) ich++;
      }
      if(ich!=nCh){
	std::cout<<"error in line:"<<line<<std::endl;
	std::cout<<"# of values are different from nCh(="<<nCh<<")"<<std::endl;
	exit(1);
      }
    }else if(type == "rawtime2sec" ){
      ss>>rawtime2sec;
    }else if(type == "unit" ){
      ss>>unit;
    }else{
      std::cout<<"warning. unknown type in header file : "<<type<<std::endl; 
    }
  }
}


void Header::readTimingFile(std::string timingfile){
  std::ifstream ifs(timingfile);
  if(!ifs){
    std::cout<<"WARNING : can't find timing file named "<<timingfile<<std::endl;
    std::cout<<"The \"date\" should be no meaning value."<<std::endl;
  }
  std::string line;
  while(std::getline(ifs,line)){
    if(line[0]=='#') continue; // comment
    
    for(int i=0;i<line.length();i++){
      if(line[i]==',') line[i] = ' ';
    }
    std::stringstream ss(line);
    std::string first;
    ss>>first;
    if(first=="micros") continue; // header
    if(first=="DAQ_start:"){
      ss>>DAQ_start_clock;
      DAQ_start_clock /= 1e6; // usec to sec
      ss>>DAQ_start_date;
      DAQ_start_date /= 1e9; //nsec to sec
    }else if(first=="DAQ_end:"){
      ss>>DAQ_end_clock;
      DAQ_end_clock /= 1e6; // usec to sec
      ss>>DAQ_end_date;
      DAQ_end_date /= 1e9; // nsec to sec
    }else if(first[0]>='0' && first[0]<='9'){
      PTP_calib_clock = atof(first.c_str());
      PTP_calib_clock /= 1e6; // usec to sec
      ss>>PTP_calib_date;
      PTP_calib_date /= 1e9; // nsec to sec
    }else{
      std::cout<<"unexpected description in timing file : "<<line<<std::endl;
      continue;
    }
  }
}

void Header::branch(){
  if(trout==0){
    std::cout<<"error in Header! Output TTree is not specified."<<std::endl;
    exit(1);
  }
  trout->Branch("unit",&unit);
  trout->Branch("nCh",&nCh,"nCh/I");
  trout->Branch("ADC2mV",ADC2mV,"ADC2mV[nCh]/D");
  trout->Branch("mV2val",mV2val,"mV2val[nCh]/D");
  trout->Branch("gain",gain,"gain[nCh]/D");
  trout->Branch("rawtime2sec",&rawtime2sec,"rawtime2sec/D");
  trout->Branch("DAQ_start_date",&DAQ_start_date,"DAQ_start_date/D");
  trout->Branch("DAQ_start_clock",&DAQ_start_clock,"DAQ_start_clock/D");
  trout->Branch("DAQ_end_date",&DAQ_end_date,"DAQ_end_date/D");
  trout->Branch("DAQ_end_clock",&DAQ_end_clock,"DAQ_end_clock/D");
  trout->Branch("PTP_calib_date",&PTP_calib_date,"PTP_calib_date/D");
  trout->Branch("PTP_calib_clock",&PTP_calib_clock,"PTP_calib_clock/D");
}


class BaseData{
 public:
  int nCh;
  double rawtime;
  double localtime;
  double date;
  double ADC[16];
  double mV[16];
  double val[16];
  void setTree(TTree* tree){
    trout = tree;
    branch();
  }
  void readDataFile(std::string datafile, Header &header);
  
 public:
  virtual void discardHeader(std::ifstream& ifs){;}
  virtual void readOneLine(std::string line){;}
  virtual bool convert(Header &header){return false;}

  virtual void branch();
  
  TTree* trout;
};


void BaseData::branch(){
  if(trout==0){
    std::cout<<"error! Output TTree is not specified."<<std::endl;
    exit(1);
  }
  trout->Branch("rawtime",&rawtime,"rawtime/D");
  trout->Branch("localtime",&localtime,"localtime/D");
  trout->Branch("date",&date,"date/D");
  trout->Branch("nCh",&nCh,"nCh/I");
  trout->Branch("ADC",ADC,"ADC[nCh]/D");
  trout->Branch("mV",mV,"mV[nCh]/D");
  trout->Branch("val",val,"val[nCh]/D");
}

void BaseData::readDataFile(std::string csvfile,Header &header){
  std::ifstream ifs(csvfile.c_str());

  discardHeader(ifs);
  std::string line;
  while(std::getline(ifs,line)){
    if(line.empty()) continue;
    for(int i=0;i<line.length();i++){
      if(line[i]==',') line[i] = ' ';
    }

    readOneLine(line);
    if(!convert(header)){
      std::cout<<"WARNING: "<<line<<std::endl;
      continue;
    }
    trout->Fill();
  }
}

class KellerData : public BaseData{
  void discardHeader(std::ifstream& ifs);
  void readOneLine(std::string line);
  bool convert(Header &header);
};

void KellerData::discardHeader(std::ifstream& ifs){
  std::string line;
  std::getline(ifs,line); // the 1st line is header.
  std::getline(ifs,line); // the 1st ADC data is not valid.
}

void KellerData::readOneLine(std::string line){
  std::stringstream ss(line);
  ss>>rawtime>>ADC[0];
}

bool KellerData::convert(Header &header){
  nCh = header.nCh; // should be 1
  localtime = rawtime*header.rawtime2sec;
  date =  header.DAQ_start_date + localtime;

  for(int ich=0;ich<nCh;ich++){
    mV[ich] = ADC[ich] / header.gain[ich] * header.ADC2mV[ich];
    val[ich] = mV[ich] * header.mV2val[ich];
  }
  return true;
}


class StrainData : public BaseData{
 public:
  double mV2strain(double mV);
 public:
  void discardHeader(std::ifstream& ifs);
  void readOneLine(std::string line);
  bool convert(Header &header);
};

void StrainData::discardHeader(std::ifstream& ifs){
  ; // no header
}

void StrainData::readOneLine(std::string line){
  std::stringstream ss(line);
  ss>>rawtime; // already usec unit
  nCh=0;
  while(ss){
    ss>>ADC[nCh];
    if(ss) nCh++;
  }
}

double StrainData::mV2strain(double mV){
  double const VE = 5000.;
  double const GF = (120*2.1)/(120+(0.44*3));
  double st = (-2*mV/VE) / (GF / 2. + mV*GF/VE);
  return st;
}

bool StrainData::convert(Header &header){
  if(nCh != header.nCh){
    std::cout<<"WARNING: #channels written in data file != that written in header"<<std::endl;
    return false;
  }
  localtime = rawtime*header.rawtime2sec;
  date = header.DAQ_start_date + localtime;

  for(int ich=0;ich<nCh;ich++){
    mV[ich] = ADC[ich] / header.gain[ich] * header.ADC2mV[ich];
    if(header.mV2val[ich]==0){
      val[ich] = mV2strain(mV[ich]);
    }else{
      val[ich] = mV[ich] * header.mV2val[ich];
    }
  }
  return true;
}

class TourmalineData : public BaseData{
 public:
  void discardHeader(std::ifstream& ifs);
  void readOneLine(std::string line);
  bool convert(Header &header);
};

void TourmalineData::discardHeader(std::ifstream& ifs){
  ; // no header
}

void TourmalineData::readOneLine(std::string line){
  std::stringstream ss(line);
  nCh=0;
  while(ss){
    ss>>ADC[nCh];
    if(ss) nCh++;
  }
  rawtime+=1; 
}

bool TourmalineData::convert(Header &header){
  if(nCh != header.nCh){
    std::cout<<"WARNING: #channels written in data file != that written in header"<<std::endl;
    return false;
  }
  
  localtime = rawtime * header.rawtime2sec;
  date =  header.DAQ_start_date + localtime;
  
  for(int ich=0;ich<nCh;ich++){
    mV[ich] = ADC[ich] / header.gain[ich] * header.ADC2mV[ich];
    val[ich] = mV[ich] * header.mV2val[ich];
  }
  
  return true;
}

class AccelerometerData : public BaseData{
 public:
  static int const nChOnLogger = 8;
  static int const nSensor = 5;
  double acc3D[nSensor][3];
  double rawtime2;
  double localtime2;
  double date2;

 public:
  void branch();
  
 public:
  void discardHeader(std::ifstream& ifs);
  void readOneLine(std::string line);
  bool convert(Header &header);
};

void AccelerometerData::branch(){
  BaseData::branch();
  trout->Branch("rawtime2",&rawtime2,"rawtime2/D");
  trout->Branch("localtime2",&localtime2,"localtime2/D");
  trout->Branch("date2",&date2,"date2/D");
  trout->Branch("acc3D",acc3D,Form("acc3D[%d][3]/D",nSensor));
}

void AccelerometerData::discardHeader(std::ifstream& ifs){
  ; // no header
}

void AccelerometerData::readOneLine(std::string line){
  std::stringstream ss(line);
  for(int iLogger=0;iLogger<2;iLogger++){
    if(iLogger==0)  ss>>rawtime;
    else  ss>>rawtime2;
    
    for(int i=0;i<nChOnLogger;i++){
      ss>>ADC[i+iLogger*nChOnLogger];
    }
  }
}

bool AccelerometerData::convert(Header &header){
  nCh = header.nCh;
  localtime = rawtime*header.rawtime2sec;
  localtime2 = rawtime2*header.rawtime2sec;
  date =  header.DAQ_start_date + localtime;
  date2 =  header.DAQ_start_date + localtime2;

  for(int ich=0;ich<nCh;ich++){
    mV[ich] = ADC[ich] / header.gain[ich] * header.ADC2mV[ich];
    val[ich] = mV[ich] * header.mV2val[ich];
  }
  int const nSensor = 5;
  for(int isensor = 0;isensor<nSensor;isensor++){
    for(int idir = 0;idir<3;idir++){
      acc3D[isensor][idir] = val[isensor*3+idir];
    }
  }
  return true;
}


enum{
  KELLER = 1, STRAIN=2, TOURMALINE = 3, ACCELEROMETER = 4
};

void csv2root(std::string csvfile, std::string headerfile, std::string timingfile,int mode){
  BaseData *data;
  if(mode==KELLER){
    data = new KellerData();
  }else if(mode==STRAIN){
    data = new StrainData();
  }else if(mode==TOURMALINE){
    data = new TourmalineData();
  }else if(mode==ACCELEROMETER){
    data = new AccelerometerData();
  }else{
    std::cout<<"Data type "<<mode<<" is not supported"<<std::endl;
    return;
  }

  
  std::string rootfile = MyTools::sed(csvfile,".csv",".root");
  TFile fout(rootfile.c_str(),"recreate");

  TTree *trheader = new TTree("trheader","");
  Header header;
  header.setTree(trheader);
  header.readSettingFile(headerfile,timingfile);

  TTree *trout = new TTree("tr","");
  data->setTree(trout);
  data->readDataFile(csvfile,header);

  trheader->Write();
  trout->Write();

  
  
  fout.Close();
}


  
