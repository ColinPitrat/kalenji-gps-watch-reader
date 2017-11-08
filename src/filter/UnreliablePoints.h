#ifndef _FILTER_UNRELIABLE_POINTS_H
#define _FILTER_UNRELIABLE_POINTS_H

#include "Filter.h"

namespace filter
{
	class UnreliablePoints : public Filter
	{
		public:
			void filter(Session *session, std::map<std::string, std::string> configuration) override;
			std::string getName() override { return "UnreliablePoints"; };

		private:
			DECLARE_FILTER(UnreliablePoints);
	};
}

#endif
