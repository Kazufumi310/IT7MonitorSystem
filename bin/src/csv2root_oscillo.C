void csv2root(std::string csvfile){
  std::string rootfile = MyTools::sed(csvfile,".csv",".root");
  TFile fout(rootfile.c_str(),"recreate");

  TTree *trout = new TTree("tr","");
  int samp=0;
  int nCh=0;
  double volt[16];
  trout->Branch("samp",&samp,"samp/I");
  trout->Branch("nCh",&nCh,"nCh/I");
  trout->Branch("volt",volt,"volt[nCh]/D");

  std::ifstream ifs(csvfile.c_str());
  std::string line;
  std::getline(ifs,line);  // The first line is header. 
  while(std::getline(ifs,line)){
    if(line.empty()) continue;
    for(int i=0;i<line.length();i++){
      if(line[i]==',') line[i] = ' ';
    }
    std::stringstream ss(line);
    nCh=0;
    while(ss){
      ss>>volt[nCh];
      if(ss) nCh++;
    }
    trout->Fill();
    samp++;
  }
  trout->Write();
  fout.Close();
}

  
