#ifndef _FILTER_UNRELIABLE_POINTS_H
#define _FILTER_UNRELIABLE_POINTS_H

#include "Filter.h"

namespace filter
{
	class UnreliablePoints : public Filter
	{
		public:
			virtual void filter(Session *session);
			virtual std::string getName() { return "UnreliablePoints"; };

		private:
			DECLARE_FILTER(UnreliablePoints);
	};
}

#endif
