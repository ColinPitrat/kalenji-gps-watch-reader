#include "NullHeartrate.h"

namespace filter
{
	REGISTER_FILTER(NullHeartrate);

	void NullHeartrate::filter(Session *session)
	{
		// TODO: If first heartrates are 0, they will stay 0
		uint16_t previousHeartrate = 0;
		std::list<Point*> &points = session->getPoints();
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if((*it)->getHeartRate() == 0)
			{
				(*it)->setHeartRate(previousHeartrate);
			}
			else
			{
				previousHeartrate = (*it)->getHeartRate();
			}
		}
	}
}
