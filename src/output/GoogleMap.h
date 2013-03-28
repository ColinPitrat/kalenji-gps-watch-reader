#ifndef _OUTPUT_GOOGLEMAP_HPP_
#define _OUTPUT_GOOGLEMAP_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GoogleMap : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "GoogleMap"; };
			virtual std::string getExt() { return "html"; };

		private:
			DECLARE_OUTPUT(GoogleMap);
	};
}

#endif
