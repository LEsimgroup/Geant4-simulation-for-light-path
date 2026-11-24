#include<TH1F.h>
#include<TSpectrum.h>
#include<TPolyMarker.h>
#include<TLatex.h>
#include <TString.h>
#include<vector>
#include<map>
#include<iostream>
#include <sstream>
#include <string>

TH1D *h_angle    = new TH1D ("h_angle","h_angle",50,0,200);


TCanvas *c1 = new TCanvas();
void angular_distribution()
{
TFile *ipf = new TFile("./build/output.root");
ipf->cd();
TTree *tree = (TTree*)ipf->Get("Scoring");
Double_t        fEdep;
Double_t        theta;
Double_t        tid;
Double_t        g_theta;

fEdep = 0.0 ;
theta = 0.0 ;
tid = 0.0;
g_theta = 0.0;

 tree->SetBranchAddress("fEdep", &fEdep);
 tree->SetBranchAddress("theta", &theta);
 tree->SetBranchAddress("tid", &tid);
 tree->SetBranchAddress("g_theta", &g_theta);

for (Long64_t jentry = 0 ; jentry < tree->GetEntriesFast() ; jentry++)
	{
		tree->GetEntry(jentry);
		if(jentry%1000000 ==0)
			cout<<"jentry = "<<jentry<<endl;
		if (fEdep <1.038 ) continue;
		if(abs(theta-72)<5)
			h_angle->Fill(72,1/7.0);
		if(abs(theta-85)<5)
			h_angle->Fill(85,1/5.0);
		if(abs(theta-95)<5)
			h_angle->Fill(95,1/5.0);
		if(abs(theta-107)<5)
			h_angle->Fill(107,1/10.0);
		if(abs(theta-133)<5)
			h_angle->Fill(133,1/10.0);
		if(abs(theta-157)<5)
			h_angle->Fill(157,1/5.0);
	}
h_angle->Draw("E1");
c1->Draw();
TString  OutputPath, outfile;
OutputPath = "/home/yuxh/Desktop/gg4_personal/juro_final_angular_correlation_Pb/";
outfile.Form("%sE2_24ns_angular_distribution.root", OutputPath.Data());
h_angle->SaveAs(outfile);
ipf->Close();
}
