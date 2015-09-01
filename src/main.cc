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
#include <iostream>
#include <unistd.h>

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

#define LOG_VERBOSE(x) if(configuration["verbose"] == "true") { std::cout << __FILE__ << ":" << __LINE__ << ": " << x << std::endl; };

std::map<std::string, std::string> configuration;

int testDir(std::string path, bool create_if_not_exist)
{
	if(access(path.c_str(), W_OK) != 0)
	{
		switch(errno)
		{
			case ENOENT:
				if(create_if_not_exist)
				{
					#ifdef WINDOWS
					mkdir(path.c_str());
					#else
					mkdir(path.c_str(), 0777);
					#endif
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
	std::cout << "Usage: " << progname << " [ -h | [ -v ] [ -c <rc_file> ] [ -d <output_directory> ] [ -f <filters> ] [-D <device> ] [ -i <input_file> ] [ -o <outputs> ] [ -t <trigger_type> ] ]" << std::endl;
	std::cout << "  - h: help:        Show this help message " << std::endl;
	std::cout << "  - v: verbose:     Print some debug messages " << std::endl;
	std::cout << "  - c: conf file:   Provide alternate configuration file instead of ~/.kalenji_readerrc" << std::endl;
	std::cout << "  - d: output dir:  Directory to which output files should be produced" << std::endl;
	std::cout << "  - f: filters:     Comma separated list of filters to apply on data before the export. Use 'none' for empty list" << std::endl;
	std::cout << "  - D: device:      Type of device to use (e.g: GPX, Kalenji, OnMove710)" << std::endl;
	std::cout << "  - p: path:        Folder path for file-based access device (mass-storage device like OnMove710)" << std::endl;
	std::cout << "  - i: input file:  Provide input file instead of reading from device" << std::endl;
	std::cout << "  - o: outputs:     Comma separated list of output formats to produce for each session." << std::endl;
	std::cout << "  - t: trigger:     Override the type of trigger (possible values: manual, distance, time, location, hr)" << std::endl;
}

std::map<std::string, std::string> readOptions(int argc, char **argv)
{
	std::map<std::string, std::string> options;
	int option;
	while((option = getopt(argc, argv, ":c:d:f:D:p:i:o:t:vh")) != -1)
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
			case 'p':
				// UGLY:
				// Issue 43: if source is File it means -i option was used
				// In this case, the -p option is used to specify output directory, not input !
				if(options["source"] != "File")
				{
					options["source"] = "Path";
				}
				options["path"] = std::string(optarg);
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
			case 'v':
				options["verbose"] = "true";
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

bool readConf(std::map<std::string, std::string>& options)
{
	// Default conf
	#ifdef WINDOWS
	configuration["directory"] = "c:\\tmp\\kalenji_import";
	#else
	configuration["directory"] = "/tmp/kalenji_import";
	#endif
	configuration["import"] = "new";
	configuration["trigger"] = "manual";
	configuration["log_transactions"] = "yes";
	configuration["source"] = "USB";
	configuration["device"] = "auto";
	configuration["filters"] = "UnreliablePoints,EmptyLaps";
	configuration["outputs"] = "GPX,GoogleMap";
	configuration["output_name"] = "date";
	configuration["gpx_extensions"] = "gpxdata";
	configuration["tcx_sport"] = "Running";
	configuration["reduce_points_max"] = "200";
	configuration["verbose"] = "false";
	// Default value for log_transactions_directory is defined later (depends on directory)
	// TODO: Check that content of file is correct (i.e key is already in the map, except for log_transactions_directory that we define later if given ?)

	// Load from ~/.kalenji_readerrc
	#ifdef WINDOWS
	char *homeDir = getenv("USERPROFILE");
	#else
	char *homeDir = getenv("HOME");
	#endif
	if(!homeDir)
	{
		std::cerr << "No home dir found ! This is strange ... Why would $HOME not be set for your user ?" << std::endl;
		return false;
	}

	#ifdef WINDOWS
	std::string rcfile = std::string(homeDir) + "\\kalenji_reader.conf";
	#else
	std::string rcfile = std::string(homeDir) + "/.kalenji_readerrc";
	#endif
	if(options.count("rcfile") != 0)
	{
		rcfile = options["rcfile"];
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
					trimString(key);
					trimString(value);
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
	// Now override configuration with options given by the users
	for(std::map<std::string, std::string>::iterator it = options.begin(); it != options.end(); ++it)
	{
		configuration[it->first] = it->second;
	}
	return true;
}

bool parseConfAndOptions(int argc, char** argv)
{
	std::map<std::string, std::string> options = readOptions(argc, argv);
	readConf(options);

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
	// TODO: Find a better way to handle this (maybe a callback of device ?)
	if(configuration["source"] == "File" && configuration["device"] != "GPX")
	{
		configuration["source"] = "HexdumpFile";
	}
	return true;
}

std::string filterSessionsToImport(SessionsMap *sessions, std::list<std::string> &outputs)
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
		std::cout << "List of sessions to import (space separated - 'all' to import everything - 'new' to import only new sessions): " << std::endl;

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
	if(to_import_string == "new")
	{
		std::ostringstream to_import;
		for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); ++it)
		{
			bool import = false;
			for(std::list<std::string>::iterator it2 = outputs.begin(); it2 != outputs.end(); ++it2)
			{
				output::Output *output = LayerRegistry<output::Output>::getInstance()->getObject(*it2);
				if(output && !output->exists(&(it->second), configuration))
				{
					to_import << " " << it->second.getNum();
					import = true;
					break;
				}
			}
			if(!import)
				std::cout << " session " << it->second.getNum() << " already imported" << std::endl;
		}
		to_import_string = to_import.str();
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
	return to_import_string;
}

int main(int argc, char *argv[])
{
	try
	{
		if(!parseConfAndOptions(argc, argv)) return -1;
		LOG_VERBOSE("Configuration parsed");

		// First attempt, creating dir if it doesn't exist
		if(!checkAndCreateDir(configuration["directory"])) return -1;
		LOG_VERBOSE("Create output directory '" << configuration["directory"] << "'");

		// TODO: Use registry for source too
		source::Source *dataSource = NULL;
		if(configuration["source"] == "File")
		{
			LOG_VERBOSE("Source is File");
			dataSource = new source::File(configuration["sourcefile"]);
		}
		else if(configuration["source"] == "HexdumpFile")
		{
			LOG_VERBOSE("Source is HexdumpFile");
			dataSource = new source::HexdumpFile(configuration["sourcefile"]);
		}
		else if(configuration["source"] == "USB")
		{
			LOG_VERBOSE("Source is USB");
			dataSource = new source::USB();
			if(configuration["log_transactions"] == "yes")
			{
				LOG_VERBOSE("With transaction logger");
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

		// Auto detection of device
		if(configuration["device"] == "auto")
		{
			if(configuration["source"] == "USB")
			{
				LOG_VERBOSE("Auto-detecting device");
				libusb_context *myUSBContext;
				libusb_device **listOfDevices;
				libusb_init(&myUSBContext);
				ssize_t nbDevices = libusb_get_device_list(myUSBContext, &listOfDevices);
				if (nbDevices < 0)
				{
					std::cerr << "Error retrieving USB devices list: " << nbDevices << std::endl;
					throw std::exception();
				}
				for(int i = 0; i < nbDevices; ++i)
				{
					libusb_device_descriptor deviceDescriptor;
					libusb_get_device_descriptor(listOfDevices[i], &deviceDescriptor);
					std::map<std::string, device::Device*> objects = LayerRegistry<device::Device>::getInstance()->getObjects();
					for(std::map<std::string, device::Device*>::iterator it = objects.begin(); it != objects.end(); ++it)
					{
						if(it->second->getVendorId() == deviceDescriptor.idVendor && it->second->getProductId() == deviceDescriptor.idProduct)
						{
							configuration["device"] = it->second->getName();
						}
					}
				}
				if(configuration["device"] == "auto")
				{
					std::cerr << "No known USB device found." << std::endl;
					return 1;
				}
				LOG_VERBOSE("Auto-detected " << configuration["device"]);
			}
			else
			{
				std::cerr << "Can't autodetermine device with a source different from USB. Specify your device using -D option or device= in your configuration file." << std::endl;
				return 1;
			}
		}

		LOG_VERBOSE("Registering device");
		device::Device *myDevice = LayerRegistry<device::Device>::getInstance()->getObject(configuration["device"]);
		if(myDevice == NULL)
		{
			std::cerr << "Error trying to register device " << configuration["device"] << ": Unknown device" << std::endl;
			throw std::exception();
		}
		LOG_VERBOSE("Attaching source to device");
		myDevice->setSource(dataSource);
		myDevice->setConfiguration(configuration);
		LOG_VERBOSE("Initializing device");
		myDevice->init();
		LOG_VERBOSE("Device initialized");
		SessionsMap sessions;
		LOG_VERBOSE("Get sessions list");
		myDevice->getSessionsList(&sessions);

		// If import = ask, prompt the user for sessions to import.
		// TODO: also prompt here for trigger type (and other info not found in the watch ?). This means at session level instead of global but could also be at lap level !
		std::list<std::string> outputs = splitString(configuration["outputs"]);
		LOG_VERBOSE("Filter out sessions");
		std::string to_import = filterSessionsToImport(&sessions, outputs);

		LOG_VERBOSE("Get sessions details");
		myDevice->getSessionsDetails(&sessions);

		LOG_VERBOSE("Release device");
		myDevice->release();
		delete myDevice;
		if(dataSource != NULL)
		{
			LOG_VERBOSE("Release datasource");
			dataSource->release();
			delete dataSource;
		}

		// TODO: Cleaner, modular way to include it ?
		// Remove empty sessions, most likely they were not imported when using a file so we don't want to export them
		for(SessionsMap::iterator it = sessions.begin(); it != sessions.end(); )
		{
			if(it->second.getPoints().empty())
				sessions.erase(it++);
			else ++it;
		}

		std::list<std::string> filters = splitString(configuration["filters"]);

		for(SessionsMap::iterator it = sessions.begin(); it != sessions.end(); ++it)
		{
			for(std::list<std::string>::iterator it2 = filters.begin(); it2 != filters.end(); ++it2)
			{
				filter::Filter *filter = LayerRegistry<filter::Filter>::getInstance()->getObject(*it2);
				if(filter)
				{
					std::cout << "  Applying filter " << *it2 << std::endl;
					filter->filter(&(it->second), configuration);
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
					try
					{
						output->dump(&(it->second), configuration);
					}
					catch(std::exception &e)
					{
						std::cerr << "Error: couldn't export to output " << *it2 << ":" << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Output does not exist: " << *it2 << std::endl;
				}
			}
		}

		sessions.clear();

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "error " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		return 1;
	}
}
