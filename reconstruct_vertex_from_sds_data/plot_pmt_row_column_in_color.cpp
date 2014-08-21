#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TH3D.h>
#include <TPad.h>
#include <TLegend.h>

#include "libmtc-sds.hpp"
#include "TFile.h"

int main(int argc, char **argv)
{
	std::cout << "Starting extraction.\n";
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

	std::vector<TH3D> h(24,
			TH3D("h", "h", 65, -70, 70, 65, -70, 70, 65, -70, 70));
	double x, y, z;
	int pixelID;

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
						pixelID = pixel.at(i_pixel);
						fout << pixelID << std::endl;
						int column = pixelID % 10;
						pixelID /= 10;
						int row = pixelID % 10;
						pixelID /= 10;
						int pmtID = pixelID;
						fout << "pmtID: " << pmtID << std::endl;
						fout << "row: " << row << std::endl;
						fout << "column: " << column << std::endl;
						fout << data.at(i_data) << "\t";
						fout << data.at(++i_data) << std::endl;
						x = data.at(++i_data);
						y = data.at(++i_data);
						z = data.at(++i_data);
						++i_data;
						if ( (x > 65 || y > 65 || z < -64) ) continue;

						fout << "x: " << x << std::endl;
						fout << "y: " << y << std::endl;
						fout << "z: " << z << std::endl;
						//int binID = h[row-1].FindBin(x, y, z);
						//h[row-1].SetBinContent(binID, 1);
						//int binID = h[column-1].FindBin(x, y, z);
						//h[column-1].SetBinContent(binID, 1);
						int binID = h[pmtID-1].FindBin(x, y, z);
						h[pmtID-1].SetBinContent(binID, 1);
					}
				}
			}
		}
		test.close();
	}
	fout.close();

	for (int i = 0; i < 24; ++i) {
		h[i].SetMarkerStyle(10);
		h[i].SetMarkerSize(1);
	}

	h[0].SetMarkerColor(kRed);
	h[1].SetMarkerColor(kYellow);
	h[2].SetMarkerColor(kGreen);
	h[3].SetMarkerColor(kCyan);
	h[4].SetMarkerColor(kBlue);
	h[5].SetMarkerColor(kMagenta);
	h[6].SetMarkerColor(kRed+2);
	h[7].SetMarkerColor(kYellow+2);
	h[8].SetMarkerColor(kGreen+2);
	h[9].SetMarkerColor(kCyan+2);
	h[10].SetMarkerColor(kBlue+2);
	h[11].SetMarkerColor(kMagenta+2);
	h[12].SetMarkerColor(kRed-1);
	h[13].SetMarkerColor(kYellow-1);
	h[14].SetMarkerColor(kGreen-1);
	h[15].SetMarkerColor(kCyan-1);
	h[16].SetMarkerColor(kBlue-1);
	h[17].SetMarkerColor(kMagenta-1);
	h[18].SetMarkerColor(kRed-7);
	h[19].SetMarkerColor(kYellow-7);
	h[20].SetMarkerColor(kGreen-7);
	h[21].SetMarkerColor(kCyan-7);
	h[22].SetMarkerColor(kBlue-7);
	h[23].SetMarkerColor(kMagenta-7);
	h[0].Draw("box");
	h[1].Draw("box same");
	h[2].Draw("box same");
	h[3].Draw("box same");
	h[4].Draw("box same");
	h[5].Draw("box same");
	h[6].Draw("box same");
	h[7].Draw("box same");
	h[8].Draw("box same");
	h[9].Draw("box same");
	h[10].Draw("box same");
	h[11].Draw("box same");
	h[12].Draw("box same");
	h[13].Draw("box same");
	h[14].Draw("box same");
	h[15].Draw("box same");
	h[16].Draw("box same");
	h[17].Draw("box same");
	h[18].Draw("box same");
	h[19].Draw("box same");
	h[20].Draw("box same");
	h[21].Draw("box same");
	h[22].Draw("box same");
	h[23].Draw("box same");

	TLegend l(0.9, 0.0, 1.0, 0.5);
	for (int iPmt = 0; iPmt < 24; ++iPmt) {
		std::ostringstream oss;
		oss << iPmt+1;
		l.AddEntry(&h[iPmt], ("PMT " + oss.str()).c_str(), "p");
	}
	//for (int iRow = 0; iRow < 8; ++iRow) {
	//	std::ostringstream oss;
	//	oss << iRow+1;
	//	l.AddEntry(&h[iRow], ("Row " + oss.str()).c_str(), "p");
	//}
	//for (int iCol = 0; iCol < 8; ++iCol) {
	//	std::ostringstream oss;
	//	oss << iCol+1;
	//	l.AddEntry(&h[iCol], ("Col " + oss.str()).c_str(), "p");
	//}
	l.Draw("same");
	gPad->Update();

	gPad->SaveAs("test.eps");
	std::cout << "Done.\n";

	return 0;
}
