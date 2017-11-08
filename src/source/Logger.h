#ifndef _SOURCE_LOGGER_HPP_
#define _SOURCE_LOGGER_HPP_

#include <string>
#include "Source.h"

namespace source
{
	/**
	  A class to handle transparent logging of another source
	 */
	class Logger : public Source
	{
		public:
			Logger(Source *source, std::string logfilename) : _logfilename(std::move(logfilename)), _truesource(source) {};
			/** 
			  Initialize the source. Any action that needs to be taken before using the source.
			 */ 
			void init(uint32_t vendorId, uint32_t productId) override;

			/** 
			  Release the source. Any action that needs to be taken once the source is not needed anymore.
			 */ 
			void release() override;

			/** 
			  Read data from source. Handle associated memory. 
			  @param iEndPoint Ignored
			  @param oData Address of a pointer that will be set to point to read data after the call
			  @param oLength Address of a variable that will contain the size of the data after the call
			 */ 
			bool read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength) override;

			/** 
			  Send data to the source. 
			  @param iEndPoint Ignored
			  @param iData A pointer to data to be sent
			  @param iLength Size of the data to be sent
			 */ 
			void write_data(unsigned char iEndPoint, unsigned char *iData, size_t iLength) override;
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
			void control_transfer(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength) override;

			std::string getName() override { return "Logger"; };

		protected:
			std::string _logfilename;
			Source *_truesource;
	};
}

#endif
