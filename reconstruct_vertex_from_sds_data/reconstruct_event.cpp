#include <cassert>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TStyle.h>
#include <TH3D.h>
#include <TPad.h>
#include <TLegend.h>
#include <TVector3.h>
#include <TMath.h>
#include <TF1.h>

#include "libmtc-sds.hpp"
#include "TFile.h"

const double SCINTILLATOR_REF_INDEX = 1.58;
const double LIGHT_SPEED_IN_SCINTILLATOR =
	TMath::C()/SCINTILLATOR_REF_INDEX*1.0e3*1.0e-9;

void generateDictionary()
{
	if (!(gInterpreter->IsLoaded("vector"))) {                                  
		gInterpreter->ProcessLine("#include <vector>");                         
		//gROOT->ProcessLine("#include <vector>");                              
	}                                                                           
	gSystem->Exec("rm -f AutoDict*");                                         
	gInterpreter->GenerateDictionary("std::vector<TVector3>","TVector3.h");   
	gInterpreter->GenerateDictionary(                                           
			"vector<TVector3>;"                                        
			"vector< vector<TVector3> >;"                              
			"vector< vector<string> >;"                                
			"vector< vector<int> >"                                    
			"vector< vector<double> >",                                
			"TVector3.h");
	return;
}

struct PmtData {
	int pixelID;
	TVector3 pixelPosition;
	double time;
	int charge;
};

struct ByPmtID {
	bool operator() (const PmtData& a, const PmtData& b) {
		return a.pixelID < b.pixelID;
	}
};

struct ByHitTime {
	bool operator() (const PmtData& a, const PmtData& b) {
		return a.time < b.time;
	}
};

void plotT0(int argc, char** argv)
{
	int pe;
	double t0;

	TFile* f = new TFile(argv[1], "read");
	TTree* t = dynamic_cast<TTree*>(f->Get("event_tree"));
	t->SetBranchAddress("totalPE", &pe);
	t->SetBranchAddress("t0", &t0);

	gStyle->SetOptStat("emrou");
	TH1D hist_t0("hist_t0", "hist_t0", 100, -1, 1);
	hist_t0.StatOverflows();

	for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
		t->GetEntry(iEvent);
		if (pe < 70) continue;
		std::cout << "t0 = " << t0 << std::endl;
		hist_t0.Fill(t0);
	}

	hist_t0.Draw();
	gPad->SaveAs("hist_t0.pdf");
}

void plotPE(int argc, char** argv)
{
	int totalPE;

	TFile* f = new TFile(argv[1], "read");
	TTree* t = dynamic_cast<TTree*>(f->Get("event_tree"));
	t->SetBranchAddress("totalPE", &totalPE);

	gStyle->SetOptStat("emrou");
	TH1D hist_pe("hist_pe", "hist_pe", 100, 0, 300);
	hist_pe.StatOverflows();

	for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
		t->GetEntry(iEvent);
		hist_pe.Fill((double)totalPE);
	}

	hist_pe.Draw();
	gPad->SaveAs("hist_pe.pdf");
}

void plotReconDirection(int argc, char** argv)
{
	// Create map holding track info from ROOT step data file.
	// Get truth neutron momentums from ROOT file.
	std::map<int, TVector3> truthMomentums;
	std::map<int, bool> neutronsAreCapturedInScint;
	int runID;
	int eventID;
	int pdg;
	int parentID;
	int stepID;
	int trackStatus;
	double nMom_x, nMom_y, nMom_z;
	char postStepPhysVolumeName[256];
	TFile* fTruth = new TFile("100kev_n_from-65mmSurfaceToUpperHemisphere_all_particle_step.root",
			"read");
	TTree* tTruth = (TTree*)fTruth->Get("stepTree");
	tTruth->SetBranchAddress("eventID", &eventID);
	tTruth->SetBranchAddress("pdgEncoding", &pdg);
	tTruth->SetBranchAddress("stepID", &stepID);
	tTruth->SetBranchAddress("parentID", &parentID);
	tTruth->SetBranchAddress("postStepMomX", &nMom_x);
	tTruth->SetBranchAddress("postStepMomY", &nMom_y);
	tTruth->SetBranchAddress("postStepMomZ", &nMom_z);
	tTruth->SetBranchAddress("trackStatus", &trackStatus);
	tTruth->SetBranchAddress("postStepPhysVolumeName", postStepPhysVolumeName);
	for (int iEvent = 0; iEvent < tTruth->GetEntries(); ++iEvent) {
		tTruth->GetEntry(iEvent);
		if (pdg != 2112) continue;
		if (parentID != 0) continue;
		if (stepID == 0) {
			std::cout << "event: " << eventID << ", "
				<< "step: " << stepID << ", "
				<< "pdg: " << pdg << ", "
				<< "postStepMomX: " << nMom_x << std::endl;
			truthMomentums.insert(std::pair<int, TVector3>(eventID, TVector3(nMom_x,
							nMom_y, nMom_z)));
		}
		if (trackStatus == 2) {
			std::cout << "postStepPhysVolumeName: " <<
				std::string(postStepPhysVolumeName) << std::endl;
			if (std::string(postStepPhysVolumeName) == "outOfWorld")
				neutronsAreCapturedInScint.insert(
												  std::pair<int, bool>(eventID,
																	   false));
			else
				neutronsAreCapturedInScint.insert(
												  std::pair<int, bool>(eventID,
																	   true));
		}
	}
	for (int iEvent = 0; iEvent < truthMomentums.size(); ++iEvent) {
		std::cout << "event: " << iEvent
			<< ", mom: " << truthMomentums.at(iEvent).X() << std::endl;
	}
	for (int iEvent = 0; iEvent < neutronsAreCapturedInScint.size(); ++iEvent) {
		std::cout << "neutron captured in scint: " <<
			neutronsAreCapturedInScint.at(iEvent) << std::endl;
	}
	delete fTruth;


	int totalPE;
	TVector3* centerOfCharge = NULL;
	TVector3* centerOfTime_mean = NULL;
	TVector3* centerOfTime_peak = NULL;

	TFile* f = new TFile(argv[1], "read");
	TTree* t = dynamic_cast<TTree*>(f->Get("event_tree"));
	t->SetBranchAddress("runID", &runID);
	t->SetBranchAddress("eventID", &eventID);
	t->SetBranchAddress("totalPE", &totalPE);
	t->SetBranchAddress("centerOfCharge", &centerOfCharge);
	t->SetBranchAddress("centerOfTime_mean", &centerOfTime_mean);
	t->SetBranchAddress("centerOfTime_peak", &centerOfTime_peak);

	gStyle->SetOptStat("emrou");
	TH1D hist_reconDirCosine("reconAngle", "reconAngle", 10, -1, 1);
	hist_reconDirCosine.StatOverflows();

	for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
		std::cout << "iEvent: " << iEvent << std::endl;
		t->GetEntry(iEvent);
		if (neutronsAreCapturedInScint.at(iEvent) == false) continue;
		std::cout << "eventID: " << eventID << std::endl;
		TVector3 trueDir(0, 0, 1);
		trueDir = truthMomentums.at(eventID);
		//TVector3 reconDir = (*centerOfCharge) - (*centerOfTime_mean);
		//TVector3 reconDir = (*centerOfCharge) - (*centerOfTime_peak);
		TVector3 reconDir = (*centerOfCharge) - TVector3(0, 0, 0);
		double cosine = cos(trueDir.Angle(reconDir));
		if (cosine == cosine) {
			std::cout << "cosine: " << cosine << std::endl;
			hist_reconDirCosine.Fill(cosine);
		}
	}

	hist_reconDirCosine.Draw();
	hist_reconDirCosine.GetXaxis()->SetTitle("cosine of agreement angle");
	gPad->SaveAs("hist_reconDirCosine.pdf");

	return;
}

void plotCenterOfParameters(int argc, char** argv)
{
	int totalPE;
	TVector3* centerOfCharge = NULL;
	TVector3* centerOfTime_mean = NULL;
	TVector3* centerOfTime_peak = NULL;

	TFile* f = new TFile(argv[1], "read");
	TTree* t = dynamic_cast<TTree*>(f->Get("event_tree"));
	t->SetBranchAddress("totalPE", &totalPE);
	t->SetBranchAddress("centerOfCharge", &centerOfCharge);
	t->SetBranchAddress("centerOfTime_mean", &centerOfTime_mean);
	t->SetBranchAddress("centerOfTime_peak", &centerOfTime_peak);

	TH1D hist_x("hist_x", "hist_x", 100, -70, 70);
	hist_x.StatOverflows();
	TH1D hist_y("hist_y", "hist_y", 100, -70, 70);
	hist_y.StatOverflows();
	TH1D hist_z("hist_z", "hist_z", 100, -70, 70);
	hist_z.StatOverflows();

	for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
		t->GetEntry(iEvent);
		hist_x.Fill(centerOfCharge->X());
		hist_y.Fill(centerOfCharge->Y());
		hist_z.Fill(centerOfCharge->Z());
	}
	//for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
	//	t->GetEntry(iEvent);
	//	hist_x.Fill(centerOfTime_mean->X());
	//	hist_y.Fill(centerOfTime_mean->Y());
	//	hist_z.Fill(centerOfTime_mean->Z());
	//}
	//for (int iEvent = 0; iEvent < t->GetEntries(); ++iEvent) {
	//	t->GetEntry(iEvent);
	//	double x = centerOfTime_peak->X();
	//	double y = centerOfTime_peak->Y();
	//	double z = centerOfTime_peak->Z();
	//	std::cout << "x: " << x << std::endl;
	//	std::cout << "y: " << y << std::endl;
	//	std::cout << "z: " << z << std::endl;
	//	hist_x.Fill(x);
	//	hist_y.Fill(y);
	//	hist_z.Fill(z);
	//}

	gStyle->SetOptStat("emrou");
	hist_x.Draw();
	gPad->SaveAs("recon_x.pdf");
	hist_y.Draw();
	gPad->SaveAs("recon_y.pdf");
	hist_z.Draw();
	gPad->SaveAs("recon_z.pdf");
}

template <class T> bool isUnique(const std::vector<T>& X) {
	std::set<T> Y(X.begin(), X.end());
	return X.size() == Y.size();
}

TVector3 getCenterOfCharge(const std::vector<PmtData>& pmtData)
{
	std::vector<int> ids;
	std::vector<TVector3> positions;
	std::vector<int> charges;
	for (std::vector<PmtData>::const_iterator it = pmtData.begin(); it !=
			pmtData.end(); ++it) {
		int id = it->pixelID;
		TVector3 pos = it->pixelPosition;
		int charge = it->charge;
			// User commands.
			std::vector<int>::iterator iFind =
				std::find(ids.begin(), ids.end(), id);
			if (iFind == ids.end()) {// Pixel not yet recorded.
				ids.push_back(id);
				positions.push_back(pos);
				charges.push_back(charge);
			}
			else { // Pixel recorded before.
				int pos = iFind - ids.begin();
				charges.at(pos) += charge;
			}
	}
	// PMT IDs should be unique.
	if (isUnique(ids) == false) abort();
	// Vector sizes should be all identical.
	assert(ids.size() == positions.size());
	assert(ids.size() == charges.size());

	TVector3 centerOfCharge(0, 0, 0);
	const int nPixels = ids.size();
	for (int iPixel = 0; iPixel < nPixels; ++iPixel) {
		TVector3 pos = positions.at(iPixel);
		double q = (double)charges.at(iPixel);
		double weight = sqrt(q);
		centerOfCharge += weight*pos;
	}
	centerOfCharge *= 1.0/((double)nPixels);

	// Return value.
	return centerOfCharge;
}

//TVector3 getCenterOfTime_mean(
//		const std::vector<int>& ids,
//		const std::vector<TVector3>& positions,
//		const std::vector<double>& times,
//	   	const TVector3& centerOfCharge)
//{
//	// PMT IDs should be unique.
//	if (isUnique(ids) == false) abort();
//	// Vector sizes should be all identical.
//	assert(ids.size() == positions.size());
//	assert(ids.size() == times.size());
//
//	const int nPixels = ids.size();
//
//	// Get T0.
//	TH1D hist_t0;
//	hist_t0.StatOverflows();
//	for (int iPixel = 0; iPixel < nPixels; ++iPixel) {
//		TVector3 pos = positions.at(iPixel);
//		double t = times.at(iPixel);
//		double dist = (pos - centerOfCharge).Mag();
//		double t0 = t - dist/LIGHT_SPEED_IN_SCINTILLATOR;
//		hist_t0.Fill(t0);
//	}
//	const double T0 = hist_t0.GetMean();
//
//	// Get center of time.
//	TVector3 centerOfTime(0, 0, 0);
//	for (int iPixel = 0; iPixel < nPixels; ++iPixel) {
//		TVector3 pos = positions.at(iPixel);
//		double t = times.at(iPixel);
//		double weight = 1.0/(t - T0);
//		centerOfTime += weight*pos;
//	}
//	centerOfTime *= 1.0/((double)nPixels);
//
//	// Return value.
//	return centerOfTime;
//}

double doublePulseFct(double* x, double* par)
{
	double firstPulse = (x[0] < par[1])? 0.0 : exp(-(x[0] - par[1])/2.2);
	double secondPulse = (x[0] < par[3])? 0.0 : exp(-(x[0] - par[3])/2.2);
	return par[0]*firstPulse + par[2]*secondPulse;
}

TVector3 getCenterOfTime_peak(
		const int& i_run, const int& i_event,
		const std::vector<PmtData>& pmtData,
		const TVector3& centerOfCharge,
		double& t0)
{
	// Get fist hit times.
	std::vector<int> ids;
	std::vector<TVector3> positions;
	std::vector<int> times; // Fist hit times.
	for (std::vector<PmtData>::const_iterator it = pmtData.begin(); it !=
			pmtData.end(); ++it) {
		int id = it->pixelID;
		TVector3 pos = it->pixelPosition;
		double time = it->time;
			// User commands.
			std::vector<int>::iterator iFind =
				std::find(ids.begin(), ids.end(), id);
			if (iFind == ids.end()) {// Pixel not yet recorded.
				ids.push_back(id);
				positions.push_back(pos);
				times.push_back(time);
			}
			else { // Pixel recorded before.
				int pos = iFind - ids.begin();
				if (times.at(pos) > time) times.at(pos) = time;
			}
	}
	// PMT IDs should be unique.
	if (isUnique(ids) == false) abort();
	// Vector sizes should be all identical.
	assert(ids.size() == positions.size());
	assert(ids.size() == times.size());

	const int nPixels = ids.size();

	// Get T0.
	std::vector<double> t0s;
	for (int iPixel = 0; iPixel < nPixels; ++iPixel) {
		TVector3 pos = positions.at(iPixel);
		double t = times.at(iPixel);
		double dist = (pos - centerOfCharge).Mag();
		t0s.push_back(t - dist/LIGHT_SPEED_IN_SCINTILLATOR);
	}
	const double histMin_x = *std::min_element(t0s.begin(), t0s.end());
	const double histMax_x = *std::max_element(t0s.begin(), t0s.end()) + 1.0;
	std::cout << "histMin_x: " << histMin_x << std::endl;
	std::cout << "histMax_x: " << histMax_x << std::endl;
	TH1D hist_t0("hist_t0", "hist_t0", 100, histMin_x, histMax_x);
	hist_t0.StatOverflows(true);
	for (std::vector<double>::iterator it = t0s.begin(); it != t0s.end(); ++it)
		hist_t0.Fill(*it);
	gStyle->SetOptStat("emrou");
	hist_t0.Draw();
	gPad->Update();
	std::ostringstream runEventID("");
	runEventID << i_run << "_" << i_event;
	std::string hist_t0_name = "hist_t0_" + runEventID.str() + ".pdf";
	gPad->SaveAs(hist_t0_name.c_str());
	const int binMax = hist_t0.GetMaximumBin();
	const double peak_content = hist_t0.GetBinContent(hist_t0.GetMaximumBin());
	//const double peak_x = hist_t0.GetXaxis()->GetBinCenter(binMax);
	double peak_x = 0.0;
	std::cout << "nbins: " << hist_t0.GetNbinsX() << std::endl;
	for (int iBin = 1; iBin <= hist_t0.GetNbinsX(); ++iBin) { // Search bins.
		double content = hist_t0.GetBinContent(iBin);
		if (content < 0.5*peak_content) {
			continue;
		}
		else {
			peak_x = hist_t0.GetXaxis()->GetBinCenter(iBin);
			break;
		}
	}
	std::cout << "peak_x: " << peak_x << std::endl;
	double early_min_x = floor(histMin_x - 10.0);
	double early_max_x = floor(histMin_x + 30.0);
	TH1D hist_earlyT0("hist_earlyT0", "hist_earlyT0",
			int(early_max_x - early_min_x)/1, early_min_x, early_max_x);
	hist_earlyT0.StatOverflows(false);
	for (std::vector<double>::iterator it = t0s.begin(); it != t0s.end(); ++it)
		hist_earlyT0.Fill(*it);

	// Fit early time distribution.
	double fitMin_x = early_min_x;;
	std::cout << "fitMin_x: " << fitMin_x << std::endl;
	double fitMax_x = early_max_x;;
	std::cout << "fitMax_x: " << fitMax_x << std::endl;
	// Fit function.
	const int npar = 4;
	TF1 fitfunc("fitfunc", doublePulseFct, fitMin_x, fitMax_x, npar);
	fitfunc.SetParameters(2, 0, 2, 0);
	hist_earlyT0.Fit("fitfunc", "RM");
	//TF1* fittedFunc = hist_t0.GetFunction("fitfunc");
	double pars[npar];
	fitfunc.GetParameters(pars);
	for (int ipar = 0; ipar < npar; ++ipar) {
		std::cout << "par[" << ipar << "]: " << pars[ipar] << std::endl;
	}
	//if (fittedFunc) t0 = static_cast<double>(fittedFunc->GetMaximumX());

	// Get mean of early hits.
	t0 = hist_earlyT0.GetMean();
	std::cout << "t0: " << t0 << std::endl;
	hist_earlyT0.Draw();
	std::string hist_earlyT0_name = "hist_earlyT0_" + runEventID.str() + ".pdf";
	gPad->SaveAs(hist_earlyT0_name.c_str());

	// Get center of time.
	TVector3 centerOfTime(0, 0, 0);
	for (int iPixel = 0; iPixel < nPixels; ++iPixel) {
		TVector3 pos = positions.at(iPixel);
		double t = times.at(iPixel);
		//if (t - t0 <= 0.0) continue; // This make directionality worse.
		double weight = 1.0/(t - t0);
		centerOfTime += weight*pos;
	}
	centerOfTime *= 1.0/((double)nPixels);

	// Return value.
	return centerOfTime;
}

int getTotalPE(const std::vector<PmtData>& pmtData)
{
	int totalPE = 0;
	for (std::vector<PmtData>::const_iterator it = pmtData.begin();
			it != pmtData.end(); ++it) {
		totalPE += it->charge;
	}
	return totalPE;
}

void getPmtData(
		const std::string& run_name,
		const mtc::sds::EventDescriptorStruct& event, // Event descriptor.
		mtc::sds::SDS& sds,
		std::vector<int> pixels,
		std::vector<PmtData>& collectionOfPmtData)
{
	// Clear vectors.
	collectionOfPmtData.clear();

	for (int i_pixel = 0; i_pixel < pixels.size(); i_pixel++) {
		std::vector<float> data = // List of number of p.e. and hittimes.
		   	sds.getPixel(run_name, event, pixels.at(i_pixel));
		int i_data = 0;
		while (i_data < data.size()) {
			PmtData pmtData;
			int pixelID = pixels.at(i_pixel);
			pmtData.pixelID = pixelID;
			int column = pixelID % 10;
			pixelID /= 10;
			int row = pixelID % 10;
			pixelID /= 10;
			int pmtID = pixelID;
			pmtData.time = data.at(i_data);
			pmtData.charge = data.at(++i_data);
			double x = data.at(++i_data);
			double y = data.at(++i_data);
			double z = data.at(++i_data);
			pmtData.pixelPosition = TVector3(x, y, z);
			collectionOfPmtData.push_back(pmtData);

			++i_data;

			//std::cout << run_name << "\t";
			//std::cout << event.number << "\t";
			//std::cout << pixelID << std::endl;
			//std::cout << "pmtID: " << pmtID << std::endl;
			//std::cout << "row: " << row << std::endl;
			//std::cout << "column: " << column << std::endl;
			//std::cout << "x: " << x << std::endl;
			//std::cout << "y: " << y << std::endl;
			//std::cout << "z: " << z << std::endl;
		}
	}

	std::sort(collectionOfPmtData.begin(), collectionOfPmtData.end(),
		   	ByHitTime()); // Sort by time.
}

void createEventTree(int argc, char** argv)
{
	int runID = 0;
	int eventID = 0;
	int totalPE = 0;
	double t0 = 0.0;
	TVector3 centerOfCharge(0, 0, 0);
	TVector3 centerOfTime_mean(0, 0, 0);
	TVector3 centerOfTime_peak(0, 0, 0);

	TFile* f = new TFile("recon.root", "recreate");
	TTree* t = new TTree("event_tree", "event_tree");
	t->SetDirectory(f);
	t->Branch("runID", &runID);
	t->Branch("eventID", &eventID);
	t->Branch("totalPE", &totalPE);
	t->Branch("t0", &t0);
	t->Branch("centerOfCharge", &centerOfCharge);
	t->Branch("centerOfTime_mean", &centerOfTime_mean);
	t->Branch("centerOfTime_peak", &centerOfTime_peak);

	struct mtc::sds::RunDescriptorStruct run; // Run descriptor.
    struct mtc::sds::EventDescriptorStruct event; // Event descriptor.
	std::vector<std::string> list_of_runs; // List of run names.
	std::vector<mtc::sds::EventDescriptorStruct> list_of_events; // list of event descriptors.

	/*
	 * PMT pixel ID number = 100*<PMT ID#> + 10*<anode row#> + <anode column#>
	 * 
	 */

	for (int i_arg = 1; i_arg < argc; i_arg++) { // Iterate over input root files.
		mtc::sds::SDS sds(argv[i_arg]);
		sds.open();
		list_of_runs = sds.runs();
		for (int i_run = 0; i_run < list_of_runs.size(); i_run++) {
			std::string run_name = list_of_runs.at(i_run);
			list_of_events = sds.events(run_name);
			for (int i_event = 0; i_event < list_of_events.size(); i_event++) {
				//if (i_event != 0) continue;
				std::cout << "Run: " << i_run << ", event: " << i_event <<
					std::endl;
				event = list_of_events.at(i_event);
				std::vector<int> pixels = // PMT pixel ID numbers.
					sds.pixels(run_name, event);

				// User commands.
				runID = i_run;
				eventID = i_event;
				std::vector<PmtData> pmtData;
				getPmtData(run_name, event, sds, pixels, pmtData);
				totalPE = getTotalPE(pmtData);
				centerOfCharge = getCenterOfCharge(pmtData);
				//centerOfTime_mean = getCenterOfTime_mean(pmtData);
				centerOfTime_peak = getCenterOfTime_peak(i_run, i_event,
						pmtData, centerOfCharge, t0);
				t->Fill();
				std::cout << "i_event: " << i_event << std::endl;
				for (std::vector<PmtData>::iterator i = pmtData.begin(); i !=
						pmtData.end(); ++i) {
					std::cout << "t: " << i->time
						<< ", q: " << i->charge << std::endl;
				}
				std::cout << "cOfQ: ("
					<< centerOfCharge.X() << ", "
					<< centerOfCharge.Y() << ", "
					<< centerOfCharge.Z() << ")\n";
			}
			if (i_run == 0) break;
		}
		sds.close();
	}

	f->cd();
	t->Write();
	f->Close();
	delete f;
}

int main(int argc, char **argv)
{
	std::cout << "Starting extraction.\n";

	//generateDictionary();

	if (argc <= 1) {
		std::cerr << "Execution format: read_mtc_root_data <input files>\n" <<
			std::endl;
		return 0;
	}

	//createEventTree(argc, argv);
	//plotT0(argc, argv);
	//plotPE(argc, argv);
	//plotCenterOfParameters(argc, argv);
	plotReconDirection(argc, argv);

	std::cout << "Done.\n";

	return 0;
}
