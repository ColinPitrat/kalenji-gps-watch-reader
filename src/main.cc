#include <iostream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <map>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <iterator>

#include "source/Logger.h"
#include "source/USB.h"
#include "source/File.h"
#include "source/HexdumpFile.h"
#include "bom/Session.h"
#include "device/Device.h"
#include "filter/Filter.h"
#include "output/Output.h"
#include "Registry.h"
#include "Utils.h"

#ifndef DEBUG
#define DEBUG 0
#endif

std::map<std::string, std::string> configuration;

std::list<std::string> splitString(std::string toSplit, std::string separator = ",")
{
    std::list<std::string> result;
    size_t begin = 0;
    size_t end = 0;
    while(end != std::string::npos)
    {
        end = toSplit.find(separator, begin);
        std::string value = toSplit.substr(begin, end-begin);
        begin = end + separator.length();
	// none is a special value to specify empty lists
	if(value != "none")
	{
		result.push_back(value);
	}
    }
    return result;
}

int testDir(std::string path, bool create_if_not_exist)
{
	if(access(path.c_str(), W_OK) != 0)
	{
		switch(errno)
		{
			case ENOENT:
				if(create_if_not_exist)
				{
					mkdir(path.c_str(), 0777);
					return 1;
				}
				else
				{
					std::cerr << "Error: " << path << " doesn't exist and I couldn't create it" << std::endl;
					return -1;
				}
			case EACCES:
			case EROFS:
				std::cerr << "Error: Don't have write access in " << path << std::endl;
				return -1;
			case ENOTDIR:
				std::cerr << "Error: " << path << " is not a directory" << std::endl;
				return -1;
			default:
				std::cerr << "Error: Unknown reason (" << errno << ") preventing write access to " << path << std::endl;
				return -1;
		}
	}
	return 0;
}

bool checkAndCreateDir(std::string path)
{
	int dir_status = testDir(path, true);
	// If dir was tentatively created, second attempt
	if(dir_status > 0) dir_status = testDir(path, false);
	if(dir_status < 0) return false;
	return true;
}

void usage(char *progname)
{
	std::cout << "Usage: " << progname << " [ -h | [ -c <rc_file> ] [ -d <output_directory> ] [ -f <filters> ] [-D <device> ] [ -i <input_file> ] [ -o <outputs> ] [ -t <trigger_type> ] ]" << std::endl;
	std::cout << "  - h: help:        Show this help message " << std::endl;
	std::cout << "  - c: conf file:   Provide alternate configuration file instead of ~/.kalenji_readerrc" << std::endl;
	std::cout << "  - d: output dir:  Directory to which output files should be produced" << std::endl;
	std::cout << "  - f: filters:     Comma separated list of filters to apply on data before the export. Use 'none' for empty list" << std::endl;
	std::cout << "  - D: device:      Type of device to use (e.g: GPX, Kalenji)" << std::endl;
	std::cout << "  - i: input file:  Provide input file instead of reading from device" << std::endl;
	std::cout << "  - o: outputs:     Comma separated list of output formats to produce for each session." << std::endl;
	std::cout << "  - t: trigger:     Override the type of trigger (possible values: manual, distance, time, location, hr)" << std::endl;
}

std::map<std::string, std::string> readOptions(int argc, char **argv)
{
	std::map<std::string, std::string> options;
	int option;
	while((option = getopt(argc, argv, ":c:d:f:D:i:o:t:h")) != -1)
	{
		switch(option)
		{
			case 'c':
				options["rcfile"] = std::string(optarg);
				break;
			case 'd':
				options["directory"] = std::string(optarg);
				break;
			case 'f':
				options["filters"] = std::string(optarg);
				break;
			case 'D':
				options["device"] = std::string(optarg);
				break;
			case 'i':
				options["source"] = "File";
				options["sourcefile"] = std::string(optarg);
				break;
			case 'o':
				options["outputs"] = std::string(optarg);
				break;
			case 't':
				options["trigger"] = std::string(optarg);
				break;
			case 'h':
				usage(argv[0]);
				exit(0);
			case '?':
				std::cerr << " Error, unknown option " << (char)optopt << std::endl;
				usage(argv[0]);
				exit(-1);
		}
	}
	return options;
}

bool readConf()
{
	// Default conf
	configuration["directory"] = "/tmp/kalenji_import";
	configuration["import"] = "all";
	configuration["trigger"] = "manual";
	configuration["log_transactions"] = "yes";
	configuration["source"] = "USB";
	configuration["device"] = "Kalenji";
	configuration["filters"] = "UnreliablePoints,EmptyLaps";
	configuration["outputs"] = "GPX,GoogleMap";
	// Default value for log_transactions_directory is defined later (depends on directory)
	// TODO: Check that content of file is correct (i.e key is already in the map, except for log_transactions_directory that we define later if given ?)

	// Load from ~/.kalenji_readerrc
	char *homeDir = getenv("HOME");
	if(!homeDir)
	{
		std::cerr << "No home dir found ! This is strange ... Why would $HOME not be set for your user ?" << std::endl;
		return false;
	}

	std::string rcfile = std::string(homeDir) + "/.kalenji_readerrc";
	if(configuration.count("rcfile") != 0)
	{
		rcfile = configuration["rcfile"];
	}
	if(access(rcfile.c_str(), F_OK) == 0)
	{
		std::string line;
		std::ifstream conf_file(rcfile.c_str());
		if (conf_file.is_open())
		{
			while ( conf_file.good() )
			{
				getline(conf_file, line);
				size_t cut_place = line.find("=");
				if(cut_place != std::string::npos)
				{
					std::string key = line.substr(0, cut_place);
					std::string value = line.substr(cut_place+1);
					configuration[key] = value;
				}
			}
			conf_file.close();
		}
		else 
		{
			std::cerr << "Unable to open " << rcfile; 
			return false;
		}
	}
	return true;
}

bool parseConfAndOptions(int argc, char** argv)
{
	std::map<std::string, std::string> options = readOptions(argc, argv);
	readConf();
	// Now override configuration with options given by the users
	for(std::map<std::string, std::string>::iterator it = options.begin(); it != options.end(); ++it)
	{
		configuration[it->first] = it->second;
	}

	// Some configuration adaptation ... 
	// TODO: Cleaner way to handle it ?
	if(configuration.count("log_transactions_directory") == 0)
	{
		configuration["log_transactions_directory"] = configuration["directory"] + "/logs";
	}
	if(configuration["source"] == "File")
	{
		// When using a file as input, we don't want the user to be prompted as we read everything and ignore all sending
		configuration["import"] = "all";
	}
	if(configuration["source"] == "File" && configuration["device"] == "Kalenji")
	{
		configuration["source"] = "HexdumpFile";
	}
	return true;
}

void filterSessionsToImport(SessionsMap *sessions)
{
	std::string to_import_string;
	if(configuration["import"] == "ask")
	{
		// Display sessions that can be imported, prompt for list of sessions to import
		std::cout << "Sessions available for import:" << std::endl;
		for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); ++it)
		{
			// TODO: Use a Session method instead !
			std::cout << std::setw(5) << it->second.getNum() << " - " << it->second.getBeginTime(true); 
			std::cout << " " << std::setw(5) << it->second.getNbLaps() << " laps ";
			std::cout << std::setw(10) << (double)it->second.getDistance() / 1000 << " km ";
			std::cout << std::setw(15) << durationAsString(it->second.getDuration()) << std::endl;
		}
		std::cout << "List of sessions to import (space separated - 'all' to import everything): " << std::endl;

/*
		// It's a pity, but this doesn't work (only stops after a wrong entry)
		std::copy(std::istream_iterator<uint32_t>(std::cin), std::istream_iterator<uint32_t>(), std::back_inserter(to_import));
*/
		getline(std::cin, to_import_string);
	}
	else
	{
		to_import_string = configuration["import"];
	}

	if(to_import_string != "all")
	{
		std::vector<uint32_t> to_import;
		std::stringstream iss(to_import_string);
		std::copy(std::istream_iterator<uint32_t>(iss), std::istream_iterator<uint32_t>(), std::back_inserter(to_import));

		// TODO: Check for error in user entry. Re-ask if there is one !

		// Remove sessions that are not in the list of selected sessions
		for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); )
		{
			bool keep = false;
			for(std::vector<uint32_t>::iterator it2 = to_import.begin(); it2 != to_import.end(); ++it2)
			{
				if(it->second.getNum() == *it2)
				{
					keep = true;
				}
			}
			if(!keep) sessions->erase(it++);
			else ++it;
		}
	}
}

int main(int argc, char *argv[])
{
	if(!parseConfAndOptions(argc, argv)) return -1;

	// First attempt, creating dir if it doesn't exist
	if(!checkAndCreateDir(configuration["directory"])) return -1;

	// TODO: Use registry for source too 
	source::Source *dataSource;
	if(configuration["source"] == "File")
	{
		dataSource = new source::File(configuration["sourcefile"]);
	}
	else if(configuration["source"] == "HexdumpFile")
	{
		dataSource = new source::HexdumpFile(configuration["sourcefile"]);
	}
	else
	{
		dataSource = new source::USB();
		if(configuration["log_transactions"] == "yes")
		{
			if(!checkAndCreateDir(configuration["log_transactions_directory"])) return -1;

			// Create log file name 
			// TODO: Improve ?
			char buffer[256];
			time_t t = time(NULL);
			strftime(buffer, 256, "%Y%m%d_%H%M%S", localtime(&t));
			std::stringstream log_filename;
			log_filename << configuration["log_transactions_directory"] << "/" << "kalenji_reader_" << buffer << ".log";

			dataSource = new source::Logger(dataSource, log_filename.str());
		}
	}

	device::Device *myDevice = LayerRegistry<device::Device>::getInstance()->getObject(configuration["device"]);
	myDevice->setSource(dataSource);
	myDevice->init();
	SessionsMap sessions;
	myDevice->getSessionsList(&sessions);

	// If import = ask, prompt the user for sessions to import. 
	// TODO: could filter already imported sessions (for example import = new)
	// TODO: also prompt here for trigger type (and other info not found in the watch ?). This means at session level instead of global but could also be at lap level !
	filterSessionsToImport(&sessions);

	myDevice->getSessionsDetails(&sessions);
	// TODO: in case of error we should finish reading until getting 8Axxx so that the device is in a clean state

	std::list<std::string> filters = splitString(configuration["filters"]);
	std::list<std::string> outputs = splitString(configuration["outputs"]);

	for(SessionsMap::iterator it = sessions.begin(); it != sessions.end(); ++it)
	{
		for(std::list<std::string>::iterator it2 = filters.begin(); it2 != filters.end(); ++it2)
		{
			filter::Filter *filter = LayerRegistry<filter::Filter>::getInstance()->getObject(*it2);
			if(filter)
			{
				filter->filter(&(it->second));
			}
			else
			{
				std::cout << "Filter does not exist: " << *it2 << std::endl;
			}
		}
		for(std::list<std::string>::iterator it2 = outputs.begin(); it2 != outputs.end(); ++it2)
		{
			output::Output *output = LayerRegistry<output::Output>::getInstance()->getObject(*it2);
			if(output)
			{
				output->dump(&(it->second), configuration);
			}
			else
			{
				std::cout << "Output does not exist: " << *it2 << std::endl;
			}
		}
	}

	sessions.clear();

	dataSource->release();
	delete dataSource;
	myDevice->release();
	delete myDevice;

	return 0;
}
