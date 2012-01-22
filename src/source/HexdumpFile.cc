#include "HexdumpFile.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>

namespace source
{
	void HexdumpFile::init(uint32_t vendorId, uint32_t productId)
	{
		if(access(_filename.c_str(), R_OK) == 0)
		{
			std::string line;
			std::ifstream inputfile(_filename.c_str());
			if (inputfile.is_open())
			{
				while ( inputfile.good() )
				{
					std::string thisLine;
					getline(inputfile, line);
					std::stringstream iss(line);
					iss >> std::hex;
					std::copy(std::istream_iterator<unsigned int>(iss), std::istream_iterator<unsigned int>(), std::back_inserter(thisLine));
					_inputStream.push_back(thisLine);
				}
				inputfile.close();
			}
			else 
			{
				std::cerr << "Unable to open " << inputfile; 
				// TODO: Throw an exception
			}
		}
	}

	bool HexdumpFile::read_data(unsigned char **oData, size_t *oLength)
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
