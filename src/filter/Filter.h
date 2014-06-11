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
			virtual ~Filter() {};
			virtual void filter(Session *session) = 0;
			virtual std::string getName() = 0;
	};
}

#endif
