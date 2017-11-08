#ifndef _OUTPUT_GOOGLESTATICMAP_HPP_
#define _OUTPUT_GOOGLESTATICMAP_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class GoogleStaticMap : public FileOutput
	{
		public:
			void dumpContent(std::ostream &out, const Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "GoogleStaticMap"; };
			std::string getExt() override { return "lnk"; }

		private:
			DECLARE_OUTPUT(GoogleStaticMap);
	};
}

#endif
