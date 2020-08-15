#include "ReducePoints.h"
#include "../Utils.h"
#include <cmath>
#include <sstream>

namespace filter
{
	REGISTER_FILTER(ReducePoints);

	std::vector<Point*>::iterator removePoint(Session *session, std::vector<Point*>::iterator itToRemove, Point *previousKept)
	{
		Point *point = *itToRemove;
		// Change lap boundary if it references the point
		std::vector<Lap*> &laps = session->getLaps();
		for(auto& lap : laps)
		{
			if(lap->getStartPoint() == point)
			{
				lap->setStartPoint(previousKept);
			}
			if(lap->getEndPoint() == point)
			{
				lap->setEndPoint(previousKept);
			}
		}

		// Remove point from list of points of the session
		std::vector<Point*> &points = session->getPoints();
		auto it = points.erase(itToRemove);
		delete (point);

		return it;
	}

	void ReducePoints::filter(Session *session, std::map<std::string, std::string> configuration)
	{
		std::vector<Point*> &points = session->getPoints();
		uint32_t nbPointsOri = points.size();
		
		uint32_t maxNbPoints = str_to_int(configuration["reduce_points_max"]);
		if(maxNbPoints < 3) maxNbPoints = 3;
		uint32_t minDistBetweenPoints = str_to_int(configuration["reduce_points_min_dist"]);
		uint32_t maxDistBetweenPoints = str_to_int(configuration["reduce_points_max_dist"]);
		// Arbitrary distance of 10k because session->getDistance() can return 0 in some cases
		if(maxDistBetweenPoints == 0) maxDistBetweenPoints = 10000;

		if(points.size() <= maxNbPoints) return;

		// First pass: remove points that are too close
		if(minDistBetweenPoints > 0)
		{
			auto it = points.begin();
			auto previousKept = it;
			++it;
			while(it != points.end())
			{
				if(distanceEarth(**previousKept, **it) >= minDistBetweenPoints)
				{
					previousKept = it;
					++it;
				}
				else
				{
					it = removePoint(session, it, *previousKept);
				}
			}
		}

		// Second pass: remove points that are 'nearly' aligned with the previous/next ones
		uint32_t divider = 64;
		while(points.size() > maxNbPoints && divider > 4)
		{
			auto it = points.begin();
			auto previousKept = it;
			// Always keep the first point. Always OK because we verified that points.size() > maxNbPoints > 3
			++it;
			auto previousPoint = it;
			++it;

			// TODO: A method removePoint in Session would be nice !
			while(it != points.end())
			{
				double orientation1 = 0;
				double orientation2 = 0;
				double lat1 = (*it)->getLatitude();
				double lon1 = (*it)->getLongitude();
				double lat2 = (*previousPoint)->getLatitude();
				double lon2 = (*previousPoint)->getLongitude();
				double lat3 = (*previousKept)->getLatitude();
				double lon3 = (*previousKept)->getLongitude();
				// TODO: Compare not only (pK, p) with (pP, p) but also (pK, pK+1) with (pK, p)
				orientation1 = atan2(lat1 - lat2, lon1 - lon2);
				orientation2 = atan2(lat1 - lat3, lon1 - lon3);
				double dOrientation = fabs(orientation1 - orientation2);
				double pi = 3.14159265358979323846;
				// For cases where orientation1 ~= pi and orientation2 ~= -pi or reverse
				if(dOrientation > pi) dOrientation -= 2*pi;
				if(dOrientation < -pi) dOrientation += 2*pi;

				// TODO: Arbitrary limit to determine in a smarter way ?
				if(dOrientation < (pi / divider)
				   && !(*previousPoint)->isImportant()
				   && distanceEarth(**previousKept, **it) <= maxDistBetweenPoints)
				{
					previousPoint = removePoint(session, previousPoint, *previousKept);
					it = previousPoint;
				}
				else
				{
					previousKept = previousPoint;
					previousPoint = it;
				}
				++it;
			}
			divider--;
		}
		if(points.size() > maxNbPoints)
		{
			std::cout << "Oups ! Tried to reduce the number of points as much as possible but there are still more than " << maxNbPoints << " (" << points.size() << ") !" << std::endl;
		}
		else
		{
			std::cout << "    Reduced session from " << nbPointsOri << " to " << points.size() << " points (min angle = pi/" << divider << ")." << std::endl;
		}
	}
}
