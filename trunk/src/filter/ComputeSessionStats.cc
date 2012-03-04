#include "ComputeSessionStats.h"
#include <cmath>

namespace filter
{
	REGISTER_FILTER(ComputeSessionStats);

	void ComputeSessionStats::filter(Session *session)
	{
		std::list<Point*> &points = session->getPoints();
		uint32_t ascent = 0;
		uint32_t descent = 0;
		uint32_t distance = 0;
		double maxspeed = 0;
		double avgspeed = 0;
		time_t mintime;
		time_t maxtime;
		uint32_t duration = 0;

		std::list<Point*>::iterator it = points.begin();
		maxtime = mintime = (*it)->getTime();
		while(true)
		{
			Point *prevPoint = *it;
			++it;
			if(it == points.end()) break;

			// -8<--- This part computes the distance between the 2 points
			double pi = 3.14159265358979323846;
			double R = 6371000; // Approximate radius of the Earth in meters
			double dLatRad = ((*it)->getLatitude()  - prevPoint->getLatitude())  * pi / 180.0;
			double dLonRad = ((*it)->getLongitude() - prevPoint->getLongitude()) * pi / 180.0;
			double lat1Rad =  prevPoint->getLatitude() * pi / 180.0;
			double lat2Rad =  (*it)->getLatitude()     * pi / 180.0;
			double a = sin(dLatRad/2) * sin(dLatRad/2) + sin(dLonRad/2) * sin(dLonRad/2) * cos(lat1Rad) * cos(lat2Rad);
			double c = 2 * atan2(sqrt(a), sqrt(1-a));
			double dist = R * c;
			// ->8---
			distance += dist;

			int eleChange = (*it)->getAltitude() - prevPoint->getAltitude();
			if(eleChange > 0)
				ascent += eleChange;
			else
				descent -= eleChange;

			if((*it)->getTime() > maxtime) maxtime = (*it)->getTime();
			if((*it)->getTime() < mintime) mintime = (*it)->getTime();
			double speed = (3.6 * distance) / ((*it)->getTime() - prevPoint->getTime());
			if(speed > maxspeed) maxspeed = speed;
		}
		duration = maxtime - mintime;
		avgspeed = (3.6 * distance) / duration;

		std::cout << "Ascent is " << ascent << " and descent is " << descent << std::endl;
		session->setAscent(ascent);
		session->setDescent(descent);
		session->setDistance(distance);
		session->setMaxSpeed(maxspeed);
		session->setAvgSpeed(avgspeed);
		session->setDuration(duration);
	}
}
