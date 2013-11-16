////////////////////////////////////////////////////////////////////////////////
///////// macro to read data from an ascii file and plot data on graph /////////
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "Math/WrappedTF1.h"
#include "Math/GSLIntegrator.h"
#include "TMath.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TVector3.h"
#include "TCut.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"

using namespace std;

#define MAX_NEUTRON_STEP 50 // Max number of steps before neutron stops in medium

///////////////////////////////
// Draw Average of Step Plot //
///////////////////////////////

void DrawPlotOfAverageValueOfStep(string titleOfXAxis, string titleOfYAxis, string nameOfVariableToPlot, int numberOfRuns, string nameOfVariableToCompare, string* nameOfRuns, TTree& tree, TCut& cut, TCut runNumberCut[], string titleOfFigure, int numberOfBins, double leftEdge, double rightEdge, double minHeight, double maxHeight, bool statsAreOnRightSide){
	TCut stepNumberCut[MAX_NEUTRON_STEP];
	for(int i = 0; i < MAX_NEUTRON_STEP; i++){
		stringstream stream;
		stream << i+1;
		string stringOfStepNumber = stream.str();
		stepNumberCut[i] = ("stepNumber == "+stringOfStepNumber).data();
	}
	string drawingOption;
	if(statsAreOnRightSide) drawingOption = "";
	else drawingOption = "y+";
	
	TCanvas* c0 = new TCanvas("c0", "Canvas 0");
	c0->cd();
	c0->SetLogy();
	TLegend* c0Legend;
	double particleStepNumber[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_0[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_1[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_2[MAX_NEUTRON_STEP] = {0.};

	int entries_0, entries_1, entries_2;
	for(int step = 0; step < MAX_NEUTRON_STEP; step++){
		TH1F histogram_0(nameOfRuns[0].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
		tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[0]).data(), runNumberCut[0]&&stepNumberCut[step]&&cut, "goff");
		if(step == 0) entries_0 = histogram_0.GetEntries();
		kineticEnergyOfStep_0[step] = histogram_0.GetMean();
		if(numberOfRuns > 1){
			TH1F histogram_1(nameOfRuns[1].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
			tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[1]).data(), runNumberCut[1]&&stepNumberCut[step]&&cut, "goff");
			if(step == 0) entries_1 = histogram_1.GetEntries();
			kineticEnergyOfStep_1[step] = histogram_1.GetMean();
			if(numberOfRuns > 2){
				TH1F histogram_2(nameOfRuns[2].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[2]).data(), runNumberCut[2]&&stepNumberCut[step]&&cut, "goff");
				if(step == 0) entries_2 = histogram_2.GetEntries();
				kineticEnergyOfStep_2[step] = histogram_2.GetMean();
			}
		}
		particleStepNumber[step] = step;
	}

	TGraph* graphParticleKineticEnergyDuringStep_0 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_0);
	TMultiGraph* multigraphOfParticleKineticEnergyDuringStep = new TMultiGraph("multigraphOfParticleKineticEnergyDuringStep", "");

	graphParticleKineticEnergyDuringStep_0->SetTitle(nameOfRuns[0].data());
	graphParticleKineticEnergyDuringStep_0->SetMarkerColor(2);
	graphParticleKineticEnergyDuringStep_0->SetMarkerStyle(24);
	graphParticleKineticEnergyDuringStep_0->SetMarkerSize(2);
	//graphParticleKineticEnergyDuringStep_0->GetYaxis()->SetTitleOffset(1.2);
	multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_0);

	if(numberOfRuns > 1){
		if(statsAreOnRightSide)
			c0Legend = new TLegend(.75, .75, .89, .89, nameOfVariableToCompare.data());
		else
			c0Legend = new TLegend(.11, .75, .25, .89, nameOfVariableToCompare.data());
		c0Legend->SetMargin(0.5);
		c0Legend->SetTextSize(0.03);
		c0Legend->SetBorderSize(0);
		c0Legend->SetFillColor(0);
		TGraph* graphParticleKineticEnergyDuringStep_1 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_1);
		graphParticleKineticEnergyDuringStep_1->SetTitle(nameOfRuns[1].data());
		graphParticleKineticEnergyDuringStep_1->SetMarkerColor(3);
		graphParticleKineticEnergyDuringStep_1->SetMarkerStyle(25);
		graphParticleKineticEnergyDuringStep_1->SetMarkerSize(2);
		//graphParticleKineticEnergyDuringStep_1->GetYaxis()->SetTitleOffset(1.2);
		multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_1);
		c0Legend->AddEntry(graphParticleKineticEnergyDuringStep_0, nameOfRuns[0].data(), "p");
		c0Legend->AddEntry(graphParticleKineticEnergyDuringStep_1, nameOfRuns[1].data(), "p");

		if(numberOfRuns > 2){
			TGraph* graphParticleKineticEnergyDuringStep_2 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_2);
			graphParticleKineticEnergyDuringStep_2->SetTitle(nameOfRuns[2].data());
			graphParticleKineticEnergyDuringStep_2->SetMarkerColor(4);
			graphParticleKineticEnergyDuringStep_2->SetMarkerStyle(26);
			graphParticleKineticEnergyDuringStep_2->SetMarkerSize(2);
			//graphParticleKineticEnergyDuringStep_2->GetYaxis()->SetTitleOffset(1.2);
			multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_2);
			c0Legend->AddEntry(graphParticleKineticEnergyDuringStep_2, nameOfRuns[2].data(), "p");
		}
	}

	multigraphOfParticleKineticEnergyDuringStep->SetMaximum(maxHeight);
	multigraphOfParticleKineticEnergyDuringStep->SetMinimum(minHeight);
	multigraphOfParticleKineticEnergyDuringStep->SetTitle((titleOfFigure+";"+titleOfXAxis+";"+titleOfYAxis).data());
	multigraphOfParticleKineticEnergyDuringStep->Draw("ap");
	if(numberOfRuns > 1) c0Legend->Draw();

	c0->SaveAs((titleOfFigure+".pdf").data());
	delete c0;
}

////////////////////
// Draw Histogram //
////////////////////

void DrawHistogram(string titleOfXAxis, string titleOfYAxis, string nameOfVariableToPlot, int numberOfRuns, string nameOfVariableToCompare, string* nameOfRuns, TTree& tree, TCut& cut, TCut runNumberCut[], string titleOfFigure, int numberOfBins, double leftEdge, double rightEdge, double minHeight, double maxHeight, bool statsAreOnRightSide){
	string drawingOption;
	if(statsAreOnRightSide) drawingOption = "";
	else drawingOption = "y+";

	//cout << "Number of runs: " << numberOfRuns << endl;
	TCanvas* c0 = new TCanvas("c0", "Canvas 0");
	c0->cd();
	//c0->SetLogy();

	TH1F *histogram_0, *histogram_1, *histogram_2, *histogram_3, *histogram_4, *histogram_5, *histogram_6, *histogram_7;
	histogram_0	= new TH1F(nameOfRuns[0].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
	//histogram_0->SetMaximum(maxHeight);
	histogram_0->SetYTitle(titleOfYAxis.data());
	histogram_0->SetXTitle(titleOfXAxis.data());
	histogram_0->SetLineStyle(1);
	histogram_0->SetLineWidth(3);
	histogram_0->SetLineColor(2);
	tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[0]).data(), runNumberCut[0]&&cut, drawingOption.data());
	gPad->Update();
	TPaveStats* histogram_0Stat = (TPaveStats*)histogram_0->FindObject("stats");
	histogram_0Stat->SetX1NDC(.10+(statsAreOnRightSide)*.65);
	histogram_0Stat->SetY1NDC(.75);
	histogram_0Stat->SetX2NDC(.25+(statsAreOnRightSide)*.65);
	histogram_0Stat->SetY2NDC(.90);

	if(numberOfRuns >= 2){
		TLegend* c0Legend;
		if(statsAreOnRightSide)
			c0Legend = new TLegend(.75, .60, .89, .74, nameOfVariableToCompare.data());
		else
			c0Legend = new TLegend(.11, .60, .25, .74, nameOfVariableToCompare.data());
		c0Legend->SetMargin(0.5);
		c0Legend->SetTextSize(0.03);
		c0Legend->SetBorderSize(0);
		c0Legend->SetFillColor(0);
		histogram_1 = new TH1F(nameOfRuns[1].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
		histogram_1->SetLineStyle(2);
		histogram_1->SetLineWidth(3);
		histogram_1->SetLineColor(2);
		tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[1]).data(), runNumberCut[1]&&cut, "sames");
		gPad->Update();
		TPaveStats* histogram_1Stat = (TPaveStats*)histogram_1->FindObject("stats");
		histogram_1Stat->SetX1NDC(.25+(statsAreOnRightSide)*.35);
		histogram_1Stat->SetY1NDC(.75);
		histogram_1Stat->SetX2NDC(.40+(statsAreOnRightSide)*.35);
		histogram_1Stat->SetY2NDC(.90);
		c0Legend->AddEntry(histogram_0, nameOfRuns[0].data(), "l");
		c0Legend->AddEntry(histogram_1, nameOfRuns[1].data(), "l");

		if(numberOfRuns >= 3){
			histogram_2
				= new TH1F(nameOfRuns[2].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
			histogram_2->SetLineStyle(1);
			histogram_2->SetLineWidth(3);
			histogram_2->SetLineColor(6);
			tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[2]).data(), runNumberCut[2]&&cut, "sames");
			gPad->Update();
			TPaveStats* histogram_2Stat = (TPaveStats*)histogram_2->FindObject("stats");
			histogram_2Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
			histogram_2Stat->SetY1NDC(.75);
			histogram_2Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
			histogram_2Stat->SetY2NDC(.90);
			c0Legend->AddEntry(histogram_2, nameOfRuns[2].data(), "l");
			if(numberOfRuns == 8){
				histogram_3
					= new TH1F(nameOfRuns[3].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				histogram_3->SetLineStyle(2);
				histogram_3->SetLineWidth(3);
				histogram_3->SetLineColor(6);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[3]).data(), runNumberCut[2]&&cut, "sames");
				gPad->Update();
				TPaveStats* histogram_3Stat = (TPaveStats*)histogram_3->FindObject("stats");
				histogram_3Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
				histogram_3Stat->SetY1NDC(.75);
				histogram_3Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
				histogram_3Stat->SetY2NDC(.90);
				c0Legend->AddEntry(histogram_3, nameOfRuns[3].data(), "l");
				
				histogram_4
					= new TH1F(nameOfRuns[4].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				histogram_4->SetLineStyle(1);
				histogram_4->SetLineWidth(3);
				histogram_4->SetLineColor(4);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[4]).data(), runNumberCut[2]&&cut, "sames");
				gPad->Update();
				TPaveStats* histogram_4Stat = (TPaveStats*)histogram_4->FindObject("stats");
				histogram_4Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
				histogram_4Stat->SetY1NDC(.75);
				histogram_4Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
				histogram_4Stat->SetY2NDC(.90);
				c0Legend->AddEntry(histogram_4, nameOfRuns[4].data(), "l");

				histogram_5
					= new TH1F(nameOfRuns[5].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				histogram_5->SetLineStyle(2);
				histogram_5->SetLineWidth(3);
				histogram_5->SetLineColor(4);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[5]).data(), runNumberCut[2]&&cut, "sames");
				gPad->Update();
				TPaveStats* histogram_5Stat = (TPaveStats*)histogram_5->FindObject("stats");
				histogram_5Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
				histogram_5Stat->SetY1NDC(.75);
				histogram_5Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
				histogram_5Stat->SetY2NDC(.90);
				c0Legend->AddEntry(histogram_5, nameOfRuns[5].data(), "l");

				histogram_6
					= new TH1F(nameOfRuns[6].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				histogram_6->SetLineStyle(1);
				histogram_6->SetLineWidth(3);
				histogram_6->SetLineColor(7);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[6]).data(), runNumberCut[2]&&cut, "sames");
				gPad->Update();
				TPaveStats* histogram_6Stat = (TPaveStats*)histogram_6->FindObject("stats");
				histogram_6Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
				histogram_6Stat->SetY1NDC(.75);
				histogram_6Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
				histogram_6Stat->SetY2NDC(.90);
				c0Legend->AddEntry(histogram_6, nameOfRuns[6].data(), "l");

				histogram_7
					= new TH1F(nameOfRuns[7].data(), titleOfFigure.data(), numberOfBins, leftEdge, rightEdge);
				histogram_7->SetLineStyle(2);
				histogram_7->SetLineWidth(3);
				histogram_7->SetLineColor(7);
				tree.Draw((nameOfVariableToPlot+">>"+nameOfRuns[7]).data(), runNumberCut[2]&&cut, "sames");
				gPad->Update();
				TPaveStats* histogram_7Stat = (TPaveStats*)histogram_7->FindObject("stats");
				histogram_7Stat->SetX1NDC(.40+(statsAreOnRightSide)*.05);
				histogram_7Stat->SetY1NDC(.75);
				histogram_7Stat->SetX2NDC(.55+(statsAreOnRightSide)*.05);
				histogram_7Stat->SetY2NDC(.90);
				c0Legend->AddEntry(histogram_7, nameOfRuns[7].data(), "l");
			}
		}
	c0Legend->Draw();
	}

	c0->SaveAs((titleOfFigure+".pdf").data());
	//delete c0Legend;
	delete histogram_0;
	if(numberOfRuns >= 2){
		delete histogram_1;
		if(numberOfRuns >= 3){
			delete histogram_2;
			if(numberOfRuns == 8){
				delete histogram_3;
				delete histogram_4;
				delete histogram_5;
				delete histogram_6;
				delete histogram_7;
		}
	}
	delete c0;
	//delete histogram_0;
	//delete histogram_0Stat;
	//if(numberOfRuns > 1){
	//	delete histogram_1;
	//	delete histogram_1Stat;
	//	if(numberOfRuns > 2){
	//		delete histogram_2;
	//		delete histogram_2Stat;
	//	}
	//}
}

int main(int argc, char** argv){
	cout
		<< endl
		<< endl
		<< "####################### Running program. Please wait... ########################"
		<< endl;
	if(argc < 2 || argc > 4){
		cout << "Correct number of arguments were not specified.\n";
		cout << "Please specify 1 to 3 files that you would like to plot after the executable\n";
		cout << "\n\n";
		return 0;
	}
	// Initialize Root Environment //
	gROOT->Reset();

	// Various Visual Options for Graphing //
	gROOT->SetStyle("Plain");
	gStyle->SetTitleBorderSize(0.);
	gStyle->SetTitleX(0.5);
	gStyle->SetTitleAlign(23);
	gStyle->SetTitleW(.8);
	gStyle->SetTitleH(.09);
	gStyle->SetCanvasColor(0);
	gStyle->SetOptDate(21);
	gStyle->SetTitleYOffset(1.3);
	//	gStyle->SetFrameBorderSize(10);
	//	gStyle->SetFrameBorderMode(1);
	//	gStyle->SetDrawBorder(1);
	//	gStyle->SetPadBorderSize(5);
	// Define histogramOfs and graphs here //
	//TFile *f = new TFile("basic.root","RECREATE");
	//TFile f("myrootfile.root");
	int numberOfRuns = argc - 1;
	string nameOfVariableToCompare = "Doping";
	string nameOfRuns[numberOfRuns];// Name for different runs.
	//string* nameOfRuns;// Name for different runs.
	//nameOfRuns = (string*) calloc(numberOfRuns, sizeof(string));
	if(numberOfRuns == 1){
		nameOfRuns[0] = "0.01";
	}else if(numberOfRuns == 2){
		nameOfRuns[0] = "B10";
		nameOfRuns[1] = "Li6";
	}else if(numberOfRuns == 3){
		nameOfRuns[0] = "0.05";
		nameOfRuns[1] = "0.025";
		nameOfRuns[2] = "0.01";
	}
	else if(numberOfRuns == 8){
		nameOfRuns[0] = "5% enriched B10";
		nameOfRuns[1] = "1% enriched B10";
		nameOfRuns[2] = "5% natural B10";
		nameOfRuns[3] = "1% natural B10";
		nameOfRuns[4] = "5% enriched Li6";
		nameOfRuns[5] = "1% enriched Li6";
		nameOfRuns[6] = "5% natural Li6";
		nameOfRuns[7] = "1% natural Li6";
	}

	TCut killTrackStatusCut = "trackStatus == 2";
	TCut neutronCut = "particleID == 2112";
	TCut positronCut = "particleID == -11";
	TCut primaryPositronStoppedInScintillatorCut = "primaryPositronStoppedInScintillatorFlag == 1";
	TCut primaryNeutronStoppedInScintillatorCut = "primaryNeutronStoppedInScintillatorFlag == 1";
	TCut primaryParticleCut = "parentID == 0";
	TCut initialStepCut = "stepNumber == 1";
	TCut hadronicProcessCut = "processType == 4";
	TCut hadronInelasticProcessCut = "processSubType == 121";
	TCut hadronCaptureProcessCut = "processSubType == 131";
	TCut runNumberCut[3];
	runNumberCut[0] = "runNumber == 0";
	runNumberCut[1] = "runNumber == 1";
	runNumberCut[2] = "runNumber == 2";

	// Declaration of Common Variables //
	ifstream fin;
	int runNumber;
	int eventNumber;
	double neutrinoKineticEnergy;
	double neutrinoMomentumUnitVectorX;
	double neutrinoMomentumUnitVectorY;
	double neutrinoMomentumUnitVectorZ;
	int stepNumber;
	int particleID;
	int trackID;
	int parentID;
	double preStepPositionX;
	double preStepPositionY;
	double preStepPositionZ;
	double preStepGlobalTime;
	double postStepPositionX;
	double postStepPositionY;
	double postStepPositionZ;
	double postStepGlobalTime;
	double primaryPositronInitialVertexPositionX;
	double primaryPositronInitialVertexPositionY;
	double primaryPositronInitialVertexPositionZ;
	double primaryPositronPostStepDisplacementX;
	double primaryPositronPostStepDisplacementY;
	double primaryPositronPostStepDisplacementZ;
	double primaryPositronPostStepDisplacementMagnitude;
	double primaryPositronPostStepCosineOfDisplacementNormalAngle;
	double primaryNeutronInitialVertexPositionX;
	double primaryNeutronInitialVertexPositionY;
	double primaryNeutronInitialVertexPositionZ;
	double primaryNeutronPostStepDisplacementX;
	double primaryNeutronPostStepDisplacementY;
	double primaryNeutronPostStepDisplacementZ;
	double primaryNeutronPostStepDisplacementMagnitude;
	double primaryNeutronPostStepCosineOfDisplacementNormalAngle;
	double preStepKineticEnergy;
	double postStepKineticEnergy;
	double totalEnergyDeposit;
	double stepLength;
	double trackLength;
	int processType;
	int processSubType;
	int trackStatus;
	string boundaryStatusName;
	string isBetweenScintillatorAndSensitivePixel;
	Bool_t primaryPositronStoppedInScintillatorFlag;
	Bool_t primaryNeutronStoppedInScintillatorFlag;
	double primaryPositronStoppingTime;
	double primaryNeutronStoppingTime;
	double primaryPositronCosineOfInitialNormalAngle;
	double primaryNeutronCosineOfInitialNormalAngle;
	Bool_t particlePostStepPointInScintillatorFlag;


	//TTree* tree = new TTree("tree", "data");
	TTree tree("tree", "data");
	tree.Branch("run number", &runNumber, "runNumber/I");
	tree.Branch("event number", &eventNumber, "eventNumber/I");
	tree.Branch("neutrino kinetic energy", &neutrinoKineticEnergy, "neutrinoKineticEnergy/D");
	tree.Branch("neutrino momentum unit vector x", &neutrinoMomentumUnitVectorX, "neutrinoMomentumUnitVectorX/D");
	tree.Branch("neutrino momentum unit vector y", &neutrinoMomentumUnitVectorY, "neutrinoMomentumUnitVectorY/D");
	tree.Branch("neutrino momentum unit vector z", &neutrinoMomentumUnitVectorZ, "neutrinoMomentumUnitVectorZ/D");
	tree.Branch("step number", &stepNumber, "stepNumber/I");
	tree.Branch("particle ID", &particleID, "particleID/I");
	tree.Branch("track ID", &trackID, "trackID/I");
	tree.Branch("parent ID", &parentID, "parentID/I");
	tree.Branch("pre-step position x", &preStepPositionX, "preStepPositionX/D");
	tree.Branch("pre-step position y", &preStepPositionY, "preStepPositionY/D");
	tree.Branch("pre-step position z", &preStepPositionZ, "preStepPositionZ/D");
	tree.Branch("post-step position x", &postStepPositionX, "postStepPositionX/D");
	tree.Branch("post-step position y", &postStepPositionY, "postStepPositionY/D");
	tree.Branch("post-step position z", &postStepPositionY, "postStepPositionZ/D");
	tree.Branch("pre-step position z", &preStepGlobalTime, "preStepPositionZ/D");
	tree.Branch("post-step global time", &postStepGlobalTime, "postStepGlobalTime/D");
	tree.Branch("primary positron initial vertex position x", &primaryPositronInitialVertexPositionX, "primaryPositronInitialVertexPositionX/D");
	tree.Branch("primary positron initial vertex position y", &primaryPositronInitialVertexPositionY, "primaryPositronInitialVertexPositionY/D");
	tree.Branch("primary positron initial vertex position z", &primaryPositronInitialVertexPositionZ, "primaryPositronInitialVertexPositionZ/D");
	tree.Branch("primary positron post step displacement x", &primaryPositronPostStepDisplacementX, "primaryPositronPostStepDisplacementX/D");
	tree.Branch("primary positron post step displacement y", &primaryPositronPostStepDisplacementY, "primaryPositronPostStepDisplacementY/D");
	tree.Branch("primary positron post step displacement z", &primaryPositronPostStepDisplacementZ, "primaryPositronPostStepDisplacementZ/D");
	tree.Branch("primary positron post step displacement magnitude", &primaryPositronPostStepDisplacementMagnitude, "primaryPositronPostStepDisplacementMagnitude/D");
	tree.Branch("primary positron post step cosine of displacement normal angle", &primaryPositronPostStepCosineOfDisplacementNormalAngle, "primaryPositronPostStepCosineOfDisplacementNormalAngle/D");
	tree.Branch("primary neutron initial vertex position x", &primaryNeutronInitialVertexPositionX, "primaryNeutronInitialVertexPositionX/D");
	tree.Branch("primary neutron initial vertex position y", &primaryNeutronInitialVertexPositionY, "primaryNeutronInitialVertexPositionY/D");
	tree.Branch("primary neutron initial vertex position z", &primaryNeutronInitialVertexPositionZ, "primaryNeutronInitialVertexPositionZ/D");
	tree.Branch("primary neutron post step displacement x", &primaryNeutronPostStepDisplacementX, "primaryNeutronPostStepDisplacementX/D");
	tree.Branch("primary neutron post step displacement y", &primaryNeutronPostStepDisplacementY, "primaryNeutronPostStepDisplacementY/D");
	tree.Branch("primary neutron post step displacement z", &primaryNeutronPostStepDisplacementZ, "primaryNeutronPostStepDisplacementZ/D");
	tree.Branch("primary neutron post step displacement magnitude", &primaryNeutronPostStepDisplacementMagnitude, "primaryNeutronPostStepDisplacementMagnitude/D");
	tree.Branch("primary neutron post step cosine of displacement normal angle", &primaryNeutronPostStepCosineOfDisplacementNormalAngle, "primaryNeutronPostStepCosineOfDisplacementNormalAngle/D");
	tree.Branch("pre-step kinetic energy", &preStepKineticEnergy, "preStepKineticEnergy/D");
	tree.Branch("post-step kinetic energy", &postStepKineticEnergy, "postStepKineticEnergy/D");
	tree.Branch("total energy deposit", &totalEnergyDeposit, "totalEnergyDeposit/D");
	tree.Branch("step length", &stepLength, "stepLength/D");
	tree.Branch("track length", &trackLength, "trackLength/D");
	tree.Branch("physics process type", &processType, "processType/I");
	tree.Branch("physics process subtype", &processSubType, "processSubType/I");
	tree.Branch("track status", &trackStatus, "trackStatus/I");
	tree.Branch("name of boundary status", &boundaryStatusName);
	tree.Branch("is particle between scintillator and pixel?", &isBetweenScintillatorAndSensitivePixel);
	tree.Branch("particle post-step point in scintillator", &particlePostStepPointInScintillatorFlag, "particlePostStepPointInScintillatorFlag/b");
	tree.Branch("primary positron stopped in scintillator flag", &primaryPositronStoppedInScintillatorFlag, "primaryPositronStoppedInScintillatorFlag/b");
	tree.Branch("primary positron stopping time", &primaryPositronStoppingTime, "primaryPositronStoppingTime/D");
	tree.Branch("primary positron cosine of initial normal angle", &primaryPositronCosineOfInitialNormalAngle, "primaryPositronCosineOfInitialNormalAngle/D");
	tree.Branch("primary neutron stopped in scintillator flag", &primaryNeutronStoppedInScintillatorFlag, "primaryNeutronStoppedInScintillatorFlag/b");
	tree.Branch("primary neutron stopping time", &primaryNeutronStoppingTime, "primaryNeutronStoppingTime/D");
	tree.Branch("primary neutron cosine of initial normal angle", &primaryNeutronCosineOfInitialNormalAngle, "primaryNeutronCosineOfInitialNormalAngle/D");

	string finString;
	TVector3 displacementVector;
	TVector3 neutrinoMomentumUnitVector;
	runNumber = 0;
	for(int fileNumber = 0; fileNumber < numberOfRuns; fileNumber++){
				primaryPositronInitialVertexPositionX = 0.;
				primaryPositronInitialVertexPositionY = 0.;
				primaryPositronInitialVertexPositionZ = 0.;
				primaryPositronPostStepDisplacementX = 0.;
				primaryPositronPostStepDisplacementY = 0.;
				primaryPositronPostStepDisplacementZ = 0.;
				primaryPositronPostStepDisplacementMagnitude = 0.;
				primaryPositronPostStepCosineOfDisplacementNormalAngle = 0.;
				primaryNeutronInitialVertexPositionX = 0.;
				primaryNeutronInitialVertexPositionY = 0.;
				primaryNeutronInitialVertexPositionZ = 0.;
				primaryNeutronPostStepDisplacementX = 0.;
				primaryNeutronPostStepDisplacementY = 0.;
				primaryNeutronPostStepDisplacementZ = 0.;
				primaryNeutronPostStepDisplacementMagnitude = 0.;
				primaryNeutronPostStepCosineOfDisplacementNormalAngle = 0.;
//		cout << finString[fileNumber] << endl;
		finString.assign(argv[fileNumber+1]);
		fin.open(finString.data());
		int readInLines = 0;
		char bufferChar[2048];
		while(fin.getline(bufferChar, 2048)){
			string bufferString;
			bufferString.assign(bufferChar);
			// If first entry is not a digit.
			if(!isdigit(bufferString[bufferString.find_first_not_of("		")])){
				continue;
			}
			istringstream inputStream(bufferString, istringstream::in);
			if(!(
						inputStream
						>> eventNumber
						>> neutrinoKineticEnergy
						>> neutrinoMomentumUnitVectorX
						>> neutrinoMomentumUnitVectorY
						>> neutrinoMomentumUnitVectorZ
						>> stepNumber
						>> particleID
						>> trackID
						>> parentID
						>> preStepPositionX
						>> preStepPositionY
						>> preStepPositionZ
						>> postStepPositionX
						>> postStepPositionY
						>> postStepPositionZ
						>> preStepGlobalTime
						>> postStepGlobalTime
						>> preStepKineticEnergy
						>> postStepKineticEnergy
						>> totalEnergyDeposit
						>> stepLength
						>> trackLength
						>> processType
						>> processSubType
						>> trackStatus
						>> boundaryStatusName
						>> isBetweenScintillatorAndSensitivePixel
						>> particlePostStepPointInScintillatorFlag
						>> primaryPositronStoppedInScintillatorFlag
						>> primaryNeutronStoppedInScintillatorFlag
						>> primaryPositronStoppingTime
						>> primaryNeutronStoppingTime
						>> primaryPositronCosineOfInitialNormalAngle
						>> primaryNeutronCosineOfInitialNormalAngle
			)){
				cout << finString
					<< " cannot be read in properly.\n"
					<< "Please check formatting.\n";
				return 0;
			}
			else{
				readInLines++;
				if(particleID == -11 && parentID == 0){
					if(stepNumber == 1){
						primaryPositronInitialVertexPositionX = preStepPositionX;
						primaryPositronInitialVertexPositionY = preStepPositionY;
						primaryPositronInitialVertexPositionZ = preStepPositionZ;
					}
					//cout << postStepPositionX << " " << postStepPositionY << " " << postStepPositionZ << " " << endl;
					//cout << primaryPositronInitialVertexPositionX << " " << primaryPositronInitialVertexPositionY << " " << primaryPositronInitialVertexPositionZ << endl;
					//				primaryPositronPostStepDisplacementX = postStepPositionX - primaryPositronInitialVertexPositionX;
					//				primaryPositronPostStepDisplacementY = postStepPositionY - primaryPositronInitialVertexPositionY;
					//				primaryPositronPostStepDisplacementZ = postStepPositionZ - primaryPositronInitialVertexPositionZ;
					displacementVector.SetXYZ(postStepPositionX - primaryPositronInitialVertexPositionX, postStepPositionY - primaryPositronInitialVertexPositionY, postStepPositionZ - primaryPositronInitialVertexPositionZ);
					primaryPositronPostStepDisplacementMagnitude = displacementVector.Mag();
					neutrinoMomentumUnitVector.SetXYZ(neutrinoMomentumUnitVectorX, neutrinoMomentumUnitVectorY, neutrinoMomentumUnitVectorZ);
					primaryPositronPostStepCosineOfDisplacementNormalAngle = displacementVector.Dot(neutrinoMomentumUnitVector)/(displacementVector.Mag()*neutrinoMomentumUnitVector.Mag());
					//			cout << primaryPositronPostStepDisplacementMagnitude << endl;
				}
				else if(particleID == 2112 && parentID == 0){
					if(stepNumber == 1){
						primaryNeutronInitialVertexPositionX = preStepPositionX;
						primaryNeutronInitialVertexPositionY = preStepPositionY;
						primaryNeutronInitialVertexPositionZ = preStepPositionZ;
					}
					//cout << postStepPositionX << " " << postStepPositionY << " " << postStepPositionZ << " " << endl;
					//cout << primaryPositronInitialVertexPositionX << " " << primaryPositronInitialVertexPositionY << " " << primaryPositronInitialVertexPositionZ << endl;
					//				primaryNeutronPostStepDisplacementX = postStepPositionX - primaryNeutronInitialVertexPositionX;
					//				primaryNeutronPostStepDisplacementY = postStepPositionY - primaryNeutronInitialVertexPositionY;
					//				primaryNeutronPostStepDisplacementZ = postStepPositionZ - primaryNeutronInitialVertexPositionZ;
					displacementVector.SetXYZ(postStepPositionX - primaryNeutronInitialVertexPositionX, postStepPositionY - primaryNeutronInitialVertexPositionY, postStepPositionZ - primaryNeutronInitialVertexPositionZ);
					primaryNeutronPostStepDisplacementMagnitude = displacementVector.Mag();
					neutrinoMomentumUnitVector.SetXYZ(neutrinoMomentumUnitVectorX, neutrinoMomentumUnitVectorY, neutrinoMomentumUnitVectorZ);
					primaryNeutronPostStepCosineOfDisplacementNormalAngle = displacementVector.Dot(neutrinoMomentumUnitVector)/(displacementVector.Mag()*neutrinoMomentumUnitVector.Mag());
				}
				//			if(runNumber == 0 && eventNumber == 1 && particleID == 2112 && trackStatus != 200)cout << primaryNeutronPostStepDisplacementX << " " << primaryNeutronPostStepDisplacementY << " " << primaryNeutronPostStepDisplacementZ << " " << primaryNeutronPostStepDisplacementMagnitude << endl;
				//			cout << primaryNeutronStoppingTime << endl;
				//			if(stepNumber == 1 && particleID == 2112 && parentID == 0) cout << preStepPositionX << " " << preStepPositionY << " " << preStepPositionZ << endl;
				tree.Fill();
			}
		}
		runNumber++;
		fin.close();
	cout << "Number of read-in lines: "
		<< readInLines
		<< endl;
	}
	TCut cut;
	TCut neutronCaptureCut
		= killTrackStatusCut&&neutronCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut;
	string titleOfXAxis, titleOfYAxis;
	string titleOfPlot;
	string nameOfVariableToPlot;
	DrawHistogram(
			titleOfXAxis = "distance [mm]",
			titleOfYAxis = "events",
			nameOfVariableToPlot = "primaryNeutronPostStepDisplacementMagnitude",
			numberOfRuns,
			nameOfVariableToCompare,
			nameOfRuns,
			tree,
			cut = neutronCaptureCut,
			runNumberCut,
			titleOfPlot = "Displacement of Neutron Capture from Inverse-Beta Decay in 5% Doped Scintillator",
			10,
			0.,
			150.,
			0.,
			20.,
			true);
	
	DrawHistogram(
			titleOfXAxis = "cosine(theta)",
			titleOfYAxis = "events",
			nameOfVariableToPlot = "primaryNeutronPostStepCosineOfDisplacementNormalAngle",
			numberOfRuns,
			nameOfVariableToCompare,
			nameOfRuns,
			tree,
			cut = neutronCaptureCut,
			runNumberCut,
			titleOfPlot = "Cosine of Normal Angle of Neutron Capture in 5% Doped Scintillator",
			10,
			-1.,
			+1.,
			0.,
			20.,
			false);
	/*
	//////////////////////////////////////////////////////////////////
	// Histogram of Cosine of Normal Angle of Particle Displacement //
	//////////////////////////////////////////////////////////////////

	TCanvas* c2 = new TCanvas("c2", "canvas2");
	c2->cd();
	string histogramOfCosineOfNormalAngleOfDisplacementTitle = "Cosine of Normal Angle of Inverse-Beta Decay Neutron Displacement in Boron Doped Scintillator";
	TLegend* c2Legend = new TLegend(.11, .60, .25, .74, nameOfVariableToCompare.data());
	c2Legend->SetMargin(0.5);
	c2Legend->SetTextSize(0.03);
	c2Legend->SetBorderSize(0);
	c2Legend->SetFillColor(0);

	TH1F* histogramOfCosineOfNormalAngleOfDisplacement_0
		= new TH1F("", "", 10, -1., 1.);
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle>>histogramOfCosineOfNormalAngleOfDisplacement_0(10, -1., 1.)", runNumberCut[0]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "y+");
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetMaximum(80.);
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetTitle(histogramOfCosineOfNormalAngleOfDisplacementTitle.data());
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetName(nameOfRuns[0].data());
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetYTitle("count");
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetXTitle("cos(#theta)");
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetLineStyle(1);
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetLineWidth(3);
	histogramOfCosineOfNormalAngleOfDisplacement_0->SetLineColor(2);
	gPad->Update();
	TPaveStats* histogramOfCosineOfNormalAngleOfDisplacement_0Stat = (TPaveStats*)histogramOfCosineOfNormalAngleOfDisplacement_0->FindObject("stats");
	histogramOfCosineOfNormalAngleOfDisplacement_0Stat->SetX1NDC(.10);
	histogramOfCosineOfNormalAngleOfDisplacement_0Stat->SetY1NDC(.75);
	histogramOfCosineOfNormalAngleOfDisplacement_0Stat->SetX2NDC(.25);
	histogramOfCosineOfNormalAngleOfDisplacement_0Stat->SetY2NDC(.90);
	c2Legend->AddEntry(histogramOfCosineOfNormalAngleOfDisplacement_0, nameOfRuns[0].data(), "l");

if(numberOfRuns > 1){
	TH1F* histogramOfCosineOfNormalAngleOfDisplacement_1
		= new TH1F("", "", 10, -1., 1.);
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle>>histogramOfCosineOfNormalAngleOfDisplacement_1(10, -1., 1.)", runNumberCut[1]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "sames");
	histogramOfCosineOfNormalAngleOfDisplacement_1->SetMaximum(80.);
	histogramOfCosineOfNormalAngleOfDisplacement_1->SetName(nameOfRuns[1].data());
	histogramOfCosineOfNormalAngleOfDisplacement_1->SetLineStyle(7);
	histogramOfCosineOfNormalAngleOfDisplacement_1->SetLineWidth(3);
	histogramOfCosineOfNormalAngleOfDisplacement_1->SetLineColor(3);
	gPad->Update();
	TPaveStats* histogramOfCosineOfNormalAngleOfDisplacement_1Stat = (TPaveStats*)histogramOfCosineOfNormalAngleOfDisplacement_1->FindObject("stats");
	histogramOfCosineOfNormalAngleOfDisplacement_1Stat->SetX1NDC(.25);
	histogramOfCosineOfNormalAngleOfDisplacement_1Stat->SetY1NDC(.75);
	histogramOfCosineOfNormalAngleOfDisplacement_1Stat->SetX2NDC(.40);
	histogramOfCosineOfNormalAngleOfDisplacement_1Stat->SetY2NDC(.90);
	c2Legend->AddEntry(histogramOfCosineOfNormalAngleOfDisplacement_1, nameOfRuns[1].data(), "l"); 
	
if(numberOfRuns > 2){
	TH1F* histogramOfCosineOfNormalAngleOfDisplacement_2
		= new TH1F("", "", 10, -1., 1.);
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle>>histogramOfCosineOfNormalAngleOfDisplacement_2(10, -1., 1.)", runNumberCut[2]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "sames"); 
	histogramOfCosineOfNormalAngleOfDisplacement_2->SetMaximum(80.);
	histogramOfCosineOfNormalAngleOfDisplacement_2->SetName(nameOfRuns[2].data()); 
	histogramOfCosineOfNormalAngleOfDisplacement_2->SetLineStyle(3); 
	histogramOfCosineOfNormalAngleOfDisplacement_2->SetLineWidth(3); 
	histogramOfCosineOfNormalAngleOfDisplacement_2->SetLineColor(4); 
	gPad->Update();
	TPaveStats* histogramOfCosineOfNormalAngleOfDisplacement_2Stat = (TPaveStats*)histogramOfCosineOfNormalAngleOfDisplacement_2->FindObject("stats");
	histogramOfCosineOfNormalAngleOfDisplacement_2Stat->SetX1NDC(.40); 
	histogramOfCosineOfNormalAngleOfDisplacement_2Stat->SetY1NDC(.75); 
	histogramOfCosineOfNormalAngleOfDisplacement_2Stat->SetX2NDC(.55); 
	histogramOfCosineOfNormalAngleOfDisplacement_2Stat->SetY2NDC(.90); 
	c2Legend->AddEntry(histogramOfCosineOfNormalAngleOfDisplacement_2, nameOfRuns[2].data(), "l"); 
}	
}	
	
	c2Legend->Draw();
	c2->SaveAs("primary_neutron_cosine_of_normal_angle_of_capture.pdf"); 
*/
	DrawHistogram(
			titleOfXAxis = "time [ns]",
			titleOfYAxis = "events",
			nameOfVariableToPlot = "primaryNeutronStoppingTime",
			numberOfRuns,
			nameOfVariableToCompare,
			nameOfRuns,
			tree,
			cut = neutronCaptureCut,
			runNumberCut,
			titleOfPlot = "Stopping Time of Neutron from Inverse-Beta Decay in 5% Doped Scintillator",
			10,
			0.,
			15000.,
			0.,
			20.,
			true);
/*

	////////////////////////////////////////////
	// Histogram of Stopping Time of Particle //
	////////////////////////////////////////////

	TCanvas* c3 = new TCanvas("c3", "canvas3");
	c3->cd();
	string histogramOfStoppingTimeTitle = "Stopping Time of Neutron from Inverse-Beta Decay in Boron Doped Scintillator";
//	c3->SetLogy();
	TLegend* c3Legend = new TLegend(.75, .60, .89, .74, nameOfVariableToCompare.data());
	c3Legend->SetMargin(0.5);
	c3Legend->SetTextSize(0.03);
	c3Legend->SetBorderSize(0);
	c3Legend->SetFillColor(0);

	TH1F* histogramOfStoppingTime_0
		= new TH1F("", "", 10, 0., 50000.);
	tree.Draw("postStepGlobalTime>>histogramOfStoppingTime_0(10, 0., 50000.)", runNumberCut[0]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "");
	histogramOfStoppingTime_0->SetMaximum(350.);
	histogramOfStoppingTime_0->SetTitle(histogramOfStoppingTimeTitle.data());
	histogramOfStoppingTime_0->SetName(nameOfRuns[0].data());
	histogramOfStoppingTime_0->SetYTitle("count");
	histogramOfStoppingTime_0->SetXTitle("time (ns)");
	histogramOfStoppingTime_0->SetLineStyle(1);
	histogramOfStoppingTime_0->SetLineWidth(3);
	histogramOfStoppingTime_0->SetLineColor(2);
	gPad->Update();
	TPaveStats* histogramOfStoppingTime_0Stat = (TPaveStats*)histogramOfStoppingTime_0->FindObject("stats");
	histogramOfStoppingTime_0Stat->SetX1NDC(.75);
	histogramOfStoppingTime_0Stat->SetY1NDC(.75);
	histogramOfStoppingTime_0Stat->SetX2NDC(.90);
	histogramOfStoppingTime_0Stat->SetY2NDC(.90);
	c3Legend->AddEntry(histogramOfStoppingTime_0, nameOfRuns[0].data(), "l");

if(numberOfRuns > 1){
	TH1F* histogramOfStoppingTime_1
		= new TH1F("", "", 10, 0., 50000.);
	tree.Draw("postStepGlobalTime>>histogramOfStoppingTime_1(10, 0., 50000.)", runNumberCut[1]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "sames");
	histogramOfStoppingTime_1->SetMaximum(350.);
	histogramOfStoppingTime_1->SetName(nameOfRuns[1].data());
	histogramOfStoppingTime_1->SetLineStyle(7);
	histogramOfStoppingTime_1->SetLineWidth(3);
	histogramOfStoppingTime_1->SetLineColor(3);
	gPad->Update();
	TPaveStats* histogramOfStoppingTime_1Stat = (TPaveStats*)histogramOfStoppingTime_1->FindObject("stats");
	histogramOfStoppingTime_1Stat->SetX1NDC(.60);
	histogramOfStoppingTime_1Stat->SetY1NDC(.75);
	histogramOfStoppingTime_1Stat->SetX2NDC(.75);
	histogramOfStoppingTime_1Stat->SetY2NDC(.90);
	c3Legend->AddEntry(histogramOfStoppingTime_1, nameOfRuns[1].data(), "l"); 
	
if(numberOfRuns > 2){
	TH1F* histogramOfStoppingTime_2
		= new TH1F("", "", 10, 0., 50000.);
	tree.Draw("postStepGlobalTime>>histogramOfStoppingTime_2(10, 0., 50000.)", runNumberCut[2]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "sames"); 
	histogramOfStoppingTime_2->SetMaximum(350.);
	histogramOfStoppingTime_2->SetName(nameOfRuns[2].data()); 
	histogramOfStoppingTime_2->SetLineStyle(3); 
	histogramOfStoppingTime_2->SetLineWidth(3); 
	histogramOfStoppingTime_2->SetLineColor(4); 
	gPad->Update();
	TPaveStats* histogramOfStoppingTime_2Stat = (TPaveStats*)histogramOfStoppingTime_2->FindObject("stats");
	histogramOfStoppingTime_2Stat->SetX1NDC(.45); 
	histogramOfStoppingTime_2Stat->SetY1NDC(.75); 
	histogramOfStoppingTime_2Stat->SetX2NDC(.60); 
	histogramOfStoppingTime_2Stat->SetY2NDC(.90); 
	c3Legend->AddEntry(histogramOfStoppingTime_2, nameOfRuns[2].data(), "l"); 
}	
}	
	
	c3Legend->Draw();
	c3->SaveAs("primary_neutron_stopping_time.pdf"); 

	///////////////////////////////////////////////////////////////////////////////////////////
	// 3 Dimentional Histograms of Inital Position or Initial Momentum Direction of Particle //
	///////////////////////////////////////////////////////////////////////////////////////////

	TCanvas* c4 = new TCanvas("c4", "canvas4", 600, 600);
	c4->cd();
	string histogramOfParticleInitialPositionTitle = "3D Histogram of Initial Position of Inverse-Beta Decay Neutron in Doped Scintillator";
	TLegend* c4Legend = new TLegend(.7, .8, .9, .9, nameOfVariableToCompare.data());
	c4Legend->SetMargin(0.1);
	c4Legend->SetTextSize(0.03);
	c4Legend->SetBorderSize(1);
	c4Legend->SetFillColor(0);
	
	TH3F* histogramOfParticleInitialPosition_0
		= new TH3F("", "", 10, -65, 65, 10, -65, 65, 10, -65, 65);
	tree.Draw("preStepPositionX:preStepPositionY:preStepPositionZ>>histogramOfParticleInitialPosition_0(10, -65, 65, 10, -65, 65, 10, -65, 65)", runNumberCut[0]&&initialStepCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
	histogramOfParticleInitialPosition_0->SetTitle(histogramOfParticleInitialPositionTitle.data());
	histogramOfParticleInitialPosition_0->SetName(nameOfRuns[0].data());
	histogramOfParticleInitialPosition_0->GetXaxis()->SetTitleOffset(1.5);
	histogramOfParticleInitialPosition_0->GetYaxis()->SetTitleOffset(1.5);
	histogramOfParticleInitialPosition_0->GetZaxis()->SetTitleOffset(1.5);
	histogramOfParticleInitialPosition_0->SetXTitle("X [mm]");
	histogramOfParticleInitialPosition_0->SetYTitle("Y [mm]");
	histogramOfParticleInitialPosition_0->SetZTitle("Z [mm]");
	histogramOfParticleInitialPosition_0->SetFillColor(2);
	histogramOfParticleInitialPosition_0->SetMarkerColor(2);
	histogramOfParticleInitialPosition_0->Draw("");
	gPad->Update();
	TPaveStats* histogramOfParticleInitialPosition_0Stat = (TPaveStats*)histogramOfParticleInitialPosition_0->FindObject("stats");
	histogramOfParticleInitialPosition_0Stat->SetX1NDC(.10);
	histogramOfParticleInitialPosition_0Stat->SetY1NDC(.75);
	histogramOfParticleInitialPosition_0Stat->SetX2NDC(.25);
	histogramOfParticleInitialPosition_0Stat->SetY2NDC(.90);
	c4Legend->AddEntry(histogramOfParticleInitialPosition_0, nameOfRuns[0].data(), "f");

if(numberOfRuns > 1){
	TH3F* histogramOfParticleInitialPosition_1
		= new TH3F("", "", 10, -65, 65, 10, -65, 65, 10, -65, 65);
	tree.Draw("preStepPositionX:preStepPositionY:preStepPositionZ>>histogramOfParticleInitialPosition_1(10, -65, 65, 10, -65, 65, 10, -65, 65)", runNumberCut[1]&&initialStepCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
	histogramOfParticleInitialPosition_1->SetName(nameOfRuns[1].data());
	histogramOfParticleInitialPosition_1->SetFillColor(3);
	histogramOfParticleInitialPosition_1->SetMarkerColor(3);
	histogramOfParticleInitialPosition_1->Draw("sames");
	gPad->Update();
//	TPaveStats* histogramOfParticleInitialPosition_1Stat = (TPaveStats*)histogramOfParticleInitialPosition_1->FindObject("stats");
//	histogramOfParticleInitialPosition_1Stat->SetX1NDC(.25);
//	histogramOfParticleInitialPosition_1Stat->SetY1NDC(.75);
//	histogramOfParticleInitialPosition_1Stat->SetX2NDC(.40);
//	histogramOfParticleInitialPosition_1Stat->SetY2NDC(.90);
	c4Legend->AddEntry(histogramOfParticleInitialPosition_1, nameOfRuns[1].data(), "f");

if(numberOfRuns > 2){
	TH3F* histogramOfParticleInitialPosition_2
		= TH3F("", "", 10, -65, 65, 10, -65, 65, 10, -65, 65);
	tree.Draw("preStepPositionX:preStepPositionY:preStepPositionZ>>histogramOfParticleInitialPosition_2(10, -65, 65, 10, -65, 65, 10, -65, 65)", runNumberCut[2]&&initialStepCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
	histogramOfParticleInitialPosition_2->SetName(nameOfRuns[2].data());
	histogramOfParticleInitialPosition_2->SetFillColor(4);
	histogramOfParticleInitialPosition_2->SetMarkerColor(4);
	histogramOfParticleInitialPosition_2->Draw("sames");
	gPad->Update();
//	TPaveStats* histogramOfParticleInitialPosition_2Stat = (TPaveStats*)histogramOfParticleInitialPosition_2->FindObject("stats");
//	histogramOfParticleInitialPosition_2Stat->SetX1NDC(.40);
//	histogramOfParticleInitialPosition_2Stat->SetY1NDC(.75);
//	histogramOfParticleInitialPosition_2Stat->SetX2NDC(.65);
//	histogramOfParticleInitialPosition_2Stat->SetY2NDC(.90);
	c4Legend->AddEntry(histogramOfParticleInitialPosition_2, nameOfRuns[2].data(), "f");
}
}

	c4Legend->Draw();
	c4->SaveAs("primary_neutron_histogram_of_initial_position.pdf");

	*/
	//TCut primaryNeutronCut = neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut;
	//DrawPlotOfAverageValueOfStep(
	//		titleOfXAxis = "Step Number",
	//		titleOfYAxis = "Kinetic Energy [MeV]",
	//		nameOfVariableToPlot = "preStepKineticEnergy", // This variable will be averated and plotted for each step.
	//		numberOfRuns,
	//		nameOfVariableToCompare,
	//		nameOfRuns,
	//		tree,
	//		primaryNeutronCut,
	//		runNumberCut,
	//		titleOfPlot = "Average Kinetic Energy During Step of Neutron from Inverse-Beta Decay in Doped Scintillator",
	//		10,
	//		0.,
	//		40.,
	//		1.e-10,
	//		1.e-1,
	//		true);
	/*
	/////////////////////////////////////////////////////////
	// Plot of Kinetic Energy During Each Step of Particle //
	/////////////////////////////////////////////////////////

	TCanvas* c6 = new TCanvas("c6", "canvas6");
	c6->cd();
	c6->SetLogy();
	TLegend* c6Legend = new TLegend(0.69, 0.74, 0.89, 0.89, nameOfVariableToCompare.data());
	c6Legend->SetMargin(0.5);
	c6Legend->SetTextSize(0.03);
	c6Legend->SetBorderSize(0);
	c6Legend->SetFillColor(0);
	double particleStepNumber[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_0[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_1[MAX_NEUTRON_STEP] = {0.};
	double kineticEnergyOfStep_2[MAX_NEUTRON_STEP] = {0.};
	
	for(int step = 0; step < MAX_NEUTRON_STEP; step++){
		TH1F* histogramOfKineticEnergy_0;
		TH1F* histogramOfKineticEnergy_1;
		TH1F* histogramOfKineticEnergy_2;
		tree.Draw("preStepKineticEnergy>>histogramOfKineticEnergy_0", runNumberCut[0]&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut&&stepNumberCut[step], "goff");
		tree.Draw("preStepKineticEnergy>>histogramOfKineticEnergy_1", runNumberCut[1]&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut&&stepNumberCut[step], "goff");
		tree.Draw("preStepKineticEnergy>>histogramOfKineticEnergy_2", runNumberCut[2]&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut&&stepNumberCut[step], "goff");
		kineticEnergyOfStep_0[step] = histogramOfKineticEnergy_0->GetMean();
		kineticEnergyOfStep_1[step] = histogramOfKineticEnergy_1->GetMean();
		kineticEnergyOfStep_2[step] = histogramOfKineticEnergy_2->GetMean();
		particleStepNumber[step] = step;
	}

	TGraph* graphParticleKineticEnergyDuringStep_0 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_0);
	TMultiGraph* multigraphOfParticleKineticEnergyDuringStep = new TMultiGraph("multigraphOfParticleKineticEnergyDuringStep", "");

	graphParticleKineticEnergyDuringStep_0->SetTitle(nameOfRuns[0].data());
	graphParticleKineticEnergyDuringStep_0->SetMarkerColor(2);
	graphParticleKineticEnergyDuringStep_0->SetMarkerStyle(24);
	graphParticleKineticEnergyDuringStep_0->SetMarkerSize(2);
	graphParticleKineticEnergyDuringStep_0->GetYaxis()->SetTitleOffset(1.2);
	multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_0);
	c6Legend->AddEntry(graphParticleKineticEnergyDuringStep_0, nameOfRuns[0].data(), "p");

if(numberOfRuns > 1){
	TGraph* graphParticleKineticEnergyDuringStep_1 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_1);
	graphParticleKineticEnergyDuringStep_1->SetTitle(nameOfRuns[1].data());
	graphParticleKineticEnergyDuringStep_1->SetMarkerColor(3);
	graphParticleKineticEnergyDuringStep_1->SetMarkerStyle(25);
	graphParticleKineticEnergyDuringStep_1->SetMarkerSize(2);
	graphParticleKineticEnergyDuringStep_1->GetYaxis()->SetTitleOffset(1.2);
	multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_1);
	c6Legend->AddEntry(graphParticleKineticEnergyDuringStep_1, nameOfRuns[1].data(), "p");

if(numberOfRuns > 2){
	TGraph* graphParticleKineticEnergyDuringStep_2 = new TGraph(MAX_NEUTRON_STEP, particleStepNumber, kineticEnergyOfStep_2);
	graphParticleKineticEnergyDuringStep_2->SetTitle(nameOfRuns[2].data());
	graphParticleKineticEnergyDuringStep_2->SetMarkerColor(4);
	graphParticleKineticEnergyDuringStep_2->SetMarkerStyle(26);
	graphParticleKineticEnergyDuringStep_2->SetMarkerSize(2);
	graphParticleKineticEnergyDuringStep_2->GetYaxis()->SetTitleOffset(1.2);
	multigraphOfParticleKineticEnergyDuringStep->Add(graphParticleKineticEnergyDuringStep_2);
	c6Legend->AddEntry(graphParticleKineticEnergyDuringStep_2, nameOfRuns[2].data(), "p");
}
}

	multigraphOfParticleKineticEnergyDuringStep->SetMaximum(1.e-1);
	multigraphOfParticleKineticEnergyDuringStep->SetMinimum(1.e-10);
	multigraphOfParticleKineticEnergyDuringStep->SetTitle("Average Kinetic Energy During Step of Neutron from Inverse-Beta Decay in Doped Scintillator;Step Number;Kinetic Energy [MeV]");
	multigraphOfParticleKineticEnergyDuringStep->Draw("ap");

	c6Legend->Draw();
	c6->SaveAs("plot_of_kinetic_energy_during_step_wrt_particle_kinetic_energy.pdf");

	//////////////////////////////////////////////////////////////////
	// Histogram of the Count of Each Nuclei that Captures Particle //
	//////////////////////////////////////////////////////////////////

	int totalNumberOfEvents = 0;
	int boron10Captures = 0;
	int protonCaptures = 0;
	int otherCaptures = 0;

	TCanvas* c7 = new TCanvas("c7", "canvas7");
	c7->cd();
	c7->SetGrid();
	c7->SetLogy();
	string histogramOfCaptureNucleiCountTitle = "Number of Captures per Capture Nucleus for Neutron from Inverse-Beta Decay In Doped Scintillator";

	TH1F* histogramOfTotalNumberOfEvents = new TH1F("Total Number of Events", "Number of Captures per Capture Nucleus for Neutron from Inverse-Beta Decay Inn Doped Scintillator", numberOfRuns, 0, numberOfRuns);
	TH1F* histogramOfParticleCaptureOnCaptureNucleus = new TH1F("Capture Nucleus", "", numberOfRuns, 0, numberOfRuns);
	TH1F* histogramOfParticleCaptureOnProton = new TH1F("Proton", "", numberOfRuns, 0, numberOfRuns);
	TH1F* histogramOfParticleCaptureOnOther = new TH1F("Other", "", numberOfRuns, 0, numberOfRuns);

	for(int run = 0; run < numberOfRuns; run++){
		TH2F* histogramOfCaptureNucleiCount;
		tree.Draw("processType:processSubType>>histogramOfCaptureNucleiCount", runNumberCut[run]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
		histogramOfTotalNumberOfEvents->Fill(nameOfRuns[run].data(), histogramOfCaptureNucleiCount->GetEntries());
		histogramOfCaptureNucleiCount->Reset();
		tree.Draw("processType:processSubType>>histogramOfCaptureNucleiCount", hadronicProcessCut&&hadronInelasticProcessCut&&runNumberCut[run]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
		histogramOfParticleCaptureOnCaptureNucleus->Fill(nameOfRuns[run].data(), histogramOfCaptureNucleiCount->GetEntries());
		histogramOfCaptureNucleiCount->Reset();
		tree.Draw("processType:processSubType>>histogramOfCaptureNucleiCount", hadronicProcessCut&&hadronCaptureProcessCut&&runNumberCut[run]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
		histogramOfParticleCaptureOnProton->Fill(nameOfRuns[run].data(), histogramOfCaptureNucleiCount->GetEntries());
		histogramOfCaptureNucleiCount->Reset();
		tree.Draw("processType:processSubType>>histogramOfCaptureNucleiCount", hadronicProcessCut&&!hadronCaptureProcessCut&&!hadronInelasticProcessCut&&runNumberCut[run]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
		histogramOfParticleCaptureOnOther->Fill(nameOfRuns[run].data(), histogramOfCaptureNucleiCount->GetEntries());
	}

	histogramOfTotalNumberOfEvents->SetFillColor(2);
	histogramOfTotalNumberOfEvents->SetBarWidth(0.2);
	histogramOfTotalNumberOfEvents->SetBarOffset(0.1);
	histogramOfTotalNumberOfEvents->SetStats(0);
	histogramOfTotalNumberOfEvents->SetMaximum(1.e4);
	histogramOfTotalNumberOfEvents->SetMinimum(1.e-1);
	histogramOfParticleCaptureOnCaptureNucleus->SetFillColor(4);
	histogramOfParticleCaptureOnCaptureNucleus->SetBarWidth(0.2);
	histogramOfParticleCaptureOnCaptureNucleus->SetBarOffset(0.3);
	histogramOfParticleCaptureOnCaptureNucleus->SetStats(0);
	histogramOfParticleCaptureOnProton->SetFillColor(38);
	histogramOfParticleCaptureOnProton->SetBarWidth(0.2);
	histogramOfParticleCaptureOnProton->SetBarOffset(0.5);
	histogramOfParticleCaptureOnProton->SetStats(0);
	histogramOfParticleCaptureOnOther->SetFillColor(1);
	histogramOfParticleCaptureOnOther->SetBarWidth(0.2);
	histogramOfParticleCaptureOnOther->SetBarOffset(.7);
	histogramOfParticleCaptureOnOther->SetStats(0);

	histogramOfTotalNumberOfEvents->Draw("bar text00");
	histogramOfParticleCaptureOnCaptureNucleus->Draw("bar same text00");
	histogramOfParticleCaptureOnProton->Draw("bar same text00");
	histogramOfParticleCaptureOnOther->Draw("bar same text00");
	//	gPad->Update();
	TLegend* c7Legend = new TLegend(0.65, 0.75, 0.89, 0.89, "Capture Nucleus");
	c7Legend->AddEntry(histogramOfTotalNumberOfEvents, "Total Number of Events", "f");
	c7Legend->AddEntry(histogramOfParticleCaptureOnCaptureNucleus, "Capture Nucleus", "f");
	c7Legend->AddEntry(histogramOfParticleCaptureOnProton, "Proton", "f");
	c7Legend->AddEntry(histogramOfParticleCaptureOnOther, "Other", "f");
	c7Legend->SetMargin(0.1);
	c7Legend->SetTextSize(0.03);
	c7Legend->SetBorderSize(0);
	c7Legend->SetFillColor(0);
	c7Legend->Draw();
	//	gStyle->SetHistMinimumZero();
	c7->SaveAs("histogram_of_capture_nuclei_wrt_boron_doping_percentage.pdf");

	///////////////////////////////////////////
	// Scatter Plot of Tree Data of Particle //
	///////////////////////////////////////////

	TCanvas* c8 = new TCanvas("c8", "canvas8");
	c8->cd();
	c8->SetLogx();
	TLegend* c8Legend = new TLegend(0.11, 0.11, 0.3, 0.2, nameOfVariableToCompare.data());
	gStyle->SetOptStat("nem");

	TH1F* histogramOfTree0;
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude>>histogramOfTree0", runNumberCut[0]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
	histogramOfTree0->GetYaxis()->Set(10, -1, 1);
	histogramOfTree0->GetXaxis()->Set(10, .01, 200);
	histogramOfTree0->SetTitle("#splitline{Cos(#theta) vs Magnitude of Neutron Capture Point Displacement Vector}{(Full Reactor Neutrino Spectrum)}");
	histogramOfTree0->SetName(nameOfRuns[0].data());
	histogramOfTree0->GetYaxis()->SetTitleOffset(1.2);
	histogramOfTree0->SetYTitle("cos(#theta) of neutron capture point vector");
	histogramOfTree0->SetXTitle("magnitude of capture point vector [mm]");
	histogramOfTree0->SetMarkerColor(0);
	histogramOfTree0->Draw("");
	gPad->Update();
	TPaveStats* histogramOfTree0Stat = (TPaveStats*)histogramOfTree0->FindObject("stats");
	histogramOfTree0Stat->SetX1NDC(.90);
	histogramOfTree0Stat->SetY1NDC(.80);
	histogramOfTree0Stat->SetX2NDC(1);
	histogramOfTree0Stat->SetY2NDC(.90);
	
if(numberOfRuns > 1){
	TH1F* histogramOfTree1;
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude>>histogramOfTree1", runNumberCut[1]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");

	histogramOfTree1->SetName(nameOfRuns[1].data());
	histogramOfTree1->SetMarkerColor(0);
	histogramOfTree1->Draw("sames");
	gPad->Update();
	TPaveStats* histogramOfTree1Stat = (TPaveStats*)histogramOfTree1->FindObject("stats");
	histogramOfTree1Stat->SetX1NDC(.90);
	histogramOfTree1Stat->SetY1NDC(.70);
	histogramOfTree1Stat->SetX2NDC(1.);
	histogramOfTree1Stat->SetY2NDC(.80);
	
if(numberOfRuns > 2){
//	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude>>histogramOfTree2", runNumberCut[2]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "goff");
//	histogramOfTree2->SetName(nameOfRuns[2].data());
//	histogramOfTree2->SetMarkerColor(0);
//	histogramOfTree2->Draw("sames");
//	gPad->Update();
//	TPaveStats* histogramOfTree2Stat = (TPaveStats*)histogramOfTree2->FindObject("stats");
//	histogramOfTree2Stat->SetX1NDC(.40);
//	histogramOfTree2Stat->SetY1NDC(.10);
//	histogramOfTree2Stat->SetX2NDC(.65);
//	histogramOfTree2Stat->SetY2NDC(.25);
}
}
	
	tree.SetMarkerStyle(6);

	tree.SetMarkerColor(2);
	histogramOfTree0->SetFillColor(2);
	c8Legend->AddEntry(histogramOfTree0, nameOfRuns[0].data(), "f");
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude", runNumberCut[0]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "same");

if(numberOfRuns > 1){
	tree.SetMarkerColor(3);
	histogramOfTree1->SetFillColor(3);
	c8Legend->AddEntry(histogramOfTree1, nameOfRuns[1].data(), "f");
	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude", runNumberCut[1]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "same");

if(numberOfRuns > 2){	
//	tree.SetMarkerColor(4);
//	histogramOfTree2->SetFillColor(4);
//	c8Legend->AddEntry(histogramOfTree2, nameOfRuns[2].data(), "f");
//	tree.Draw("primaryNeutronPostStepCosineOfDisplacementNormalAngle:primaryNeutronPostStepDisplacementMagnitude", runNumberCut[2]&&killTrackStatusCut&&neutronCut&&primaryPositronStoppedInScintillatorCut&&primaryNeutronStoppedInScintillatorCut&&primaryParticleCut, "same");
}
}

	c8Legend->SetMargin(0.1);
	c8Legend->SetTextSize(0.03);
	c8Legend->SetBorderSize(0);
	c8Legend->SetFillColor(0);
	c8Legend->Draw("");
	c8->SaveAs("scatterplot_of_cosine_of_displacement_normal_angle.pdf");

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// End of Data Analysis Plotting Code//
	/////////////////////////////////////////////////////////////////////////////////////////////////

	*/
	cout
		<< "####################### Program terminated successfully. #######################"
		<< endl
		<< endl
		<< endl;
	return 0;
}
