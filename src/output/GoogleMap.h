#ifndef _OUTPUT_GOOGLEMAP_HPP_
#define _OUTPUT_GOOGLEMAP_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GoogleMap : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "GoogleMap"; };

		private:
			DECLARE_OUTPUT(GoogleMap);
	};
}

#endif
