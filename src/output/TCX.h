#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class TCX : public FileOutput
	{
		public:
			void dumpContent(std::ostream &out, const Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "TCX"; };
			std::string getExt() override { return "tcx"; }

		private:
			DECLARE_OUTPUT(TCX);
	};
}

#endif
