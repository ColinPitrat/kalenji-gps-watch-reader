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
			void exportSession(const Session *iSession) override {};

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			void getSessionsDetails(SessionsMap *oSessions) override;

			std::string getName() override { return "Keymaze"; };
      DeviceId getDeviceId() override { return { 0x067B, 0x2303 }; };

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
