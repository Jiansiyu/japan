using namespace ROOT;




/******* Channel class for storing information regarding a particular device *******/

// Class initialization
class Channel{ 
    public: 
    TString name;                           // Name of channel. Ends with _mean, _rms, or _slope according to aggregator convention.
    double value;                           // value for the slug
    double value_err;                       // error for the slug 
    double value_cor;                       // sign corrected value for the slug
    double value_cor_err;                   // error on sign corrected value for the slug
  
    Channel (TString a): name (a)  {};
    void draw(TTree *, TString);
    void drawRange(TTree *, TString, Int_t, Int_t);
    TGraphErrors drawReg(Int_t, Double_t *, Double_t *, Double_t *);
    void printInfo() { std::cout << "Plotting "<< name<<" vs slug number" << endl;}
};  

//Draw, but only in a range of slug numbers
void Channel::drawRange(TTree* tree,TString output, Int_t min = 0, Int_t max = 100000){
    // Overload draw() by default initializing range to 0-10000
    // But enable user specified cutting if wanted
    auto chan_name=this->name;
    auto chan_t=tree;
    TTree* cut_tree = tree->CopyTree(Form("n_runs>=%d && n_runs<=%d",min,max));
    draw(cut_tree, output);
}

void Channel::draw(TTree* tree,TString output){
auto chan_name=this->name;
auto chan_t=tree;

// global cuts
TString gc1= "&&"+ chan_name+"!=0 && abs("+ chan_name+")<0.99e6";
TString gc2= "&&"+ chan_name+"_error!=0 && abs("+chan_name+"_error)<0.99e6";
//
// device cut
TString lc="";
if (chan_name.Contains("usl")|| chan_name.Contains("dsl") || chan_name.Contains("left_avg") || chan_name.Contains("left_dd")){
    lc+= "&& hrs!=1";
}
if (chan_name.Contains("usr")|| chan_name.Contains("dsr") || chan_name.Contains("right_avg") || chan_name.Contains("right_dd")){
    lc+= "&& hrs!=2";
}
if (chan_name.Contains("us_avg")|| chan_name.Contains("ds_avg") || chan_name.Contains("us_dd") || chan_name.Contains("ds_dd")){
    lc+= "&& hrs!=1 && hrs!=2";
}
//
TString cut1="ihwp==1 && wein==1"+ gc1+ gc2+ lc;
TString cut2="ihwp==2 && wein==1" + gc1+ gc2+ lc;
TString cut3="ihwp==1 && wein==2"+gc1+gc2+lc;
TString cut4="ihwp==2 && wein==2"+gc1+gc2+lc;

TTreeFormula f("name",chan_name, chan_t);

if(f.GetNdim()!=0){
TCanvas c(chan_name, chan_name, 1200,1000);

Int_t nEntries=chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut1 , "goff");
TGraphErrors g1 = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());
    
nEntries= chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut2 ,  "goff");
TGraphErrors g2 = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

nEntries= chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error", cut3, "goff");
TGraphErrors g3= drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

nEntries =chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error", cut4, "goff");
TGraphErrors g4= drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

if (  !chan_name.Contains("yield") &&  !chan_name.Contains("rms")){ 
for( auto i=0; i< nEntries; i ++){
  std::cout<< chan_name<<" : "<< *(chan_t->GetV1()+i) << std::endl;
 *(chan_t->GetV2()+i)=-1*(*(chan_t->GetV2()+i));  
}
}
TGraphErrors g2_cor = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());
/*
nEntries.push_back(chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error","run_number==1 && split_n==2",  "goff");
nEntries.push_back(chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error","run_number==2 && split_n==2",  "goff");  
*/

gStyle->SetOptFit(0);

TF1* fconst1=new TF1("fconst1", "pol0");
TF1* fconst2=new TF1("fconst2", "pol0");
TF1* fconst3=new TF1("fconst3", "pol0");
TF1* fconst4=new TF1("fconst4", "pol0");
TF1* fconst_p=new TF1("fconst_p", "pol0");
TF1* fconst_m=new TF1("fconst_m", "pol0");


g1.SetMarkerColor(kBlue);
g1.SetMarkerStyle(20);
fconst1->SetLineColor(kBlue);
g1.Fit(fconst1,"Q");

g2.SetMarkerColor(kRed);
g2.SetMarkerStyle(21);
g2_cor.SetMarkerColor(kRed);
g2_cor.SetMarkerStyle(21);
fconst2->SetLineColor(kRed);
g2.Fit(fconst2,"Q");
fconst3->SetLineColor(kRed);
g2_cor.Fit(fconst3,"Q");

g3.SetMarkerColor(kMagenta);
g3.SetMarkerStyle(22);
fconst3->SetLineColor(kMagenta);
g3.Fit(fconst3, "Q");

g4.SetMarkerColor(kGreen+4);
g4.SetMarkerStyle(23);
fconst4->SetLineColor(kGreen+4);
g4.Fit(fconst4, "Q");

TMultiGraph mg;
mg.Add(&g1);
mg.Add(&g2);
mg.Add(&g3);
mg.Add(&g4);
TMultiGraph mg1;
mg1.Add(&g1);
mg1.Add(&g4);
mg1.Fit(fconst_p, "Q");
TMultiGraph mg2;
mg2.Add(&g2);
mg2.Add(&g3);
mg2.Fit(fconst_m, "Q");
mg.SetTitle(chan_name+" vs slug");
/*
if (!chan_name.Contains("_rms")&& !chan_name.Contains("_mean")){
  gPad->Modified();
  mg.SetMaximum(TMath::Max(g1.GetMean(2),g4.GetMean(2))+6*TMath::Max(g1.GetRMS(2),g4.GetRMS(2)));
  mg.SetMinimum(TMath::Min(g2.GetMean(2),g3.GetMean(2))-6*TMath::Max(g2.GetRMS(2),g3.GetRMS(2)));
}
*/
 c.Clear();
 TPad *p1 = new TPad("p1","",0,0.2,1,1);
 p1->SetBottomMargin(0.08);
 p1->Draw();
 TPad *p2 = new TPad("p2","",0,0,1,0.2);
 p2->SetTopMargin(0);
 p2->SetLeftMargin(0);
 p2->SetRightMargin(0);
 p2->Draw("B");
 p1->cd();

mg.Draw("ap");
p2->cd();
TLatex *pt1= new TLatex(0.0,0.8, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Right In:", fconst1->GetParameter(0), fconst1->GetParError(0), "#chi^{2}/NDF", fconst1->GetChisquare(), fconst1->GetNDF() ));
pt1->SetNDC();
pt1->SetTextFont(43);
pt1->SetTextSize(26);
pt1->SetTextColor(kBlue);
pt1->Draw();
TLatex *pt2= new TLatex(0.0,0.4, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Right Out:", fconst2->GetParameter(0), fconst2->GetParError(0), "#chi^{2}/NDF", fconst2->GetChisquare(), fconst2->GetNDF() ));
pt2->SetNDC();
pt2->SetTextFont(43);
pt2->SetTextSize(26);
pt2->SetTextColor(kRed);
pt2->Draw();
TLatex *pt3= new TLatex(0.0,0.2, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","Left In:", fconst3->GetParameter(0), fconst3->GetParError(0), "#chi^{2}/NDF", fconst3->GetChisquare(), fconst3->GetNDF() ));
pt3->SetNDC();
pt3->SetTextFont(43);
pt3->SetTextSize(26);
pt3->SetTextColor(kMagenta);
pt3->Draw();
TLatex *pt4= new TLatex(0.0,0.6, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","Left Out:", fconst4->GetParameter(0), fconst4->GetParError(0), "#chi^{2}/NDF", fconst4->GetChisquare(), fconst4->GetNDF() ));
pt4->SetNDC();
pt4->SetTextFont(43);
pt4->SetTextSize(26);
pt4->SetTextColor(kGreen+4);
pt4->Draw();
TLatex *pt_p= new TLatex(0.45,0.7, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","", fconst_p->GetParameter(0), fconst_p->GetParError(0), "#chi^{2}/NDF", fconst_p->GetChisquare(), fconst_p->GetNDF() ));
pt_p->SetNDC();
pt_p->SetTextFont(43);
pt_p->SetTextSize(26);
pt_p->SetTextColor(kCyan+4);
pt_p->Draw();
TLatex *pt_m= new TLatex(0.45,0.3, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","", fconst_m->GetParameter(0), fconst_m->GetParError(0), "#chi^{2}/NDF", fconst_m->GetChisquare(), fconst_m->GetNDF() ));
pt_m->SetNDC();
pt_m->SetTextFont(43);
pt_m->SetTextSize(26);
pt_m->SetTextColor(kRed+3);
pt_m->Draw();

/*
c->cd(1);
TMultiGraph mg_cor;
mg_cor.Add(&g1);
mg_cor.Add(&g2_cor);
mg_cor.SetTitle(chan_name+" vs slug (sign corrected)");
mg_cor.Draw("ap");
*/
c.Print(output+".pdf");
}
}



TGraphErrors Channel::drawReg(Int_t nEntries, Double_t *v1, Double_t *v2, Double_t *v3){

auto chan_name =  (this->name).Data(); 
TGraphErrors g(nEntries,v1, v2, 0, v3);
return g;
}



/***********************************End Class Channel*************************************/




class Source {
    TString file, tree, output;
       
  public:
    std::vector<Channel> list;
    TChain *T;   
    Source (TString a, TString b, TString c): file(a), tree(b), output(c) {T=new TChain(tree); T->Add(file);}
    void printInfo() { std::cout << "Reading from  " << tree  << " tree in file " << std::endl;} 
    void drawAll();
    void drawAllRange(Int_t min, Int_t max);
    Channel GetChannelByName(TString name);    

};

void Source::drawAll(){
  
auto filename= this->output;

TObjArray *var_list=(this->T)->GetListOfBranches();
TIter var_iter(var_list);

while (auto *var= var_iter.Next()){
   TString name(var->GetName());
   bool createPlotObj = ((name.Contains("_mean") || name.Contains("_slope") || name.Contains("_pslope")|| name.Contains("_rms")) && !(name.Contains("_error"))); 
   if (createPlotObj) {
     Channel channel(var->GetName());     
     channel.printInfo();
     channel.draw(this->T, filename);
     (this->list).push_back(channel);
   
   }
}   

void Source::drawAllRange(Int_t min = 0, Int_t max = 10000){
  
auto filename= this->output;

TObjArray *var_list=(this->T)->GetListOfBranches();
TIter var_iter(var_list);

while (auto *var= var_iter.Next()){
   TString name(var->GetName());
   bool createPlotObj = ((name.Contains("_mean") || name.Contains("_slope") || name.Contains("_pslope")|| name.Contains("_rms")) && !(name.Contains("_error"))); 
   if (createPlotObj) {
     Channel channel(var->GetName());     
     channel.printInfo();
     channel.drawRange(this->T, filename, min, max);
     (this->list).push_back(channel);
   
   }
}   


}

Channel Source::GetChannelByName(TString name){

Channel chan("");
for(auto iter=(this->list).begin();iter!=(this->list).end();iter++){
 if (iter->name==name){
  chan=*iter;
  break;
 }
} 

return chan;

}




