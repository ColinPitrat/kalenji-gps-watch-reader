#ifndef _FILTER_REDUCEPOINTS_H
#define _FILTER_REDUCEPOINTS_H

#include "Filter.h"

namespace filter
{
	class ReducePoints : public Filter
	{
		public:
			void filter(Session *session, std::map<std::string, std::string> configuration) override;
			std::string getName() override { return "ReducePoints"; };

		private:
			DECLARE_FILTER(ReducePoints);
	};
}

#endif
