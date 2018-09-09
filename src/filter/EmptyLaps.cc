#include "EmptyLaps.h"

namespace filter
{
	REGISTER_FILTER(EmptyLaps);

	void EmptyLaps::filter(Session *session, std::map<std::string, std::string> configuration)
	{
		std::vector<Lap*> &laps = session->getLaps();
		for(auto it = laps.begin(); it != laps.end(); )
		{
			if((*it)->getDistance() == 0 or (*it)->getDuration() == 0 or (*it)->getStartPoint() == (*it)->getEndPoint())
			{
				it = laps.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
