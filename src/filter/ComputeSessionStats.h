#ifndef _FILTER_COMPUTESESSIONSTATS_H
#define _FILTER_COMPUTESESSIONSTATS_H

#include "Filter.h"

namespace filter
{
	class ComputeSessionStats : public Filter
	{
		public:
			void filter(Session *session, std::map<std::string, std::string> configuration) override;
			std::string getName() override { return "ComputeSessionStats"; };

		private:
			DECLARE_FILTER(ComputeSessionStats);
	};
}

#endif
