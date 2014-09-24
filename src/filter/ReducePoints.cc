#include "ReducePoints.h"
#include "../Common.h"
#include <cmath>

namespace filter
{
	REGISTER_FILTER(ReducePoints);

	void ReducePoints::filter(Session *session)
	{
		std::list<Point*> &points = session->getPoints();
		std::list<Lap*> &laps = session->getLaps();
		uint32_t nbPointsOri = points.size();
		
		uint32_t maxNbPoints = 200;
		uint32_t divider = 64;

		while(points.size() > maxNbPoints && divider > 4)
		{
			std::list<Point*>::iterator it = points.begin();
			std::list<Point*>::iterator previousKept = it;
			// Always keep the first point. Always OK because we verified that points.size() > maxNbPoints > 3
			++it;
			std::list<Point*>::iterator previousPoint = it;
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
				orientation1 = atan2(lat1 - lat2, lon1 - lon2);
				orientation2 = atan2(lat1 - lat3, lon1 - lon3);
				double dOrientation = fabs(orientation1 - orientation2);
				double pi = 3.14159265358979323846;
				// For cases where orientation1 ~= pi and orientation2 ~= -pi or reverse
				if(dOrientation > pi) dOrientation -= 2*pi;
				if(dOrientation < -pi) dOrientation += 2*pi;

				// TODO: Arbitrary limit to determine in a smarter way ?
				if(dOrientation < (pi / divider) && !(*previousPoint)->isImportant())
				{
					for(std::list<Lap*>::iterator it2 = laps.begin(); it2 != laps.end(); ++it2)
					{
						if((*it2)->getStartPoint() == (*previousPoint))
						{
							(*it2)->setStartPoint(*previousKept);
						}
						if((*it2)->getEndPoint() == (*previousPoint))
						{
							(*it2)->setEndPoint(*previousKept);
						}
					}
					Point *toDelete = *previousPoint;
					previousPoint = points.erase(previousPoint);
					delete (toDelete);
				}
				else
				{
					previousKept = previousPoint;
				}
				previousPoint = it;
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
