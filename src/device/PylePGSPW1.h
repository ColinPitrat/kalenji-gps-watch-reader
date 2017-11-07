#ifndef _DEVICE_KALENJI_HPP_
#define _DEVICE_KALENJI_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for PylePGSPW1 (and others ?)
	 */
	class PylePGSPW1 : public Device
	{
		public:
			PylePGSPW1() {};

			/** 
			  Initialize the device. 
			 */ 
			void init(const DeviceId& deviceId) override;

			void dump(unsigned char *data, int length);

			/** 
			  Read a full message doing as much roundtrips as necessary
			 */ 
			void readMessage(unsigned char **buffer, size_t *transferred);

			/** 
			  Release the device. 
			 */ 
			// TODO: Do something in it ?!
			void release() override {};

			/** 
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */ 
			void getSessionsList(SessionsMap *oSessions) override;

			/** 
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */ 
			void exportSession(Session *iSession) override {};

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			void getSessionsDetails(SessionsMap *oSessions) override;

			std::string getName() override { return "PylePGSPW1"; };
      DeviceId getDeviceId() override { return { 0x10C4, 0xEA61 }; };

		private:
			DECLARE_DEVICE(PylePGSPW1);

			static const int lengthDataDevice;
			static unsigned char dataDevice[];
			static const int lengthDataList;
			static unsigned char dataList[];
			static const int lengthDataEnd;
			static unsigned char dataEnd[];
			static unsigned char message[];
	};
}

#endif
