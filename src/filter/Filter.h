#ifndef _FILTER_FILTER_H
#define _FILTER_FILTER_H

#include "../Registry.h"
#include "../bom/Session.h"

#define DECLARE_FILTER(FilterClass) static LayerRegistrer<Filter, FilterClass> _registrer;
#define REGISTER_FILTER(FilterClass) LayerRegistrer<Filter, FilterClass> FilterClass::_registrer;

namespace filter
{
	class Filter
	{
		public:
			virtual ~Filter() = default;
			virtual void filter(Session *session, std::map<std::string, std::string> configuration) = 0;
			virtual std::string getName() = 0;
	};
}

#endif
