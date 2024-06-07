void csv2root(std::string csvfile){
  std::string rootfile = MyTools::sed(csvfile,".csv",".root");
  TFile fout(rootfile.c_str(),"recreate");

  TTree *trout = new TTree("tr","");
  int samp;
  int ADC;
  trout->Branch("samp",&samp,"samp/I");
  trout->Branch("ADC",&ADC,"ADC/I");

  std::ifstream ifs(csvfile.c_str());
  std::string line;
  std::getline(ifs,line);  // The first line is header. 
  while(std::getline(ifs,line)){
    if(line.empty()) continue;
    for(int i=0;i<line.length();i++){
      if(line[i]==',') line[i] = ' ';
    }
    std::stringstream ss(line);
    ss>>samp>>ADC;
    trout->Fill();
  }
  trout->Write();
  fout.Close();
}

  
