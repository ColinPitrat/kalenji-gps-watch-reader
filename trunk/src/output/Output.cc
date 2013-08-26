#include "Output.h"
#include <sstream>
#include <iomanip>
#include <fstream>

#include <unistd.h>

namespace output
{
	std::string FileOutput::getFileName(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::ostringstream filename;
		filename << configuration["directory"] << "/" << session->getYear() <<
			std::setw(2) << std::setfill('0') << session->getMonth() <<
			std::setw(2) << std::setfill('0') << session->getDay() << "_" <<
			std::setw(2) << std::setfill('0') << session->getHour() <<
			std::setw(2) << std::setfill('0') << session->getMinutes() <<
			std::setw(2) << std::setfill('0') << session->getSeconds() <<
			"." << getExt();
		return filename.str();
	}

	void FileOutput::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::string filename(getFileName(session, configuration));
		std::ofstream mystream(filename.c_str());
		std::cout << "Creating " << filename << std::endl;
		dumpContent(mystream, session, configuration);
		mystream.close();
	}

	bool FileOutput::exists(Session *session, std::map<std::string, std::string> &configuration)
	{
		return access(getFileName(session, configuration).c_str(), F_OK) == 0;
	}
}
