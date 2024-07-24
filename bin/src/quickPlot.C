enum{
  KELLER1=0, KELLER2, STRAIN, TOURMALINE, ACC
};

enum{
  ADC=0,MV,VAL
};

void quickPlot(int nth = 0){
  //  Plotter::getPlotter()->setPsFile("plot%d.ps",nth);
  
  int const nDir = 5;
  std::string dirName[nDir] = {
    "pressure1","pressure2","strain","tourmaline","accelerometer"
  };
  TChain *ch[nDir]={0};
  for(int i=0;i<nDir;i++){
    std::cout<<"i:"<<i<<std::endl;
    std::string filelist = MyTools::cmdOutput("ls "+dirName[i]+"/*.root");
    std::cout<<"fileList:"<<filelist<<std::endl;
    if(filelist.empty()) continue;
    std::stringstream ss(filelist);
    std::vector<std::string> fvec;
    std::string fname;
    while(ss){
      ss>>fname;
      if(fname.empty()) continue;
      fvec.push_back(fname);
    }
    if(fvec.size()>nth){
      fname = fvec[nth];
    }else{
      fname = fvec.back();
    }
    std::cout<<"reading "<<fname<<std::endl;
    ch[i] = new TChain("tr");
    ch[i]->Add(fname.c_str());
    if(ch[i]->GetEntries()==0){
      std::cout<<fname<<" is empty or doesn't exist. "<<std::endl;
      delete ch[i];
      ch[i] = 0;
    }
  }
  Plotter::getPlotter()->canvas()->SetWindowSize(1200,800);
  Plotter::getPlotter()->canvas()->Divide(3,2);
  int const nPad = 6;
  TH2D *frame[nPad]={0}; //{tourmaline, KELLER, strain, accX, accY, accZ}
  double maxVal[3][nPad],minVal[3][nPad];
  double maxTime[2][nPad],minTime[2][nPad];
  std::string unit[nPad]={"MPa","MPa","strain","G","G","G"};
  for(int i=0;i<nPad;i++){
    for(int j=0;j<3;j++){
      maxVal[j][i] = -1e6;
      minVal[j][i] = 1e6;
    }
    for(int j=0;j<2;j++){
      maxTime[j][i] = -1e15;
      minTime[j][i] = 1e15;
    }
  }

  for(int i=0;i<3;i++){
    std::string v;
    if(i==0) v = "ADC";
    if(i==1) v = "mV";
    if(i==2) v = "val";
    if(ch[TOURMALINE]!=0){
      maxVal[i][0] = ch[TOURMALINE]->GetMaximum(v.c_str());
      minVal[i][0] = ch[TOURMALINE]->GetMinimum(v.c_str());
    }
    if(ch[KELLER1]!=0){
      maxVal[i][1] = std::max(maxVal[i][1],ch[KELLER1]->GetMaximum(v.c_str()));
      minVal[i][1] = std::min(minVal[i][1],ch[KELLER1]->GetMinimum(v.c_str()));
    }
    if(ch[KELLER2]!=0){
      maxVal[i][1] = std::max(maxVal[i][1],ch[KELLER2]->GetMaximum(v.c_str()));
      minVal[i][1] = std::min(minVal[i][1],ch[KELLER2]->GetMinimum(v.c_str()));
    }
    if(ch[STRAIN]!=0){
      maxVal[i][2] = ch[STRAIN]->GetMaximum(v.c_str());
      minVal[i][2] = ch[STRAIN]->GetMinimum(v.c_str());
    }
    if(ch[ACC]!=0){
      maxVal[i][3] =  maxVal[i][4] = maxVal[i][5] =  ch[ACC]->GetMaximum(v.c_str());
      minVal[i][3] =  minVal[i][4] = minVal[i][5] =  ch[ACC]->GetMinimum(v.c_str());
    }
  }
  std::cout<<"check ymin and ymax"<<std::endl;
  for(int ipad=0;ipad<nPad;ipad++){
    std::cout<<"ipad : "<<ipad<<std::endl;
    std::cout<<"ADC: "<<minVal[ADC][ipad]<<" - "<<maxVal[ADC][ipad]<<std::endl;
    std::cout<<"mV: "<<minVal[MV][ipad]<<" - "<<maxVal[MV][ipad]<<std::endl;
    std::cout<<"val: "<<minVal[VAL][ipad]<<" - "<<maxVal[VAL][ipad]<<std::endl;
  }

  for(int i=0;i<2;i++){
    std::string v;
    if(i==0) v = "localtime";
    if(i==1) v = "date";
    if(ch[TOURMALINE]!=0){
      maxTime[i][0] = ch[TOURMALINE]->GetMaximum(v.c_str());
      minTime[i][0] = ch[TOURMALINE]->GetMinimum(v.c_str());
    }
    if(ch[KELLER1]!=0){
      maxTime[i][1] = std::max(maxTime[i][1],ch[KELLER1]->GetMaximum(v.c_str()));
      minTime[i][1] = std::min(minTime[i][1],ch[KELLER1]->GetMinimum(v.c_str()));
    }
    if(ch[KELLER2]!=0){
      maxTime[i][1] = std::max(maxTime[i][1],ch[KELLER2]->GetMaximum(v.c_str()));
      minTime[i][1] = std::min(minTime[i][1],ch[KELLER2]->GetMinimum(v.c_str()));
    }
    if(ch[STRAIN]!=0){
      maxTime[i][2] = ch[STRAIN]->GetMaximum(v.c_str());
      minTime[i][2] = ch[STRAIN]->GetMinimum(v.c_str());
    }
    if(ch[ACC]!=0){
      maxTime[i][3] =  maxTime[i][4] = maxTime[i][5] =  ch[ACC]->GetMaximum(v.c_str());
      minTime[i][3] =  minTime[i][4] = minTime[i][5] =  ch[ACC]->GetMinimum(v.c_str());
    }
  }

  int xtype[nPad]={0,0,0,0,0,0},ytype[nPad]={0,0,0,0,0,0};
  double xrange[nPad][2], yrange[nPad][2];
  for(int ipad=0;ipad<nPad;ipad++){
    xrange[ipad][0] = minTime[0][ipad];
    xrange[ipad][1] = maxTime[0][ipad];
    yrange[ipad][0] = minVal[0][ipad];
    yrange[ipad][1] = maxVal[0][ipad];
  }
  int cpad = 0;
  bool drawNewly[nPad] = {true,true,true,true,true,true};
  double compBase = 1.;
  

  while(1){
    for(int ipad=0;ipad<nPad;ipad++){
      if(!drawNewly[ipad]) continue;
      if(ipad==0 && ch[TOURMALINE]==0 ) continue;
      if(ipad==1 && ch[KELLER1]==0 && ch[KELLER2]==0 ) continue;
      if(ipad==2 && ch[STRAIN]==0 ) continue;
      if(ipad>=3 && ch[ACC]==0 ) continue;
      
      std::cout<<"drawing "<<ipad<<"-th pad"<<std::endl;
      
      Plotter::getPlotter()->canvas()->cd(ipad+1);
      int compLevel = (xrange[ipad][1]-xrange[ipad][0])/compBase;

      std::cout<<"compLevel "<<compLevel<<std::endl;
      
      std::string xval = (xtype[ipad]==0) ? "localtime":"date";
      TCut xcut = (compLevel<=0)
	? Form("%s>%g&&%s<%g",xval.c_str(),xrange[ipad][0],xval.c_str(),xrange[ipad][1])
	: Form("%s>%g&&%s<%g&&Entry$%%%d==0",xval.c_str(),xrange[ipad][0],xval.c_str(),xrange[ipad][1],compLevel);

      std::cout<<"xaxis cut: "<<xcut.GetTitle()<<std::endl;
      
      if(ipad==0){
	ch[TOURMALINE]->Draw(">>elistT",xcut);
	TEventList *elist = (TEventList*)gDirectory->Get("elistT");
	std::cout<<"# entry to draw: "<<elist->GetN()<<std::endl;
	ch[TOURMALINE]->SetEventList(elist);
      }else if(ipad==1){
	if(ch[KELLER1]!=0){
	  ch[KELLER1]->Draw(">>elistP1",xcut);
	  TEventList *elist = (TEventList*)gDirectory->Get("elistP1");
	  std::cout<<"# entry to draw: "<<elist->GetN()<<std::endl;
	  ch[KELLER1]->SetEventList(elist);
	}
	if(ch[KELLER2]!=0){
	  ch[KELLER2]->Draw(">>elistP2",xcut);
	  TEventList *elist = (TEventList*)gDirectory->Get("elistP2");
	  ch[KELLER2]->SetEventList(elist);
	}
      }else if(ipad==2){
	ch[STRAIN]->Draw(">>elistS",xcut);
	TEventList *elist = (TEventList*)gDirectory->Get("elistS");
	std::cout<<"# entry to draw: "<<elist->GetN()<<std::endl;
	ch[STRAIN]->SetEventList(elist);
      }else{
	ch[ACC]->Draw(">>elistA",xcut);
	TEventList *elist = (TEventList*)gDirectory->Get("elistA");
	std::cout<<"# entry to draw: "<<elist->GetN()<<std::endl;
	ch[ACC]->SetEventList(elist);
      }
      
      if(frame[ipad]!=0) delete frame[ipad];
      double ymin = yrange[ipad][0] - (yrange[ipad][1]-yrange[ipad][0])*0.1;
      double ymax = yrange[ipad][1] + (yrange[ipad][1]-yrange[ipad][0])*0.1;
      frame[ipad] = new TH2D(Form("frame%d",ipad),";;",100,xrange[ipad][0],xrange[ipad][1],100,ymin,ymax);
      frame[ipad]->SetStats(0);
      if(xtype[ipad]==0) frame[ipad]->GetXaxis()->SetTitle("sec");

      std::string yval;
      if(ytype[ipad]==0) yval = "ADC";
      if(ytype[ipad]==1) yval = "mV";
      if(ytype[ipad]==2) yval = "val";
      if(ytype[ipad]==0) frame[ipad]->GetYaxis()->SetTitle("[ADC count]");
      if(ytype[ipad]==1) frame[ipad]->GetYaxis()->SetTitle("[mV]");
      if(ytype[ipad]==2) frame[ipad]->GetYaxis()->SetTitle(Form("%s",unit[ipad].c_str()));
      
      frame[ipad]->Draw();
      if(xtype[ipad]==1){
	MyTools::setTimeDisplay(frame[ipad]->GetXaxis(),"#splitline{%m/%d}{%H:%M:%S}");
      }
      
      
      if(ipad==0){
	for(int ich=0;ich<4;ich++){
	  ch[TOURMALINE]->SetLineColor(ich+2);
	  std::string drawTitle =Form("%s[%d]:%s",yval.c_str(),ich,xval.c_str());
	  std::cout<<"draw:"<<drawTitle<<std::endl;
	  ch[TOURMALINE]->Draw(drawTitle.c_str(),"","same l");
	}
      }else if(ipad==1){

	std::string drawTitle = Form("%s[0]:%s",yval.c_str(),xval.c_str());
	std::cout<<"draw:"<<drawTitle<<std::endl;
	if(ch[KELLER1]!=0){
	  ch[KELLER1]->SetLineColor(2);
	  ch[KELLER1]->Draw(drawTitle.c_str(),"","same l");
	}
	if(ch[KELLER2]!=0){
	  ch[KELLER2]->SetLineColor(4);
	  ch[KELLER2]->Draw(drawTitle.c_str(),"","same l");
	}
      }else if(ipad==2){
	for(int ich=0;ich<8;ich++){
	  ch[STRAIN]->SetLineColor(ich+1);
	  std::string drawTitle = Form("%s[%d]:%s",yval.c_str(),ich,xval.c_str());
	  std::cout<<"draw:"<<drawTitle<<std::endl;
	  ch[STRAIN]->Draw(drawTitle.c_str(),"","same l");
	}
      }else{
	for(int ich=0;ich<5;ich++){
	  ch[ACC]->SetLineColor(ich+2);
	  std::string drawTitle = Form("%s[%d]:%s",yval.c_str(),ich*3+ipad-3,xval.c_str());
	  std::cout<<"draw:"<<drawTitle<<std::endl;
	  ch[ACC]->Draw(drawTitle.c_str(),"","same l");
	}
      }
      drawNewly[ipad]=false;
    }
    Plotter::getPlotter()->canvas()->Modified();
    Plotter::getPlotter()->canvas()->Update();
    gSystem->ProcessEvents();
    
    std::cout<<"input command >> ";
    std::string key;
    std::getline(std::cin,key);
    if(key=="q") break;
    if(key=="p"){
      if(!Plotter::getPlotter()->isSetPsFile()) Plotter::getPlotter()->setPsFile(Form("plot%d.ps",nth));
      Plotter::getPlotter()->print();
    }else if(key[0]=='x'){
      std::stringstream ss(key);
      std::string trash;
      double x0,x1;
      ss>>trash>>x0>>x1;
      if(cpad==0){
	for(int ipad=0;ipad<6;ipad++){
	  xrange[ipad][0] = x0;
	  xrange[ipad][1] = x1;
	  drawNewly[ipad]=true;
	}
      }else{
	xrange[cpad-1][0] = x0;
	xrange[cpad-1][1] = x1;
	drawNewly[cpad-1]=true;
      }
    }else if(key[0]=='y'){
      std::stringstream ss(key);
      std::string trash;
      double x0,x1;
      ss>>trash>>x0>>x1;
      if(cpad==0){
	std::cout<<"set pad before change y-axis range"<<std::endl;
      }else{
	yrange[cpad-1][0] = x0;
	yrange[cpad-1][1] = x1;
	drawNewly[cpad-1]=true;
      }
    }else if(key[0]=='c'){
      std::stringstream ss(key);
      std::string com;
      ss>>com;
      if(com=="cd"){
	int pad;
	ss>>pad;
	cpad = pad;
      }else if(com=="convt"){
	std::cout<<"key "<<key<<std::endl;
	std::string str = key.substr(5);
	TDatime date(str.c_str());
	std::cout<<"unixtime of "<<str<<" is "<<date.Convert()<<std::endl;
      }
    }else if(key=="ADC" || key=="mV" || key=="val"){
      int TYPE=ADC;
      if(key=="ADC") TYPE=ADC;
      if(key=="mV") TYPE=MV;
      if(key=="val") TYPE=VAL;
      if(cpad==0){
	for(int ipad=0;ipad<6;ipad++){
	  ytype[ipad]=TYPE;
	  yrange[ipad][0] = minVal[TYPE][ipad];
	  yrange[ipad][1] = maxVal[TYPE][ipad];
	  drawNewly[ipad]=true;
	}
      }else{
	ytype[cpad-1]=TYPE;
	yrange[cpad-1][0] = minVal[TYPE][cpad-1];
	yrange[cpad-1][1] = maxVal[TYPE][cpad-1];
	drawNewly[cpad-1]=true;
      }
    }else if(key=="date" || key=="localtime"){
      int TYPE=0;
      if(key=="date") TYPE=1;
      if(cpad==0){
	for(int ipad=0;ipad<6;ipad++){
	  xtype[ipad]=TYPE;
	  xrange[ipad][0] = minTime[TYPE][ipad];
	  xrange[ipad][1] = maxTime[TYPE][ipad];
	  drawNewly[ipad]=true;
	}
      }else{
	xtype[cpad-1]=TYPE;
	xrange[cpad-1][0] = minTime[TYPE][cpad-1];
	xrange[cpad-1][1] = maxTime[TYPE][cpad-1];
	drawNewly[cpad-1]=true;
      }
    }else{
      std::cout<<"unknown command:"<<key<<std::endl;
    }
  }
}
