#ifndef _FILTER_REDUCEPOINTS_H
#define _FILTER_REDUCEPOINTS_H

#include "Filter.h"

namespace filter
{
	class ReducePoints : public Filter
	{
		public:
			virtual void filter(Session *session, std::map<std::string, std::string> configuration);
			virtual std::string getName() { return "ReducePoints"; };

		private:
			DECLARE_FILTER(ReducePoints);
	};
}

#endif
