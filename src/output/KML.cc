#include "KML.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(KML);

	void KML::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::stringstream filename;
		filename << configuration["directory"] << "/"; 
		filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_"; 
		filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".kml";
		std::cout << "Creating " << filename.str() << std::endl;
		std::ofstream mystream(filename.str().c_str());
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);

		// TODO: Whole KML support !

		mystream.close();
	}
}
