#ifndef _OUTPUT_GOOGLESTATICMAP_HPP_
#define _OUTPUT_GOOGLESTATICMAP_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GoogleStaticMap : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "GoogleStaticMap"; };
			virtual std::string getExt() { return "lnk"; }

		private:
			DECLARE_OUTPUT(GoogleStaticMap);
	};
}

#endif
