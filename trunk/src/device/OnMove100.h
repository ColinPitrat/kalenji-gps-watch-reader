#ifndef _DEVICE_GEONAUTE100_HPP_
#define _DEVICE_GEONAUTE100_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for OnMove 100 GPS watch. 
	 */
	class OnMove100 : public Device
	{
		public:
			OnMove100() {};

			/** 
			  Dump a message on stdout
			 */ 
			virtual void dump(unsigned char *iData, int iLength);

			/** 
			  Initialize the device. 
			 */ 
			virtual void init();

			/** 
			  Release the device. 
			 */ 
			virtual void release();

			/** 
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */ 
			virtual void getSessionsList(SessionsMap *oSessions);

			/** 
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */ 
			virtual void exportSession(Session *iSession);

			/** 
			  Retrieve details of sessions passed in input/output map
			  @param ioSessions The sessions to import, the function enrich it with details
			 */ 
			virtual void getSessionsDetails(SessionsMap *ioSessions);

			virtual std::string getName() { return "OnMove100"; };
			virtual unsigned int getVendorId() { return 0x10c4; };
			virtual unsigned int getProductId() { return 0xea61; };

		private:
			double decodeCoordinate(unsigned char *buffer);

			std::string getPath();
			DECLARE_DEVICE(OnMove100);
			static const int lengthGetData;
			static unsigned char getData[];
			static const int lengthDeviceInit;
			static unsigned char deviceInit[];
	};
}

#endif
