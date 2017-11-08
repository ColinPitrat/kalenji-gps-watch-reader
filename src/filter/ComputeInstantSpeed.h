#ifndef _FILTER_COMPUTEINSTANTSPEED_H
#define _FILTER_COMPUTEINSTANTSPEED_H

#include "Filter.h"

namespace filter
{
	class ComputeInstantSpeed : public Filter
	{
		public:
			void filter(Session *session, std::map<std::string, std::string> configuration) override;
			std::string getName() override { return "ComputeInstantSpeed"; };

		private:
			DECLARE_FILTER(ComputeInstantSpeed);
	};
}

#endif
