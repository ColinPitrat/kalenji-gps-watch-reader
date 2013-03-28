#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GPX : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "GPX"; };
			virtual std::string getExt() { return "gpx"; };

		private:
			DECLARE_OUTPUT(GPX);
	};
}

#endif
