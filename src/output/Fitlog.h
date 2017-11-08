#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class Fitlog : public FileOutput
	{
		public:
			void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "Fitlog"; };
			std::string getExt() override { return "fit"; };

		private:
			DECLARE_OUTPUT(Fitlog);
	};
}

#endif
