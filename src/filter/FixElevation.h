#ifndef _FILTER_FIXELEVATION_H
#define _FILTER_FIXELEVATION_H

#include "Filter.h"

namespace filter
{
	class FixElevation : public Filter
	{
		public:
			static size_t copyHTTPData(void *ptr, size_t size, size_t nmemb, FILE *stream);
			void parseHTTPData(Session *session, std::list<Point*>::iterator first, std::list<Point*>::iterator last);
			virtual void filter(Session *session);
			virtual std::string getName() { return "FixElevation"; };

		private:
			static std::string HTTPdata;
			DECLARE_FILTER(FixElevation);
	};
}

#endif
