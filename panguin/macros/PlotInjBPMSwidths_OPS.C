// Injector BPMs Plot Macro 
// Source from Caryn Palatchi
// Adapted by Tao Ye for JAPAN 
// Adapted by KDP and PK for panguin
// Last Update : 05-2019

//nclude "device_list.h"
// vector<const char*> vBPM = {"bpm2i01","bpm2i02","bpm1i02","bpm1i04",
//                                "bpm1i06", "bpm0i01","bpm0i01a","bpm0i02",
//                                "bpm0i02a","bpm0i05","bpm0i07","bpm0l01",
//                                "bpm0l02","bpm0l03","bpm0l04","bpm0l05",
//                                "bpm0l06","bpm0l07","bpm0l10", "bpm0r02",
//                                "bpm0r05"};

void PlotInjBPMSwidths_OPS(TString ucut){

  vector<const char*> vBPM = {"2i01","2i02","1i02","1i04",
                               "1i06", "0i01","0i01a","0i02",
                               "0i02a","0i05","0i07","0l01",
                               "0l02"};

  const int intper = 900;
  //collect data points from first run file
  TTree* evt_tree = (TTree*)gROOT->FindObject("evt");
  TTree* pair_tree = (TTree*)gROOT->FindObject("pr");
  TTree* mul_tree = (TTree*)gROOT->FindObject("mul");

  Int_t ndeti = vBPM.size();
  Double_t* Aq= new Double_t[ndeti];
  Double_t* eAq= new Double_t[ndeti];
  Double_t* rmsAq= new Double_t[ndeti];
  Double_t* Dx= new Double_t[ndeti];
  Double_t* Dy= new Double_t[ndeti];
  Double_t* eDx= new Double_t[ndeti];
  Double_t* eDy= new Double_t[ndeti];
  Double_t* rmsDx= new Double_t[ndeti];
  Double_t* rmsDy= new Double_t[ndeti];
  Double_t* zero= new Double_t[ndeti];

  Int_t nument = mul_tree->GetEntries();

  TString lcut = ucut +  Form(" && Entry$>(%d-%d)",nument,intper) ;

  TPad *pad1= new TPad("pad1","pad1",0,0,1,1);
  // pad1->Divide(2,2);
  pad1->Draw();
  TH1D* thishist;
  TString objnam;
  for(Int_t dd=0;dd<ndeti;dd++) {
    mul_tree->Draw(Form("1e6*asym_bpm%sWS>>hista%d",vBPM[dd],dd),lcut,"goff");
    objnam = Form("hista%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Aq[dd] = thishist->GetMean();
    eAq[dd]= thishist->GetMeanError();
    rmsAq[dd]= thishist->GetRMS();    
    zero[dd]=0;
    // delete thishist;
    mul_tree->Draw(Form("1e3*diff_bpm%sX>>histx%d",vBPM[dd],dd),lcut,"goff");
    objnam = Form("histx%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Dx[dd] = thishist->GetMean();
    eDx[dd]= thishist->GetMeanError();
    rmsDx[dd]= thishist->GetRMS();
    // delete thishist;
    mul_tree->Draw(Form("1e3*diff_bpm%sY>>histy%d",vBPM[dd],dd),lcut,"goff");
    objnam = Form("histy%d",dd);
    thishist = (TH1D*)gDirectory->FindObject(objnam);
    Dy[dd] = thishist->GetMean();
    eDy[dd]= thishist->GetMeanError();
    rmsDy[dd]= thishist->GetRMS();
    // delete thishist;
    // cout<<vBPM[dd]<<" Aq[ppm](RMS)+-error,Dx[nm](RMS)+-err,Dy[nm](RMS)+-err: "<<Aq[dd]<<" "<<rmsAq[dd]<<" "<<eAq[dd]<<" "<<1e3*Dx[dd]<<" "<<1e3*rmsDx[dd]<<" "<<1e3*eDx[dd]<<" "<<1e3*Dy[dd]<<" "<<1e3*rmsDy[dd]<<" "<<1e3*eDy[dd]<<endl;

  }

  const Int_t numchains=ndeti;
  Double_t chain[numchains];
  Double_t chainerr[numchains];
  for (Int_t i=0;i<numchains;i++){
    chain[i]=i+1;
    chainerr[i]=0.0;
  };

  TGraphErrors *Dxgraph;
  TGraphErrors *Dygraph;
  TGraphErrors *Dxrmsgraph;
  TGraphErrors *Dyrmsgraph;
  TGraphErrors *Aqgraph;
  TGraphErrors *Aqrmsgraph;

  Dxgraph = new TGraphErrors(numchains,chain,&(Dx[0]),chainerr,&(eDx[0]));
  Dygraph= new TGraphErrors(numchains,chain,&(Dy[0]),chainerr,&(eDy[0]));
  Dxrmsgraph = new TGraphErrors(numchains,chain,&(rmsDx[0]),chainerr,&(zero[0]));
  Dyrmsgraph= new TGraphErrors(numchains,chain,&(rmsDy[0]),chainerr,&(zero[0]));
  Aqgraph= new TGraphErrors(numchains,chain,&(Aq[0]),chainerr,&(eAq[0]));
  Aqrmsgraph = new TGraphErrors(numchains,chain,&(rmsAq[0]),chainerr,&(zero[0]));

  
  TMultiGraph *xygraph = new TMultiGraph();
  TMultiGraph *xyrmsgraph = new TMultiGraph();

  // TAxis *ax = Dygraph->GetHistogram()->GetXaxis();
  // Double_t x1 = ax->GetBinLowEdge(1);
  // Double_t x2 = ax->GetBinUpEdge(ax->GetNbins());
  Double_t x1 = chain[0]-0.5;
  Double_t x2 = chain[numchains-1]+0.5;

  TVirtualPad* pad_buff;



  pad_buff = pad1->cd();
  Aqrmsgraph->SetMarkerStyle(21);
  Aqrmsgraph->SetMarkerColor(4);
  Aqrmsgraph->SetLineColor(4);
  Aqrmsgraph->Draw("AP");

  Aqrmsgraph->SetTitle(Form("Aq RMS: Cut =  %s ",ucut.Data()));
  Aqrmsgraph->GetYaxis()->SetTitle("RMS ppm");
  Aqrmsgraph->GetHistogram()->GetXaxis()->Set(numchains,x1,x2);
  for(Int_t k=0;k<numchains;k++){
    Aqrmsgraph->GetHistogram()->GetXaxis()->SetBinLabel(k+1,vBPM[k]);
  }
  Aqrmsgraph->GetHistogram()->GetXaxis()->SetLabelSize(0.05);
  Aqrmsgraph->GetHistogram()->GetXaxis()->LabelsOption("v");
  pad_buff->SetGrid();





 


}

