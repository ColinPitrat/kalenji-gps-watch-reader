#ifndef _FILTER_COMPUTEINSTANTSPEED_H
#define _FILTER_COMPUTEINSTANTSPEED_H

#include "Filter.h"

namespace filter
{
	class ComputeInstantSpeed : public Filter
	{
		public:
			virtual void filter(Session *session);
			virtual std::string getName() { return "ComputeInstantSpeed"; };

		private:
			DECLARE_FILTER(ComputeInstantSpeed);
	};
}

#endif
