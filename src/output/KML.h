#ifndef _OUTPUT_KML_HPP_
#define _OUTPUT_KML_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class KML : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "KML"; };

		private:
			DECLARE_OUTPUT(KML);
	};
}

#endif
