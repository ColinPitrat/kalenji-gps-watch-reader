#ifndef _SOURCE_HEXDUMPFILE_HPP_
#define _SOURCE_HEXDUMPFILE_HPP_

#include <string>
#include <vector>
#include "Source.h"

namespace source
{
	/**
	  A class to handle input from a file instead of from USB device
	 */
	class HexdumpFile : public Source
	{
		public:
			HexdumpFile(std::string filename) : _filename(filename), _index(0) {};
			/** 
			  Read the whole file.
			 */ 
			virtual void init(uint32_t vendorId, uint32_t productId);

			/** 
			  Do nothing for file.
			 */ 
			virtual void release() {};

			/** 
			  Each call retrieve next line from the file.
			  @param Ignored
			  @param oData Address of a pointer that will be set to point to read data after the call
			  @param oLength Address of a variable that will contain the size of the data after the call
			 */ 
			virtual bool read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength);

			/** 
			  Simply ignore anything that is sent.
			  @param Ignored
			  @param iData A pointer to data to be sent
			  @param iLength Size of the data to be sent
			 */ 
			virtual void write_data(unsigned char iEndPoint, unsigned char *iData, size_t iLength) {};
			/** 
			  Send control transfer to the source. 
			  @param iRequestType
			  @param iRequest
			  @param iValue
			  @param iIndex
			  @param iData
			  @param iLength
			 */ 
			 // TODO: doxygen !
			virtual void control_transfer(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength) {};

			virtual std::string getName() { return "HexdumpFile"; };

		protected:
			std::string _filename;
			std::vector<std::string> _inputStream;
			size_t _index;
	};
}

#endif
