#include "libmtc-sds.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace mtc::sds;
using namespace std;

void functionalityTest()
{
	struct RunDescriptorStruct run;
    run.runName = "test";
    run.type = "cosmic";
    run.initials = "SIN";
    run.time.sec = 12345566;
    run.time.nsec = 1234123;
    run.time.psec = 321;
    run.location = "Dentist";
    run.temperature = "31C";
    run.comments = "This is lies. There is no data here.";

    struct EventDescriptorStruct event;
    event.number = 10;
    event.firstHitTime = run.time;
    event.firstHitTime.nsec += 10;

    unlink("test.root");

    SDS test("test.root");
    test.open();
    test.addRun(run);

    cout << "Created Run" << endl;
    std::vector <string> runs = test.runs();
    for(unsigned int i = 0; i< runs.size(); i++)
    {
        run = test.runDesriptor(runs[i]);
        cout << run.runName << endl;
        cout << "\t" << run.type << endl;
        cout << "\t" << run.initials << endl;
        cout << "\t" << run.time.sec <<  "." << run.time.nsec << "." << run.time.psec << endl;
        cout << "\t" << run.location << endl;
        cout << "\t" << run.temperature << endl;
        cout << "\t" << run.comments << endl;
    }
    cout << "Created Two Events" << endl;

    test.addEvent(run.runName, event);
    event.number += 1;
    test.addEvent(run.runName, event);

    vector <struct EventDescriptorStruct> events = test.events(run.runName);
    for(unsigned int i = 0; i< events.size(); i++)
    {
        event = events[i];
        cout << event.number << endl;
        cout << "\t" << event.firstHitTime.sec <<  "." << event.firstHitTime.nsec << "." << event.firstHitTime.psec << endl;
    }
    cout << "Adding pixel data to last event" << endl;
    vector<float> data;
    srand ( time(NULL) );
    for(int i = 0; i< 10; i++)
    {
        data.push_back(100 - rand()%100);
        cout << i << " : " << data.back() << endl;
    }
    test.addPixel(run.runName, event,1010,data);
    cout << "Reading back pixel data" << endl;
    data.clear();
    data = test.getPixel(run.runName, event, 1010);
    for(unsigned int i = 0; i< data.size(); i++)
    {
        cout << i << " : " << data[i] << endl;
    }
    cout << "Reading back pixel data for non existent pixel" << endl;
    data.clear();
    data = test.getPixel(run.runName, event, 1011);
    for(unsigned int i = 0; i< data.size(); i++)
    {
        cout << i << " : " << data[i] << endl;
    }
    test.close();
}

void benchmarkTest()
{
		struct RunDescriptorStruct run;
    run.runName = "test";
    run.type = "cosmic";
    run.initials = "SIN";
    run.time.sec = 12345566;
    run.time.nsec = 1234123;
    run.time.psec = 321;
    run.location = "Dentist";
    run.temperature = "31C";
    run.comments = "This is lies. There is no data here.";

    struct EventDescriptorStruct event;
    event.number = 10;
    event.firstHitTime = run.time;
    event.firstHitTime.nsec += 10;

    vector<float> data;
    srand ( time(NULL) );
    for(int i = 0; i< 10; i++)
    {
        data.push_back(100 - rand()%100);
    }

    unlink("test.root");

    SDS test("test.root");
    test.open();
	test.addRun(run);
    for(int i = 0; i< 100; i++)
    {
		event.number++;
		test.addEvent(run.runName, event);
		for(int j = 0; j < 1536; j++)
		{
			test.addPixel(run.runName, event,j,data);
		}
	}
    for(int i = 0; i< 100; i++)
    {
		for(int j = 0; j < 1536; j++)
		{
			test.getPixel(run.runName,event,j);
		}
		event.number--;

	}	
    test.close();
}

int main()
{
	functionalityTest();
	cout << "Beggining benchmark" << endl;
	benchmarkTest();
}
