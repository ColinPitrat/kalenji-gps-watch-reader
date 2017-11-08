#ifndef _OUTPUT_OUTPUT_HPP_
#define _OUTPUT_OUTPUT_HPP_

#include "../Registry.h"
#include "../bom/Session.h"
#include <ostream>

#define DECLARE_OUTPUT(OutputClass) static LayerRegistrer<Output, OutputClass> _registrer;
#define REGISTER_OUTPUT(OutputClass) LayerRegistrer<Output, OutputClass> OutputClass::_registrer;

namespace output
{
	class Output
	{
		public:
			virtual ~Output() = default;
			// TODO: I don't really like giving the configuration this way. Better way to configure each output ?
			virtual void dump(const Session *session, std::map<std::string, std::string> &configuration) = 0;
			virtual bool exists(const Session *session, std::map<std::string, std::string> &configuration) { return false; }
			virtual std::string getName() = 0;
	};

	class FileOutput: public Output
	{
		public:
			std::string getFileName(const Session *session, std::map<std::string, std::string> &configuration);
			void dump(const Session *session, std::map<std::string, std::string> &configuration) override;
			virtual void dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration) = 0;
			bool exists(const Session *session, std::map<std::string, std::string> &configuration) override;
			virtual std::string getExt() = 0;
	};
}

#endif
