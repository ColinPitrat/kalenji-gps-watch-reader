#ifndef _OUTPUT_CSV_HPP_
#define _OUTPUT_CSV_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class CSV : public FileOutput
	{
		public:
			virtual void dumpContent(std::ostream &out, Session *session, std::map<std::string, std::string> &configuration);
			virtual std::string getName() { return "CSV"; };
			virtual std::string getExt() { return "csv"; };

		private:
			DECLARE_OUTPUT(CSV);
	};
}

#endif
