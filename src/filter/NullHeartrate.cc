#include "NullHeartrate.h"

namespace filter
{
	REGISTER_FILTER(NullHeartrate);

	void NullHeartrate::filter(Session *session, std::map<std::string, std::string> configuration)
	{
		// TODO: If first heartrates are 0, they will stay 0
		uint16_t previousHeartrate = 0;
		std::vector<Point*> &points = session->getPoints();
		for(const auto& point : points)
		{
			if(point->getHeartRate() == 0)
			{
				point->setHeartRate(previousHeartrate);
			}
			else
			{
				previousHeartrate = point->getHeartRate();
			}
		}
	}
}
