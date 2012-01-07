#ifndef _OUTPUT_GOOGLESTATICMAP_HPP_
#define _OUTPUT_GOOGLESTATICMAP_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GoogleStaticMap : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "GoogleStaticMap"; };

		private:
			DECLARE_OUTPUT(GoogleStaticMap);
	};
}

#endif
