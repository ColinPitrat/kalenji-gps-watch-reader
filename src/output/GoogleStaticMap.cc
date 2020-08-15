#include "GoogleStaticMap.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(GoogleStaticMap);

	void GoogleStaticMap::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		out.precision(8);
		out << "http://maps.googleapis.com/maps/api/staticmap?size=640x640&maptype=hybrid&sensor=true&path=weight:5";
		// Google static maps can't support more than 2048 char
		// URL size is 98 + 22 per point + 9 + 22 per lap 
		// So we have to filter points so that 22 * lap + 107 + 22 * remaining_points < 2048 (hoping we don't have too much laps !)
		// i.e. remaining_points < 89 - lap
		// and remaining_points = points / filter => filter = points / (89 - lap) (plus one for rounding)
		std::vector<Point*> points = session->getPoints();
		std::vector<Lap*> laps = session->getLaps();
		uint32_t filter = 1 + points.size() / (89 - laps.size());
		uint32_t filter_index = 0;
		for(const auto& point : points)
		{
			if(filter_index % filter == 0)
				out << "%7C" << point->getLatitude() << "," << point->getLongitude();
			filter_index++;
		}
		out << "&markers=";
		for(const auto& lap : laps)
		{
			if(lap->getStartPoint() != nullptr)
			{
				out << "%7C" << lap->getStartPoint()->getLatitude() << "," << lap->getStartPoint()->getLongitude();
			}
			else
			{
				std::cerr << "Start point of lap is nullptr - This deserves a bug report !" << std::endl;
			}
		}
		out << std::endl;
	}
}
