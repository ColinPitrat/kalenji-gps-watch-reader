#ifndef _DEVICE_KALENJI_HPP_
#define _DEVICE_KALENJI_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for Keymaze 700 (and others ?)
	 */
	class Keymaze : public Device
	{
		public:
			Keymaze() {};

			/** 
			  Initialize the device. 
			 */ 
			virtual void init();

			/** 
			  Read a full message doing as much roundtrips as necessary
			 */ 
			void readMessage(unsigned char **buffer, size_t *transferred);

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
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */ 
			virtual void exportSession(Session *iSession) {};

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			virtual void getSessionsDetails(SessionsMap *oSessions);

			virtual std::string getName() { return "Keymaze"; };
			virtual unsigned int getVendorId() { return 0x067B; };
			virtual unsigned int getProductId() { return 0x2303; };

		private:
			DECLARE_DEVICE(Keymaze);

			static const int lengthDataDevice;
			static unsigned char dataDevice[];
			static const int lengthDataList;
			static unsigned char dataList[];
			static const int lengthDataMore;
			static unsigned char dataMore[];
			static unsigned char message[];
	};
}

#endif
