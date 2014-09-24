#ifndef _FILTER_COMPUTESESSIONSTATS_H
#define _FILTER_COMPUTESESSIONSTATS_H

#include "Filter.h"

namespace filter
{
	class ComputeSessionStats : public Filter
	{
		public:
			virtual void filter(Session *session, std::map<std::string, std::string> configuration);
			virtual std::string getName() { return "ComputeSessionStats"; };

		private:
			DECLARE_FILTER(ComputeSessionStats);
	};
}

#endif
