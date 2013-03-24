#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class TCX : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "TCX"; };

		private:
			DECLARE_OUTPUT(TCX);
	};
}

#endif
