#ifndef _OUTPUT_ONMOVE710_HPP_
#define _OUTPUT_ONMOVE710_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class OnMove710 : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "OnMove710"; };

		private:
			DECLARE_OUTPUT(OnMove710);
	};
}

#endif
