#ifndef _OUTPUT_GPX_HPP_
#define _OUTPUT_GPX_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class Fitlog : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "Fitlog"; };
			virtual std::string getExt() { return "fit"; };

		private:
			DECLARE_OUTPUT(Fitlog);
	};
}

#endif
