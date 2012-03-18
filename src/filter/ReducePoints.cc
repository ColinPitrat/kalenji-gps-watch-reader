#include "ReducePoints.h"
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
		uint32_t divider = 16;

		while(points.size() > maxNbPoints && divider > 4)
		{
			std::list<Point*>::iterator it = points.begin();
			Point *previousKept = *it;

			// TODO: A method removePoint in Session would be nice !
			++it;
			Point *previousPoint = *it;
			while(it != points.end())
			{
				double orientation1 = atan2(((*it)->getLatitude() - previousPoint->getLatitude()), ((*it)->getLongitude() - previousPoint->getLongitude()));
				double orientation2 = atan2(((*it)->getLatitude() - previousKept->getLatitude() ), ((*it)->getLongitude() - previousKept->getLongitude() ));
				double dOrientation = fabs(orientation1 - orientation2);
				double pi = 3.14159265358979323846;
				// For cases where orientation1 ~= pi and orientation2 ~= -pi or reverse
				if(dOrientation > pi) dOrientation -= 2*pi;
				if(dOrientation < -pi) dOrientation += 2*pi;

				// TODO: Arbitrary limit to determine in a smarter way ?
				if(dOrientation < (pi / 8) && !previousPoint->isImportant())
				{
					for(std::list<Lap*>::iterator it2 = laps.begin(); it2 != laps.end(); ++it2)
					{
						if((*it2)->getStartPoint() == previousPoint)
						{
							(*it2)->setStartPoint(previousKept);
						}
						if((*it2)->getEndPoint() == previousPoint)
						{
							(*it2)->setEndPoint(previousKept);
						}
					}
					points.remove(previousPoint);
					delete (previousPoint);
				}
				else
				{
					previousKept = previousPoint;
				}
				previousPoint = *it;
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
			std::cout << "    Reduced session from " << nbPointsOri << " to " << points.size() << " points." << std::endl;
		}
	}
}
