#ifndef _OUTPUT_KALENJI_HPP_
#define _OUTPUT_KALENJI_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class Kalenji : public Output
	{
		public:
			void dump(Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "Kalenji"; };

		private:
			DECLARE_OUTPUT(Kalenji);
	};
}

#endif
