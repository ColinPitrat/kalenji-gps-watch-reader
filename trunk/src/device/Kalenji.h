#ifndef _DEVICE_KALENJI_HPP_
#define _DEVICE_KALENJI_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for W Kalenji 300 GPS and CW Kalenji 700 GPS. 
	 */
	class Kalenji : public Device
	{
		public:
			Kalenji() {};

			/** 
			  Initialize the device. 
			 */ 
			virtual void init();

			/** 
			  Release the device. 
			 */ 
			// TODO: Do something in it ?!
			virtual void release() {};

			/** 
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */ 
			virtual void getSessionsList(SessionsMap *oSessions);

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			virtual void getSessionsDetails(SessionsMap *oSessions);

			virtual std::string getName() { return "Kalenji"; };

		private:
			DECLARE_DEVICE(Kalenji);

			static const int lengthDataDevice;
			static unsigned char dataDevice[];
			static const int lengthDataList;
			static unsigned char dataList[];
			static const int lengthDataMore;
			static unsigned char dataMore[];
	};
}

#endif
