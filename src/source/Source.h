#ifndef _SOURCE_SOURCE_HPP_
#define _SOURCE_SOURCE_HPP_

#include <stdint.h>
#include <string>

namespace source
{
	/**
	  An interface for a source of data. Typically, USB or file.
	 */
	class Source
	{
		public:
			virtual ~Source() = default;
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
			  @param iEndPoint End Point for USB devices
			  @param oData Address of a pointer that will be set to point to read data after the call
			  @param oLength Address of a variable that will contain the size of the data after the call
			 */ 
			virtual bool read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength) = 0;
			/** 
			  Send data to the source. 
			  @param iEndPoint End Point for USB devices
			  @param iData A pointer to data to be sent
			  @param iLength Size of the data to be sent
			 */ 
			virtual void write_data(unsigned char iEndPoint, unsigned char *iData, size_t iLength) = 0;
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
			virtual void control_transfer(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength) = 0;

			virtual std::string getName() = 0;
	};
}

#endif
