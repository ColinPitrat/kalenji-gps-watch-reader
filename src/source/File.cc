#include "File.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <unistd.h>

namespace source
{
	void File::init(uint32_t vendorId, uint32_t productId)
	{
		if(access(_filename.c_str(), R_OK) == 0)
		{
			std::string line;
			std::ifstream inputfile(_filename.c_str());
			if (inputfile.is_open())
			{
				while ( inputfile.good() )
				{
					getline(inputfile, line);
					_inputStream.push_back(line);
				}
				inputfile.close();
			}
			else 
			{
				std::cerr << "Unable to open " << _filename; 
				// TODO: Throw an exception
			}
		}
	}

	bool File::read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength)
	{
		if(_index < _inputStream.size())
		{
			*oLength = _inputStream[_index].length();
			*oData = (unsigned char*) _inputStream[_index].c_str();
			_index++;
		}
		return _index < _inputStream.size();
	}
}
