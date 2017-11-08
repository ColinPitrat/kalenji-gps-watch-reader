#include "CSV.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

namespace output
{
	REGISTER_OUTPUT(CSV);

	void CSV::dumpContent(std::ostream& mystream, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "Time (s),Distance " << session->getName() << " (m)" << ",Altitude " << session->getName() << " (m)" << std::endl;

		uint32_t distance = 0;
		std::list<Point*> points = session->getPoints();
		Point *prevPoint = nullptr;
		uint32_t time_begin = (*points.begin())->getTime();
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			// -8<--- This part computes the distance between the 2 points
			if(prevPoint)
			{
				if( (*it)->getLatitude().isDefined() && prevPoint->getLatitude().isDefined() && (*it)->getLongitude().isDefined() && prevPoint->getLongitude().isDefined() )
				{
					static double pi = 3.14159265358979323846;
					static double R = 6371000; // Approximate radius of the Earth in meters
					double dLatRad = ((*it)->getLatitude()  - prevPoint->getLatitude())  * pi / 180.0;
					double dLonRad = ((*it)->getLongitude() - prevPoint->getLongitude()) * pi / 180.0;
					double lat1Rad =  prevPoint->getLatitude() * pi / 180.0;
					double lat2Rad =  (*it)->getLatitude()     * pi / 180.0;
					double a = sin(dLatRad/2) * sin(dLatRad/2) + sin(dLonRad/2) * sin(dLonRad/2) * cos(lat1Rad) * cos(lat2Rad);
					double c = 2 * atan2(sqrt(a), sqrt(1-a));
					double dist = R * c;
					distance += dist;
				}
				else
				{
					distance += ( (*it)->getTime() - prevPoint->getTime() ) * (double) (*it)->getSpeed() / 3.6;
				}
			}
			prevPoint = *it;
			// ->8---
			uint32_t time = (*it)->getTime() - time_begin;
			mystream << time << ",";
			mystream << distance << ",";
      mystream << (*it)->getAltitude() << std::endl;
		}
	}
}
