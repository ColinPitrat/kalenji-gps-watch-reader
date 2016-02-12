#include "OnMove200.h"
#include <cstring>
#include <iomanip>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <dirent.h>
#include <sys/stat.h>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
	REGISTER_DEVICE(OnMove200);

/*
	int bytesToInt2(unsigned char b0, unsigned char b1)
	{
		int Int = b0 | ( (int)b1 << 8 );
		return Int;
	}

	int bytesToInt4(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3)
	{
		int Int = b0 | ( (int)b1 << 8 ) | ( (int)b2 << 16 ) | ( (int)b3 << 24 );
		return Int;
	}
  */

	unsigned char* OnMove200::readAllBytes(std::string filename, size_t& size)
	{
		std::ifstream fl(filename.c_str());
		fl.seekg( 0, std::ios::end );
		size_t len = fl.tellg();
		char* buffer = new char[len];
		fl.seekg(0, std::ios::beg);
		fl.read(buffer, len);
		fl.close();
		size = len;
		return (unsigned char*)buffer;
	}

	bool OnMove200::fileExists(std::string filename)
	{
		struct stat fileInfo;
		return stat(filename.c_str(), &fileInfo) == 0;
	}

	void OnMove200::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::endl);
	}

	void OnMove200::init()
	{
		//check if getPath() is a valid path
		DIR* folder = opendir(getPath().c_str());
		if (folder == NULL)
		{
			std::cout<< "Error: path '" << getPath() << "' does not exist (check option -p <path> on command line or line path=<path> in configuration file)." << std::endl;
			throw std::exception();
		}
		closedir(folder);
	}

	void OnMove200::release()
	{
		DEBUG_CMD(std::cout << "OnMove200: Release (nothing to do)" << std::endl);
	}

	std::string OnMove200::getPath()
	{
		return _configuration["path"];
	}

	void OnMove200::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "OnMove200: Get sessions list !" << std::endl);

		DIR* folder = NULL;
		struct dirent* file = NULL;
		folder = opendir(getPath().c_str());
		if (folder == NULL)
		{
			std::cerr << "Couldn't open dir " << getPath() << std::endl;
			throw std::exception();
		}

		std::set<std::string> filenamesPrefix;

		while ((file = readdir(folder)) != NULL)
		{
			std::string fn = std::string(file->d_name);
			if(strstr(file->d_name,".OMD") || strstr(file->d_name,".OMH"))
			{
				std::string fileprefix = fn.substr(0,fn.length() - 4);
				std::string filepathprefix = getPath() + std::string("/") + fileprefix;
				//check if both files (header and data) exists
				if(fileExists(filepathprefix + ".OMD") && fileExists(filepathprefix + ".OMH"))
				{
					filenamesPrefix.insert(fileprefix);
				}
				else
				{
					std::cout << "Discarding " << fileprefix << std::endl;
				}
			}
		}

		closedir(folder);

		int i = 0;
		std::set<std::string>::iterator iter;
		for (iter=filenamesPrefix.begin(); iter!=filenamesPrefix.end(); ++iter)
		{
			std::string fileprefix = *iter;

			DEBUG_CMD(std::cout << "Decode summary of session " << fileprefix << std::endl);
			// Decoding of basic info about the session
			std::vector<char> _sessionId(fileprefix.begin(), fileprefix.end());
			SessionId id = SessionId(_sessionId);
			uint32_t num = i++; //Just increment by one each time

      // TODO parseOMHFile();
			tm time;

			double duration = 0;
			uint32_t distance = 0;
			uint32_t nbLaps = 999;

			Session mySession(id, num, time, 0, duration, distance, nbLaps);

			// Properly fill necessary session info (duration, distance, nbLaps)
			std::string ghtFilename = getPath() + std::string("/") + fileprefix + std::string(".OMH");
			size_t size = -1;
			unsigned char* buffer = readAllBytes(ghtFilename, size);
			parseOMHFile(buffer, &mySession);
			delete buffer;

			oSessions->insert(SessionsMapElement(id, mySession));
		}
	}

	void OnMove200::getSessionsDetails(SessionsMap *oSessions)
	{
		for(SessionsMap::iterator it = oSessions->begin(); it != oSessions->end(); ++it)
		{
			Session* session = &(it->second);
			SessionId sessionId = it->second.getId();
			std::string filenamePrefix(sessionId.begin(),sessionId.end());	
			std::cout << "Retrieve session " << filenamePrefix << std::endl;

			filenamePrefix = getPath() + std::string("/") + filenamePrefix;

			unsigned char* buffer;
			size_t size = -1;

			std::string omdFilename = filenamePrefix + std::string(".OMD");
			buffer = readAllBytes(omdFilename,size);
			parseOMDFile(buffer,session);
			delete buffer;
		}
	}

	void OnMove200::dumpInt2(std::ostream &oStream, unsigned int aInt)
	{
		oStream << (char)(aInt & 0xFF) << (char)((aInt & 0xFF00) >> 8);
	}

	void OnMove200::dumpInt4(std::ostream &oStream, unsigned int aInt)
	{
		oStream << (char)(aInt & 0xFF) << (char)((aInt & 0xFF00) >> 8) << (char)((aInt & 0xFF0000) >> 16) << (char)((aInt & 0xFF000000) >> 24);
	}

	void OnMove200::dumpString(std::ostream &oStream, const std::string &aString, size_t aLength)
	{
		size_t toCopy = aString.length();
		if(aLength <= toCopy) toCopy = aLength - 1;
		oStream.write(aString.c_str(), toCopy);
		for(size_t i = toCopy; i < aLength; ++i)
		{
			oStream.put('\0');
		}
	}

	void OnMove200::exportSession(Session *iSession)
	{
    std::cerr << "Unsupported export session for OnMove200" << std::endl;
	}

	void OnMove200::parseOMHFile(const unsigned char* bytes, Session* session)
	{
	}

	void OnMove200::parseOMDFile(const unsigned char* bytes, Session *session)
	{
	}
}
