#include "ComputeInstantSpeed.h"
#include "../Utils.h"
#include <cmath>

namespace filter
{
	REGISTER_FILTER(ComputeInstantSpeed);

	void ComputeInstantSpeed::filter(Session *session, std::map<std::string, std::string> configuration)
	{
		std::list<Point*> &points = session->getPoints();
		Point* prevPoint = nullptr;

		std::list<Point*>::iterator it = points.begin();
		double speed = 0;
		// TODO: Support a moving average
		while(true)
		{
			prevPoint = *it;
			++it;
			if(it == points.end()) break;

			uint32_t distance = distanceEarth(**it, *prevPoint);

			speed = (3.6 * distance) / ((*it)->getTime() - prevPoint->getTime());
			prevPoint->setSpeed(speed);
		}
		// On last point, set same speed as on the previous point
		prevPoint->setSpeed(speed);
	}
}
