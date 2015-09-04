#ifndef _DEVICE_NAVBIKE_400_HPP_
#define _DEVICE_NAVBIKE_400_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for Navbike 400 GPS watch. 
	 */
	class Navbike400 : public Device
	{
		public:
			Navbike400() {};

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

			virtual std::string getName() { return "Navbike400"; };
			virtual unsigned int getVendorId() { return 0x10c4; };
			virtual unsigned int getProductId() { return 0xea61; };

		private:
			double decodeCoordinate(unsigned char *buffer);

			std::string getPath();
			DECLARE_DEVICE(Navbike400);
			static const int lengthGetData;
			static unsigned char getData[];
			static const int lengthDeviceInit;
			static unsigned char deviceInit[];
	};
}

#endif
