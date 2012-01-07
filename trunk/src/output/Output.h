#ifndef _OUTPUT_OUTPUT_HPP_
#define _OUTPUT_OUTPUT_HPP_

#include "../Registry.h"
#include "../bom/Session.h"

#define DECLARE_OUTPUT(OutputClass) static LayerRegistrer<Output, OutputClass> _registrer;
#define REGISTER_OUTPUT(OutputClass) LayerRegistrer<Output, OutputClass> OutputClass::_registrer;

namespace output
{
	class Output
	{
		public:
			// TODO: I don't really like giving the configuration this way. Better way to configure each output ?
			virtual void dump(Session *session, std::map<std::string, std::string> &configuration) = 0;
			virtual std::string getName() = 0;
	};
}

#endif
