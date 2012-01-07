#include "EmptyLaps.h"

namespace filter
{
	REGISTER_FILTER(EmptyLaps);

	void EmptyLaps::filter(Session *session)
	{
		std::list<Lap*> &laps = session->getLaps();
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); )
		{
			if((*it)->getLength() == 0 or (*it)->getDuration() == 0 or (*it)->getStartPoint() == (*it)->getEndPoint())
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
