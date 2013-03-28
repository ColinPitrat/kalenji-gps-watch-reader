#ifndef _OUTPUT_KML_HPP_
#define _OUTPUT_KML_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class KML : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "KML"; };
			virtual std::string getExt() { return "kml"; }

		private:
			DECLARE_OUTPUT(KML);
	};
}

#endif
