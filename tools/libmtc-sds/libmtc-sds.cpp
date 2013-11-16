#include "TKey.h"
#include "TROOT.h"

#include <sstream>
#include "libmtc-sds.hpp"

using namespace mtc::sds;
using namespace std;

const string SDS::PID_BRANCH_PREFIX = "PID_";

SDS::SDS(std::string file)
{
    gROOT->ProcessLine("#include <vector>");
    fname_ = file;
    lastRun_ = "";
    lastEvent_ = "";
    lastRunTree_ = NULL;
    lastPidBranch_ = NULL;
	lastDataBranch_ = NULL;
	updateNeeded_ = false;
    open_ = false;
}

bool SDS::open()
{
    treeFile_ = new TFile(fname_.c_str(), "update");
    open_ = treeFile_->IsOpen();
    if(!open_)
        delete treeFile_;
    return open_;
}

void SDS::sync()
{
    if(open_ && treeFile_->IsOpen())
        treeFile_->Flush();
}

bool SDS::close()
{
	if(updateNeeded_){
		treeFile_->cd();
		lastRunTree_->Write("", TObject::kOverwrite);
	}
    if(!open_)
        return false;
    treeFile_->Close();
    open_ = false;
    delete treeFile_;
    return true;
}

std::vector < std::string > SDS::runs()
{
    treeFile_->IsOpen();
    vector < std::string > ret;
    TIter next(treeFile_->GetListOfKeys());
    TKey *key;
    while ((key=(TKey*)next()))
    {
        ret.push_back(std::string(key->GetName()));
    }
    return ret;
}

bool SDS::addRun(struct RunDescriptorStruct run)
{
    std::string name = run.runName;
    std::string title = serializeRunDescriptor(run);
    if(updateNeeded_){
		treeFile_->cd();
		lastRunTree_->Write("", TObject::kOverwrite);
	}
    TTree *tree = new TTree(name.c_str(),title.c_str());

    lastRun_ = name.c_str();
    lastRunTree_ = tree;
	lastEvent_ = "";
	lastPidBranch_ = NULL;
	lastDataBranch_ = NULL;
	
    treeFile_->cd();
    tree->Write();
    return true;
}

struct RunDescriptorStruct SDS::runDescriptor(std::string run)
{
	TTree *runTree = (TTree*)treeFile_->Get(run.c_str());
    std::string title = runTree->GetTitle();
    std::string name = runTree->GetName();
    return deserializeRunDescriptor(name, title);
}

bool SDS::addEvent(std::string run, struct EventDescriptorStruct event)
{
	
    std::vector <float> dummyVector;
    int dummyPid  = 0;
    TTree *runTree;
    if(run == lastRun_)
    {
		runTree = lastRunTree_;
	}
	else
	{
		if(updateNeeded_)
		{
			treeFile_->cd();
			lastRunTree_->Write("", TObject::kOverwrite);
			updateNeeded_ = false;
		}
		runTree = (TTree*)treeFile_->Get(run.c_str());
		lastRunTree_ = runTree;
	}
    std::string dataName = serializeEventDescriptor(event);

    std::string pidName = PID_BRANCH_PREFIX + dataName;

	lastEvent_ = "";
	lastPidBranch_ = NULL;
	lastDataBranch_ = NULL;
	
    runTree->Branch(dataName.c_str(),&dummyVector);

    runTree->Branch(pidName.c_str(), &dummyPid);

	updateNeeded_ = true;
    return true;
}

std::vector < struct EventDescriptorStruct > SDS::events(std::string run)
{
    std::vector < struct EventDescriptorStruct > ret;
    TTree *runTree = (TTree*)treeFile_->Get(run.c_str());
    TObjArray *branches = runTree->GetListOfBranches();
    Int_t nbranches = branches->GetEntries();
    for (Int_t i=0; i<nbranches; i++)
    {
        TBranch *b = (TBranch*)branches->At(i);
        std::string name = b->GetName();
        if(name.compare(0, PID_BRANCH_PREFIX.size(),PID_BRANCH_PREFIX))
            ret.push_back(deserializeEventDescriptor(name));
        continue;
    }
    return ret;
}

bool SDS::addPixel(std::string run, struct EventDescriptorStruct event, int pixel, std::vector<float> pixelData)
{
	std::vector<float> *temp = 0;
    TTree *runTree;
    if(run == lastRun_)
    {
		runTree = lastRunTree_;
	}
	else
	{
		if(updateNeeded_)
		{
			treeFile_->cd();
			lastRunTree_->Write("", TObject::kOverwrite);
			updateNeeded_ = false;
		}
		runTree = (TTree*)treeFile_->Get(run.c_str());
		lastRunTree_ = runTree;
	}
	
    std::string dataName = serializeEventDescriptor(event);
    std::string pidName = PID_BRANCH_PREFIX + dataName;
    
    TBranch *pidBranch;
    TBranch *dataBranch;
    
    if(dataName == lastEvent_)
    {
		pidBranch = lastPidBranch_;
		dataBranch = lastDataBranch_;
	}
	else
	{
		pidBranch = runTree->GetBranch(pidName.c_str());
		dataBranch = runTree->GetBranch(dataName.c_str());
		lastPidBranch_ = pidBranch;
		lastDataBranch_ = dataBranch;
		lastEvent_ = dataName;
	}	
    dataBranch->SetAddress(&temp);
    pidBranch->SetAddress(&pixel);

    temp->assign(pixelData.begin(), pixelData.end());
    dataBranch->Fill();
    pidBranch->Fill();

    runTree->ResetBranchAddresses();
    updateNeeded_ = true;
    return true;
}

std::vector < float > SDS::getPixel(std::string run,  struct EventDescriptorStruct event, int pixel)
{
    std::vector<float> *data = 0;
    int currentPixel;
    std::vector<float> ret;
    ret.clear();
    
    std::string dataName = serializeEventDescriptor(event);
    std::string pidName = PID_BRANCH_PREFIX + dataName;

    TTree *runTree;
    if(run == lastRun_)
    {
		runTree = lastRunTree_;
	}
	else
	{
		if(updateNeeded_)
		{
			treeFile_->cd();
			lastRunTree_->Write("", TObject::kOverwrite);
			updateNeeded_ = false;
		}
		runTree = (TTree*)treeFile_->Get(run.c_str());
		lastRunTree_ = runTree;
	}
	
	TBranch *pidBranch;
    TBranch *dataBranch;

	if(dataName == lastEvent_)
    {
		pidBranch = lastPidBranch_;
		dataBranch = lastDataBranch_;
	}
	else
	{
		pidBranch = runTree->GetBranch(pidName.c_str());
		dataBranch = runTree->GetBranch(dataName.c_str());
		lastPidBranch_ = pidBranch;
		lastDataBranch_ = dataBranch;
		lastEvent_ = dataName;
	}

    dataBranch->SetAddress(&data);
    pidBranch->SetAddress(&currentPixel);

    int npix = pidBranch->GetEntries();

    for(Long64_t i = 0; i< npix; i++)
    {
        pidBranch->GetEntry(i);
        if(currentPixel == pixel)
        {
            dataBranch->GetEntry(i);
            ret.assign(data->begin(), data->end());
            break;
        }
    }
    runTree->ResetBranchAddresses();
    return ret;
}

std::vector < int > SDS::pixels(std::string run, struct EventDescriptorStruct event)
{
    std::vector<int> ret;
    int currentPixel;
    TTree *runTree = (TTree*)treeFile_->Get(run.c_str());
    std::string pidName = PID_BRANCH_PREFIX + serializeEventDescriptor(event);
    TBranch *pidBranch = runTree->GetBranch(pidName.c_str());
    pidBranch->SetAddress(&currentPixel);

    int npix = pidBranch->GetEntries();

    for(Long64_t i = 0; i< npix; i++)
    {
        pidBranch->GetEntry(i);
        ret.push_back(currentPixel);
    }
    runTree->ResetBranchAddresses();
    return ret;
}

