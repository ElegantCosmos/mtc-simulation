#ifndef MTCG4_COLLECTION_OF_NEUTRON_STEPS_HH
#define MTCG4_COLLECTION_OF_NEUTRON_STEPS_HH 1

class MTCG4PrimaryNeutronStep;

class MTCG4CollectionOfPrimaryNeutronSteps{
	public:
		MTCG4CollectionOfPrimaryNeutronSteps(MTCG4PrimaryNeutronStep* primaryNeutronStep = 0, MTCG4CollectionOfPrimaryNeutronSteps* nextCollectionOfPrimaryNeutronSteps = 0, G4int newTotalNumberOfSteps = 0)
		: primaryNeutronStep(primaryNeutronStep), nextPointerToPrimaryNeutronStep(nextCollectionOfPrimaryNeutronSteps)
		{
			if(maxTotalNumberOfSteps < newTotalNumberOfSteps) maxTotalNumberOfSteps = newTotalNumberOfSteps;
		}
		MTCG4PrimaryNeutronStep* GetPrimaryNeutronStep(){ return primaryNeutronStep; }
		MTCG4CollectionOfPrimaryNeutronSteps* GetNextCollectionOfPrimaryNeutronSteps(){ return nextPointerToPrimaryNeutronStep; }
		static G4int GetMaxTotalNumberOfSteps(){ return maxTotalNumberOfSteps; }
		static G4int maxTotalNumberOfSteps;

	private:
		MTCG4PrimaryNeutronStep* primaryNeutronStep;
		MTCG4CollectionOfPrimaryNeutronSteps* nextPointerToPrimaryNeutronStep;
};

#endif
