#ifndef LIBMTC_SDS
#define LIBMTC_SDS

//Root
#include "TFile.h"
#include "TTree.h"

//STL
#include <string>
#include <vector>

//Internal
#include "sds-structures.hpp"

namespace mtc
{
namespace sds
{

/** \brief This class simplifies dealing with the mTC Shared Data Format.
 */
class SDS
{
public:

    /**
     * Prefix for the branch name which stores pixel number.
     */
    static const std::string PID_BRANCH_PREFIX;

    /** \brief Simple constructor.
     * \param file filename of the SDS file,
     */
	SDS(std::string file);


    /** \brief Open the SDS file.
     * \return True on success, false otherwise.
     */
	bool open();

    /** \brief Check if the file is open.
     * \return True if it is, false otherwise.
     */
	bool isOpen();


    /** \brief Sync buffers to file.
     * This is done automatically when the file is closed.
     */
	void sync();


    /** \brief Close the file.
     * \return Bool true on success, false otherwise.
     */
	bool close();


    /** \brief Returns the names of runs stored in this file.
     * \return std::vector < std::string> container of run names.
     */
	std::vector < std::string > runs();


    /** \brief Gets the descriptor for the run.
     *  \param run Name of the run.
     *  \return Descriptor for the run.
     */
	struct RunDescriptorStruct runDesriptor(std::string run);

    /** \brief Get the list of event descriptors in this run.
     *
     * \param run Run name.
     * \return Container of event descriptors.
     */
	std::vector < struct EventDescriptorStruct > events(std::string run);


    /** \brief Get all available pixels from an event.
     *
     * \param run  Run to query.
     * \param event Event to query.
     * \return Container of available pixel descriptors.
     */
	std::vector < int > pixels(std::string run, struct EventDescriptorStruct event);


    /** \brief Return pixel data from a specified event and run.
     *
     * \param run Run to query.
     * \param event Event to query.
     * \param pixel Pixel to query.
     * \return Container of pixel data.
     */
	std::vector < float > getPixel(std::string run,  struct EventDescriptorStruct event, int pixel);

    /** \brief Add a new run to the file.
     *
     * \param run Descriptor for the run.
     * \return True on success, false otherwise.
     */
	bool addRun(struct RunDescriptorStruct run);

    /** \brief Add an Event to a run.
     *
     * \param run run to query.
     * \param event Event to query.
     * \return True on success, false otherwise.
     */
	bool addEvent(std::string run, struct EventDescriptorStruct event);


    /** \brief add a pixel to an event.
     *
     * \param run Run name.
     * \param event Event descriptor.
     * \param pixel Pixel id.
     * \param pixelData Container of pixel data.
     * \return bool
     */
	bool addPixel(std::string run, struct EventDescriptorStruct event, int pixel, std::vector<float> pixelData);

private:
	std::string fname_;
	bool open_;
	TFile *treeFile_;
	
	//Buffering
	std::string lastRun_;
	TTree *lastRunTree_;

	std::string lastEvent_;
	TBranch *lastPidBranch_;
	TBranch *lastDataBranch_;
	
	bool updateNeeded_;
};
}
}

#endif


