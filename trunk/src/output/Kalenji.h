#ifndef _OUTPUT_KALENJI_HPP_
#define _OUTPUT_KALENJI_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class Kalenji : public Output
	{
		public:
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "Kalenji"; };

		private:
			DECLARE_OUTPUT(Kalenji);
	};
}

#endif
