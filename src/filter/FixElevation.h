#ifndef _FILTER_FIXELEVATION_H
#define _FILTER_FIXELEVATION_H

#include "Filter.h"

namespace filter
{
	class FixElevation : public Filter
	{
		public:
			virtual void filter(Session *session);
			virtual std::string getName() { return "FixElevation"; };

		private:
			DECLARE_FILTER(FixElevation);
	};
}

#endif
