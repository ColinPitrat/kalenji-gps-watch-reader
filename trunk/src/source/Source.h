#ifndef _SOURCE_SOURCE_HPP_
#define _SOURCE_SOURCE_HPP_

// TODO: Only for uint32_t, this is sad ...
#include <libusb.h>
#include <string>

namespace source
{
	/**
	  An interface for a source of data. Typically, USB or file.
	 */
	class Source
	{
		public:
			/** 
			  Initialize the source. Any action that needs to be taken before using the source.
			 */ 
			virtual void init(uint32_t vendorId, uint32_t productId) = 0;

			/** 
			  Release the source. Any action that needs to be taken once the source is not needed anymore.
			 */ 
			virtual void release() = 0;

			/** 
			  Read data from source. Handle associated memory. 
			  @param oData Address of a pointer that will be set to point to read data after the call
			  @param oLength Address of a variable that will contain the size of the data after the call
			 */ 
			virtual void read_data(unsigned char **oData, size_t *oLength) = 0;
			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			  @param iLength Size of the data to be sent
			 */ 
			virtual void write_data(unsigned char *iData, size_t iLength) = 0;

			virtual std::string getName() = 0;
	};
}

#endif
