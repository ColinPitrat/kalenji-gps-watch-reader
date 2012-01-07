#ifndef _FILTER_EMPTYLAPS_H
#define _FILTER_EMPTYLAPS_H

#include "Filter.h"

namespace filter
{
	class EmptyLaps : public Filter
	{
		public:
			virtual void filter(Session *session);
			virtual std::string getName() { return "EmptyLaps"; };

			DECLARE_FILTER(EmptyLaps);
	};
}

#endif
