#include "OnMove710.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "../device/OnMove710.h"
#include "../source/USB.h"

namespace output
{
	REGISTER_OUTPUT(OnMove710);

	void OnMove710::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		source::Source *dataSource = new source::USB();
		device::Device *dev = new device::OnMove710();
		dev->setConfiguration(configuration);
		dev->setSource(dataSource);
		dev->init();
		dev->exportSession(session);
		delete dev;
		delete dataSource;
	}
}
