#ifndef _FILTER_FIXELEVATION_H
#define _FILTER_FIXELEVATION_H

#include "Filter.h"

namespace filter
{
	class FixElevation : public Filter
	{
		public:
			static size_t copyHTTPData(void *ptr, size_t size, size_t nmemb, FILE *stream);
			bool parseHTTPData(Session *session, std::vector<Point*>::iterator first, std::vector<Point*>::iterator last);
			void filter(Session *session, std::map<std::string, std::string> configuration) override;
			std::string getName() override { return "FixElevation"; };

		private:
			static std::string HTTPdata;
			uint32_t fixed_points;
			DECLARE_FILTER(FixElevation);
	};
}

#endif
