#include "GoogleStaticMap.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(GoogleStaticMap);

	void GoogleStaticMap::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::stringstream filename;
		filename << configuration["directory"] << "/"; 
		filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_"; 
		filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".lnk";
		std::cout << "Creating " << filename.str() << std::endl;

		std::ofstream mystream(filename.str().c_str());
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "http://maps.googleapis.com/maps/api/staticmap?size=640x640&maptype=hybrid&sensor=true&path=weight:5";
		// Google static maps can't support more than 2048 char
		// URL size is 98 + 22 per point + 9 + 22 per lap 
                // So we have to filter points so that 22 * lap + 107 + 22 * remaining_points < 2048 (hoping we don't have too much laps !)
		// i.e. remaining_points < 89 - lap
		// and remaining_points = points / filter => filter = points / (89 - lap) (plus one for rounding)
		std::list<Point*> points = session->getPoints();
		std::list<Lap*> laps = session->getLaps();
		uint32_t filter = 1 + points.size() / (89 - laps.size());
		uint32_t filter_index = 0;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if(filter_index % filter == 0)
				mystream << "%7C" << (*it)->getLatitude() << "," << (*it)->getLongitude();
			filter_index++;
		}
		mystream << "&markers=";
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			mystream << "%7C" << (*it)->getStartPoint()->getLatitude() << "," << (*it)->getStartPoint()->getLongitude();
		}
		mystream << std::endl;
	}
}
