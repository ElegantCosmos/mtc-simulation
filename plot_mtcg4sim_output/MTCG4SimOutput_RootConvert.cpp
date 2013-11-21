#include <iostream>
#include <fstream>
#include <string>

#include "TH1.h"
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
#include "TH3.h"

using namespace std;

class ParticleStep{
	public:
		ParticleStep(): // Constructor for head particle step node.
			eventNumber(0),
			neutrinoKineticEnergy(0),
			neutrinoMomentumUnitVectorX(0),
			neutrinoMomentumUnitVectorY(0),
			neutrinoMomentumUnitVectorZ(0),
			stepNumber(0),
			particleID(0),
			trackID(0),
			parentID(0),
			preStepPositionX(0),
			preStepPositionY(0),
			preStepPositionZ(0),
			postStepPositionX(0),
			postStepPositionY(0),
			postStepPositionZ(0),
			preStepGlobalTime(0),
			postStepGlobalTime(0),
			preStepKineticEnergy(0),
			postStepKineticEnergy(0),
			totalEnergyDeposit(0),
			stepLength(0),
			trackLength(0),
			processType(0),
			processSubType(0),
			/*
			boundaryStatusName(0),
			isBetweenScintillatorAndSensitivePixel(0),
			*/
			trackStatus(0),

			particlePostStepPointInScintillatorFlag(false){
				nextParticleStep = this;
				previousParticleStep = this;
				particleStepHeaderFlag = true;
				resetPrimaryNeutronStoppedInScintillatorFlag();
				resetPrimaryPositronStoppedInScintillatorFlag();
				resetPrimaryNeutronStoppingTime();
				resetPrimaryPositronStoppingTime();
				resetPrimaryNeutronCosineOfInitialNormalAngle();
				resetPrimaryPositronCosineOfInitialNormalAngle();
				resetTotalNumberOfSteps();
			}
		ParticleStep(
				int newEventNumber,
				double newNeutrinoKineticEnergy,
				double newNeutrinoMomentumUnitVectorX,
				double newNeutrinoMomentumUnitVectorY,
				double newNeutrinoMomentumUnitVectorZ,
				int newStepNumber,
				int newParticleID,
				int newTrackID,
				int newParentID,
				double newPreStepPositionX,
				double newPreStepPositionY,
				double newPreStepPositionZ,
				double newPostStepPositionX,
				double newPostStepPositionY,
				double newPostStepPositionZ,
				double newPreStepGlobalTime,
				double newPostStepGlobalTime,
				double newPreStepKineticEnergy,
				double newPostStepKineticEnergy,
				double newTotalEnergyDeposit,
				double newStepLength,
				double newTrackLength,
				int newProcessType,
				int newProcessSubType,
				int newTrackStatus,
				string newBoundaryStatusName,
				string newIsBetweenScintillatorAndSensitivePixel,
				bool newParticlePostStepPointInScintillator)
					:
					eventNumber(newEventNumber),
					neutrinoKineticEnergy(newNeutrinoKineticEnergy),
					neutrinoMomentumUnitVectorX(newNeutrinoMomentumUnitVectorX),
					neutrinoMomentumUnitVectorY(newNeutrinoMomentumUnitVectorY),
					neutrinoMomentumUnitVectorZ(newNeutrinoMomentumUnitVectorZ),
					stepNumber(newStepNumber),
					particleID(newParticleID),
					trackID(newTrackID),
					parentID(newParentID),
					preStepPositionX(newPreStepPositionX),
					preStepPositionY(newPreStepPositionY),
					preStepPositionZ(newPreStepPositionZ),
					postStepPositionX(newPostStepPositionX),
					postStepPositionY(newPostStepPositionY),
					postStepPositionZ(newPostStepPositionZ),
					preStepGlobalTime(newPreStepGlobalTime),
					postStepGlobalTime(newPostStepGlobalTime),
					preStepKineticEnergy(newPreStepKineticEnergy),
					postStepKineticEnergy(newPostStepKineticEnergy),
					totalEnergyDeposit(newTotalEnergyDeposit),
					stepLength(newStepLength),
					trackLength(newTrackLength),
					processType(newProcessType),
					processSubType(newProcessSubType),
					trackStatus(newTrackStatus),
					boundaryStatusName(newBoundaryStatusName),
					isBetweenScintillatorAndSensitivePixel
						(newIsBetweenScintillatorAndSensitivePixel),
					particlePostStepPointInScintillatorFlag(newParticlePostStepPointInScintillator)
		{
			totalNumberOfSteps++;
			particleStepHeaderFlag = false;
		}
		~ParticleStep()
		{
			totalNumberOfSteps--;
		}
		void appendParticleStep(ParticleStep* newParticleStep){ // Attach particle step node after this node.
			newParticleStep->previousParticleStep = this;
			newParticleStep->nextParticleStep = nextParticleStep;
			nextParticleStep->previousParticleStep = newParticleStep;
			nextParticleStep = newParticleStep;
		}
		void insertParticleStep(ParticleStep* newParticleStep){ // Attach particle step node before this node.
			newParticleStep->previousParticleStep = previousParticleStep;
			newParticleStep->nextParticleStep = this;
			previousParticleStep->nextParticleStep = newParticleStep;
			previousParticleStep = newParticleStep;
		}
		void removeParticleStep(){
			nextParticleStep->previousParticleStep = previousParticleStep;
			previousParticleStep->nextParticleStep = nextParticleStep;
		}
		int getEventNumber(){ return eventNumber; }
		double getNeutrinoKineticEnergy(){ return neutrinoKineticEnergy; }
		double getNeutrinoMomentumUnitVectorX(){ return neutrinoMomentumUnitVectorX; }
		double getNeutrinoMomentumUnitVectorY(){ return neutrinoMomentumUnitVectorY; }
		double getNeutrinoMomentumUnitVectorZ(){ return neutrinoMomentumUnitVectorZ; }
		int getStepNumber(){ return stepNumber; }
		int getParticleID(){ return particleID; }
		int getTrackID(){ return trackID; }
		int getParentID(){ return parentID; }
		double getPreStepPositionX(){ return preStepPositionX; }
		double getPreStepPositionY(){ return preStepPositionY; }
		double getPreStepPositionZ(){ return preStepPositionZ; }
		double getPostStepPositionX(){ return postStepPositionX; }
		double getPostStepPositionY(){ return postStepPositionY; }
		double getPostStepPositionZ(){ return postStepPositionZ; }
		double getPreStepGlobalTime(){ return preStepGlobalTime; }
		double getPostStepGlobalTime(){ return postStepGlobalTime; }
		double getPreStepKineticEnergy(){ return preStepKineticEnergy; }
		double getPostStepKineticEnergy(){ return postStepKineticEnergy; }
		double getTotalEnergyDeposit(){ return totalEnergyDeposit; }
		double getStepLength(){ return stepLength; }
		double getTrackLength(){ return trackLength; }
		int getProcessType(){ return processType; }
		int getProcessSubType(){ return processSubType; }
		string getBoundaryStatusName(){ return boundaryStatusName; }
		string getIsBetweenScintillatorAndSensitivePixel()
		{ return isBetweenScintillatorAndSensitivePixel; }
		int getTrackStatus(){ return trackStatus; }

		bool getParticleStepHeaderFlag(){ return particleStepHeaderFlag; }
		bool getParticlePostStepPointInScintillatorFlag(){ return particlePostStepPointInScintillatorFlag; }
		ParticleStep* getNextParticleStep(){ return nextParticleStep; }
		ParticleStep* getPreviousParticleStep(){ return previousParticleStep; }

		static void resetTotalNumberOfSteps(){ totalNumberOfSteps = 0; }

		static void setPrimaryNeutronStoppedInScintillatorFlag(bool value){ primaryNeutronStoppedInScintillatorFlag = value; }
		static void resetPrimaryNeutronStoppedInScintillatorFlag(){ primaryNeutronStoppedInScintillatorFlag = false; }
		static bool getPrimaryNeutronStoppedInScintillatorFlag(){ return primaryNeutronStoppedInScintillatorFlag; }

		static void setPrimaryPositronStoppedInScintillatorFlag(bool value){ primaryPositronStoppedInScintillatorFlag = value; }
		static void resetPrimaryPositronStoppedInScintillatorFlag(){ primaryPositronStoppedInScintillatorFlag = false; }
		static bool getPrimaryPositronStoppedInScintillatorFlag(){ return primaryPositronStoppedInScintillatorFlag; }

		static void setPrimaryNeutronStoppingTime(double value){ primaryNeutronStoppingTime = value; }
		static void resetPrimaryNeutronStoppingTime(){ primaryNeutronStoppingTime = -1.; }
		static double getPrimaryNeutronStoppingTime(){ return primaryNeutronStoppingTime; }

		static void setPrimaryPositronStoppingTime(double value){ primaryPositronStoppingTime = value; }
		static void resetPrimaryPositronStoppingTime(){ primaryPositronStoppingTime = -1; }
		static double getPrimaryPositronStoppingTime(){ return primaryPositronStoppingTime; }

		static void setPrimaryNeutronCosineOfInitialNormalAngle(double value){ primaryNeutronCosineOfInitialNormalAngle = value; }
		static void resetPrimaryNeutronCosineOfInitialNormalAngle(){ primaryNeutronCosineOfInitialNormalAngle = -2.; }
		static double getPrimaryNeutronCosineOfInitialNormalAngle(){ return primaryNeutronCosineOfInitialNormalAngle; }

		static void setPrimaryPositronCosineOfInitialNormalAngle(double value){ primaryPositronCosineOfInitialNormalAngle = value; }
		static void resetPrimaryPositronCosineOfInitialNormalAngle(){ primaryPositronCosineOfInitialNormalAngle = -2.; }
		static double getPrimaryPositronCosineOfInitialNormalAngle(){ return primaryPositronCosineOfInitialNormalAngle; }

	private:
		ParticleStep* nextParticleStep;
		ParticleStep* previousParticleStep;
		static bool primaryNeutronStoppedInScintillatorFlag, primaryPositronStoppedInScintillatorFlag;
		static double primaryNeutronStoppingTime, primaryPositronStoppingTime;
		static double primaryNeutronCosineOfInitialNormalAngle, primaryPositronCosineOfInitialNormalAngle;
		static int totalNumberOfSteps;

		bool particleStepHeaderFlag;
		int eventNumber;
		double neutrinoKineticEnergy;
		double neutrinoMomentumUnitVectorX;
		double neutrinoMomentumUnitVectorY;
		double neutrinoMomentumUnitVectorZ;
		int stepNumber;
		int particleID;
		int trackID;
		int parentID;
		double preStepPositionX, preStepPositionY, preStepPositionZ;
		double postStepPositionX, postStepPositionY, postStepPositionZ;
		double preStepGlobalTime, postStepGlobalTime;
		double preStepKineticEnergy, postStepKineticEnergy;
		double totalEnergyDeposit;
		double stepLength;
		double trackLength;
		int processType;
		int processSubType;
		int trackStatus;
		string boundaryStatusName;
		string isBetweenScintillatorAndSensitivePixel;
		bool particlePostStepPointInScintillatorFlag;
};

int ParticleStep::totalNumberOfSteps = 0;
bool ParticleStep::primaryNeutronStoppedInScintillatorFlag = false;
bool ParticleStep::primaryPositronStoppedInScintillatorFlag = false;
double ParticleStep::primaryNeutronStoppingTime = -1.;
double ParticleStep::primaryPositronStoppingTime = -1.;
double ParticleStep::primaryNeutronCosineOfInitialNormalAngle = -2.;
double ParticleStep::primaryPositronCosineOfInitialNormalAngle = -2.;

void outputParticleSteps(ParticleStep* particleStep, string foutString){
	ofstream fout;
	fout.open(foutString.data(), ofstream::out | ofstream::app);
	if(!fout.good() || fout.bad()){
		cout << "Error opening output file!\n";
		return;
	}
	while(true){
		if(particleStep->getParticleStepHeaderFlag() == false){
			//cout << particleStep->getEventNumber() << endl;
			fout
				<< particleStep->getEventNumber() << " "
				<< particleStep->getNeutrinoKineticEnergy() << " "
				<< particleStep->getNeutrinoMomentumUnitVectorX() << " "
				<< particleStep->getNeutrinoMomentumUnitVectorY() << " "
				<< particleStep->getNeutrinoMomentumUnitVectorZ() << " "
				<< particleStep->getStepNumber() << " "
				<< particleStep->getParticleID() << " "
				<< particleStep->getTrackID() << " "
				<< particleStep->getParentID() << " "
				<< particleStep->getPreStepPositionX() << " "
				<< particleStep->getPreStepPositionY() << " "
				<< particleStep->getPreStepPositionZ() << " "
				<< particleStep->getPostStepPositionX() << " "
				<< particleStep->getPostStepPositionY() << " "
				<< particleStep->getPostStepPositionZ() << " "
				<< particleStep->getPreStepGlobalTime() << " "
				<< particleStep->getPostStepGlobalTime() << " "
				<< particleStep->getPreStepKineticEnergy() << " "
				<< particleStep->getPostStepKineticEnergy() << " "
				<< particleStep->getTotalEnergyDeposit() << " "
				<< particleStep->getStepLength() << " "
				<< particleStep->getTrackLength() << " "
				<< particleStep->getProcessType() << " "
				<< particleStep->getProcessSubType() << " "
				<< particleStep->getTrackStatus() << " "
				<< particleStep->getBoundaryStatusName() << " "
				<< particleStep->getIsBetweenScintillatorAndSensitivePixel() << " "
				<< particleStep->getParticlePostStepPointInScintillatorFlag() << " "
				<< ParticleStep::getPrimaryPositronStoppedInScintillatorFlag() << " "
				<< ParticleStep::getPrimaryNeutronStoppedInScintillatorFlag() << " "
				<< ParticleStep::getPrimaryPositronStoppingTime() << " "
				<< ParticleStep::getPrimaryNeutronStoppingTime() << " "
				<< ParticleStep::getPrimaryPositronCosineOfInitialNormalAngle() << " "
				<< ParticleStep::getPrimaryNeutronCosineOfInitialNormalAngle() << endl;
		}
		if(particleStep->getNextParticleStep()->getParticleStepHeaderFlag() == true) break;
		else particleStep = particleStep->getNextParticleStep();
	}
	fout.close();
	return;
}

void deleteParticleSteps(ParticleStep* particleStep){
	ParticleStep* particleStepToDelete;
	while(false == particleStep->getNextParticleStep()->getParticleStepHeaderFlag()){
		particleStepToDelete = particleStep->getNextParticleStep();
		particleStepToDelete->removeParticleStep();
		delete particleStepToDelete;
	}
	delete particleStep;
	return;
}

void setStaticDataForSteps(
		int eventNumber,
		double neutrinoKineticEnergy,
		double neutrinoMomentumUnitVectorX,
		double neutrinoMomentumUnitVectorY,
		double neutrinoMomentumUnitVectorZ,
		int stepNumber,
		int particleID,
		int trackID,
		int parentID,
		double preStepPositionX,
		double preStepPositionY,
		double preStepPositionZ,
		double postStepPositionX,
		double postStepPositionY,
		double postStepPositionZ,
		double preStepGlobalTime,
		double postStepGlobalTime,
		double preStepKineticEnergy,
		double postStepKineticEnergy,
		double totalEnergyDeposit,
		double stepLength,
		double trackLength,
		int processType,
		int processSubType,
		int trackStatus,
		bool particlePostStepPointInScintillatorFlag){
			if(parentID == 0 && trackStatus == 2){ // If particle is a primary particle and of status fKill.
				if(particleID == -11){ // If particle is a positron.
					ParticleStep::setPrimaryPositronStoppingTime(postStepGlobalTime);
					if(particlePostStepPointInScintillatorFlag == 1) ParticleStep::setPrimaryPositronStoppedInScintillatorFlag(true);
				}
				else if(particleID == 2112){ // If particle is a neutron.
					ParticleStep::setPrimaryNeutronStoppingTime(postStepGlobalTime);
					if(particlePostStepPointInScintillatorFlag == 1) ParticleStep::setPrimaryNeutronStoppedInScintillatorFlag(true);
				}
			}
			if(parentID == 0 && stepNumber == 1){ // If particle is a primary particle and is taking its first step.
				if(particleID == -11){
					TVector3 neutrinoMomentumUnitVector(neutrinoMomentumUnitVectorX, neutrinoMomentumUnitVectorY, neutrinoMomentumUnitVectorZ);
					TVector3 positronInitialMomentumDirection(
							postStepPositionX - preStepPositionX,
							postStepPositionY - preStepPositionY,
							postStepPositionZ - preStepPositionZ);
					ParticleStep::setPrimaryPositronCosineOfInitialNormalAngle(positronInitialMomentumDirection.Dot(neutrinoMomentumUnitVector)/(positronInitialMomentumDirection.Mag()*neutrinoMomentumUnitVector.Mag()));
				}
				else if(particleID == 2112){
					TVector3 neutrinoMomentumUnitVector(neutrinoMomentumUnitVectorX, neutrinoMomentumUnitVectorY, neutrinoMomentumUnitVectorZ);
					TVector3 neutronInitialMomentumDirection(
							postStepPositionX - preStepPositionX,
							postStepPositionY - preStepPositionY,
							postStepPositionZ - preStepPositionZ);
					ParticleStep::setPrimaryNeutronCosineOfInitialNormalAngle(neutronInitialMomentumDirection.Dot(neutrinoMomentumUnitVector)/(neutronInitialMomentumDirection.Mag()*neutrinoMomentumUnitVector.Mag()));
				}
			}
			return;
		}

int main(int argc, char** argv){
	cout
		<< endl
		<< "##############################################################"
		<< endl
		<< "# Running MTC output data conversion program. Please wait... #"
		<< endl
		<< "##############################################################"
		<< endl;
	if(argc <= 1){
		cout << "Not enough arguments specified.\n";
		cout << "Please specify the files that your would like to convert after the executable\n";
		cout << "\n\n";
		return 0;
	}
	string initialKineticEnergySettingString;
	string primaryParticleSettingString;
	string enrichmentSettingString;
	string dopingAgentSettingString;
	string dopingFractionSettingString;

		string finString, foutString;
		ifstream fin;
		ofstream fout;
	for(int i = 1; i < argc; i++){
		finString.assign(argv[i]);
		foutString = finString+"_Converted";
		fin.open(finString.data(), ifstream::in);
		if(!fin.good() || fin.bad()){
			cout << "Error opening input file!\n";
			return 0;
		}
		cout
			<< "Converting: "
			<< finString << " -> "
			<< foutString
			<< endl;
		fout.open(foutString.data(), ofstream::trunc);
		fout.close();
		ParticleStep* particleStepHeaderNode = new ParticleStep();
		int previousEventNumber = -1;
		int eventNumber; // Initialize event number.
		double neutrinoKineticEnergy;
		double
			neutrinoMomentumUnitVectorX,
			neutrinoMomentumUnitVectorY,
			neutrinoMomentumUnitVectorZ;
		int stepNumber;
		int trackID;
		int parentID;
		string particleName;
		int particleID;
		double
			preStepGlobalTime,
			preStepPositionX,
			preStepPositionY,
			preStepPositionZ;
		double
			postStepGlobalTime,
			postStepPositionX,
			postStepPositionY,
			postStepPositionZ;
		double
			preStepKineticEnergy,
			postStepKineticEnergy;
		double totalEnergyDeposit;
		double stepLength;
		double trackLength;
		string processName;
		int processType;
		int processSubType;
		int trackStatus;
		string boundaryStatusName;
		string isBetweenScintillatorAndSensitivePixel;
		bool particlePostStepPointInScintillatorFlag;
		double
			primaryNeutronStoppingTime,
			primaryPositronStoppingTime;
		double
			primaryNeutronCosineOfInitialNormalAngle,
			primaryPositronCosineOfInitialNormalAngle;

		char bufferChar[1024];
		while(fin.getline(bufferChar, 1024)){
			string buffer = bufferChar;
			// If first entry is digit.
			if(!isdigit(buffer[buffer.find_first_not_of("		")])){
				// If line is comment starting with "#" character.
				if(buffer[buffer.find_first_not_of("	 ")] == '#'){
					//fout.open(foutString.data(), ofstream::out | ofstream::app);
					fout.open(foutString.data(), ofstream::out | ofstream::app);
					string header = "# EventNumber NeutrinoKineticEnergy NeutrinoMomentumUnitVectorX NeutrinoMomentumUnitVectorY NeutrinoMomentumUnitVectorZ StepNumber ParticleID TrackID ParentID PreStepPositionX PreStepPositionY PreStepPositionZ PostStepPositionX PostStepPositionY PostStepPositionZ PreStepGlobalTime PostStepGlobalTime PreStepKineticEnergy PostStepKineticEnergy TotalEnergyDeposit StepLength TrackLength ProcessType ProcessSubType TrackStatus BoundaryStatusName IsBetweenScintillatorAndSensitivePixel ParticlePostStepPointInScintillatorFlag PrimaryPositronStoppedInScintillatorFlag PrimaryNeutronStoppedInScintillatorFlag PrimaryPositronStoppingTime PrimaryNeutronStoppingTime PrimaryPositronCosineOfInitialNormalAngle PrimaryNeutronCosineOfInitialNormalAngle";
					fout << header << endl;
					fout.close();
					continue;
				}
				else continue;
			}
			istringstream inputStream(buffer, istringstream::in);
			if(!(
						inputStream
						>> eventNumber
						>> neutrinoKineticEnergy
						>> neutrinoMomentumUnitVectorX
						>> neutrinoMomentumUnitVectorY
						>> neutrinoMomentumUnitVectorZ
						>> stepNumber
						>> particleName
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
						>> processName
						>> processType
						>> processSubType
						>> trackStatus
						>> boundaryStatusName
						>> isBetweenScintillatorAndSensitivePixel
						>> particlePostStepPointInScintillatorFlag))
						{
							cout << finString
								<< " cannot be read in properly.\n"
								<< "Please check formatting.\n";
							return 0;
						}
			else{
				/*
				cout
					<< eventNumber << " "
					<< stepNumber << " "
					<< particleID << endl;
					*/
				if(eventNumber != previousEventNumber || fin.eof()){
					if(previousEventNumber >= 0 && particleStepHeaderNode){
						outputParticleSteps(particleStepHeaderNode, foutString);
						deleteParticleSteps(particleStepHeaderNode);
					}
					if(fin.eof()) break;
					particleStepHeaderNode = new ParticleStep();
				}
				setStaticDataForSteps(
						eventNumber,
						neutrinoKineticEnergy,
						neutrinoMomentumUnitVectorX,
						neutrinoMomentumUnitVectorY,
						neutrinoMomentumUnitVectorZ,
						stepNumber,
						particleID,
						trackID,
						parentID,
						preStepPositionX,
						preStepPositionY,
						preStepPositionZ,
						postStepPositionX,
						postStepPositionY,
						postStepPositionZ,
						preStepGlobalTime,
						postStepGlobalTime,
						preStepKineticEnergy,
						postStepKineticEnergy,
						totalEnergyDeposit,
						stepLength,
						trackLength,
						processType,
						processSubType,
						trackStatus,
						particlePostStepPointInScintillatorFlag);
				particleStepHeaderNode->insertParticleStep(
						new ParticleStep(
							eventNumber,
							neutrinoKineticEnergy,
							neutrinoMomentumUnitVectorX,
							neutrinoMomentumUnitVectorY,
							neutrinoMomentumUnitVectorZ,
							stepNumber,
							particleID,
							trackID,
							parentID,
							preStepPositionX,
							preStepPositionY,
							preStepPositionZ,
							postStepPositionX,
							postStepPositionY,
							postStepPositionZ,
							preStepGlobalTime,
							postStepGlobalTime,
							preStepKineticEnergy,
							postStepKineticEnergy,
							totalEnergyDeposit,
							stepLength,
							trackLength,
							processType,
							processSubType,
							trackStatus,
							boundaryStatusName,
							isBetweenScintillatorAndSensitivePixel,
							particlePostStepPointInScintillatorFlag));
				previousEventNumber = eventNumber;
			}
		}
		fin.close();
		cout
			<< "Done."
			<< endl
			<< endl;
	}
	cout
		<< "Conversion program terminated successfully."
		<< endl
		<< endl
		<< endl;
	return 0;
}
