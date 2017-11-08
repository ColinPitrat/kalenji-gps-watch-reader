#ifndef _OUTPUT_CSV_HPP_
#define _OUTPUT_CSV_HPP_

#include "../bom/Session.h"
#include "Output.h"

namespace output
{
	class CSV : public FileOutput
	{
		public:
			void dumpContent(std::ostream &out, const Session *session, std::map<std::string, std::string> &configuration) override;
			std::string getName() override { return "CSV"; };
			std::string getExt() override { return "csv"; };

		private:
			DECLARE_OUTPUT(CSV);
	};
}

#endif
