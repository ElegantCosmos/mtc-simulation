#ifndef MTC_SDS_STRUCTURES_HEADER
#define MTC_SDS_STRUCTURES_HEADER

#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace mtc
{
namespace sds
{

template <class T> inline int utill_to_int (const T& t)
{
    int out;
    std::stringstream ss(t);
    ss >> out;
    return out;
}

inline std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

struct TimeStruct
{
    long	sec;
    long	nsec;
    long	psec;
};

struct RunDescriptorStruct
{
    std::string runName;
    std::string type;
    std::string initials;
    struct TimeStruct time;
    std::string location;
    std::string temperature;
    std::string comments;
};

struct EventDescriptorStruct
{
    int number;
    struct TimeStruct firstHitTime;
};

inline std::string serializeTime(struct TimeStruct item)
{
    std::stringstream ss;
    ss << item.sec << "." << item.nsec << "." << item.psec;
    return ss.str();
}

inline struct TimeStruct deserializeTime(std::string item)
{
    TimeStruct ret;
    std::vector <std::string > fields = split(item,'.');
	try {
		if(fields.size() < 3)
        throw "TimeStruct does not have enough fields!";
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
	}
    ret.sec = utill_to_int(fields[0]);
    ret.nsec = utill_to_int(fields[1]);
    ret.psec = utill_to_int(fields[2]);
    return ret;
}


inline std::string serializeRunDescriptor(struct RunDescriptorStruct run)
{
    std::stringstream ss;
    ss << run.type << ";" << run.initials << ";" << serializeTime(run.time) << ";";
    ss << run.location << ";" << run.temperature << ";" << run.comments;

    return ss.str();
}

inline struct RunDescriptorStruct deserializeRunDescriptor(std::string name, std::string title)
{
    struct RunDescriptorStruct ret;
    ret.runName = name;
    std::vector<std::string> fields = split(title,';');
	try {
		if(fields.size() < 6)
        throw "RunDescriptorStruct does not have enough fields!";
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
	}
    ret.type = fields[0];
    ret.initials = fields[1];
    ret.time = deserializeTime(fields[2]);
    ret.location = fields[3];
    ret.temperature = fields[4];
    ret.comments = fields[5];

    return ret;
}

inline std::string serializeEventDescriptor(struct EventDescriptorStruct item)
{
    std::stringstream ss;
    ss << item.number << ";" << serializeTime(item.firstHitTime);
    return ss.str();
}

inline struct EventDescriptorStruct deserializeEventDescriptor(std::string item)
{
    struct EventDescriptorStruct ret;
    std::vector<std::string> fields = split(item,';');
	try {
		if(fields.size() < 2)
        throw "EventDescriptorStruct does not have enough fields!";
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
	}
    ret.number = utill_to_int(fields[0]);
    ret.firstHitTime = deserializeTime(fields[1]);
    return ret;
}
}
}

#endif
