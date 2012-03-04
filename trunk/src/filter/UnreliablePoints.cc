#include "UnreliablePoints.h"

namespace filter
{
	REGISTER_FILTER(UnreliablePoints);

	void UnreliablePoints::filter(Session *session)
	{
		std::list<Point*> &points = session->getPoints();
		std::list<Lap*> &laps = session->getLaps();
		Point *previousValid = NULL;

		// Find first reliable point. We'll need it to adjust laps if first points are not reliable.
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if((*it)->getFiability() == 3) 
			{
				previousValid = *it;
				break;
			}
		}

		// TODO: What if there is no reliable point ?! No filtering at all ?

		// TODO: A method removePoint in Session would be nice !
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); )
		{
			if((*it)->getFiability() != 3) 
			{
				for(std::list<Lap*>::iterator it2 = laps.begin(); it2 != laps.end(); ++it2)
				{
					if((*it2)->getStartPoint() == (*it))
					{
						(*it2)->setStartPoint(previousValid);
					}
					if((*it2)->getEndPoint() == (*it))
					{
						(*it2)->setEndPoint(previousValid);
					}
				}
				delete (*it);
				it = points.erase(it);
			}
			else
			{
				previousValid = *it;
				++it;
			}
		}
	}
}
