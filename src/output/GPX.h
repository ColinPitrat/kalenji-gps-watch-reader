#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GPX : public FileOutput
	{
		public:
			void dumpContent(std::ostream &out, const Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "GPX"; };
			std::string getExt() override { return "gpx"; };

		private:
			DECLARE_OUTPUT(GPX);
	};
}

#endif
