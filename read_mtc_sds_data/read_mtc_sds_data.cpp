#include <vector>
#include <iostream>
#include <fstream>
#include "libmtc-sds.hpp"
#include "TFile.h"

int main(int argc, char **argv)
{
	struct mtc::sds::RunDescriptorStruct run; // Run descriptor.
    struct mtc::sds::EventDescriptorStruct event; // Event descriptor.
	std::vector<std::string> list_of_runs; // List of run names.
	std::vector<mtc::sds::EventDescriptorStruct> list_of_events; // list of event descriptors.

	/*
	 * PMT pixel ID number = 100*<PMT ID#> + 10*<anode row#> + <anode column#>
	 * 
	 */
	std::vector<int> pixel; // PMT pixel ID number.
	std::vector<float> data; // List of number of photoelectrons and hittimes.

	if(argc <= 1){
		std::cerr << "Execution format: read_mtc_root_data <input files>\n" <<
			std::endl;
		return 0;
	}
	ofstream // Output stream to file.
		fout("pe_converted_from_root_file_to_text_file.txt", ofstream::out);
	// Header of output file.
	fout << "# run_name\tevent_number\tpixel_id hit_time(ns)\tnumber_of_pe\n";

	for(int i_arg = 1; i_arg < argc; i_arg++){ // Iterate over input root files.
		mtc::sds::SDS test(argv[i_arg]);
		test.open();
		list_of_runs = test.runs();
		for(int i_run = 0; i_run < list_of_runs.size(); i_run++){
			std::string run_name = list_of_runs.at(i_run);
			list_of_events = test.events(run_name);
			for(int i_event = 0; i_event < list_of_events.size(); i_event++){
				event = list_of_events.at(i_event);
				pixel = test.pixels(run_name, event);
				for(int i_pixel = 0; i_pixel < pixel.size(); i_pixel++){
					data = test.getPixel(run_name, event, pixel.at(i_pixel));
					int i_data = 0;
					while(i_data < data.size()){
						fout << run_name << "\t";
						fout << event.number << "\t";
						fout << pixel.at(i_pixel) << "\t";
						fout << data.at(i_data) << "\t";
						fout << data.at(++i_data) << std::endl;
						++i_data;
					}
				}
			}
		}
		test.close();
	}
	fout.close();
}
