#ifndef _FILTER_NULLHEARTRATE_H
#define _FILTER_NULLHEARTRATE_H

#include "Filter.h"

namespace filter
{
	class NullHeartrate : public Filter
	{
		public:
			virtual void filter(Session *session, std::map<std::string, std::string> configuration);
			virtual std::string getName() { return "NullHeartrate"; };

			DECLARE_FILTER(NullHeartrate);
	};
}

#endif
