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
			void dump(unsigned char *data, int length);

			/**
			  Initialize the device.
			 */
			void init(const DeviceId& deviceId) override;

			/**
			  Release the device.
			 */
			void release() override;

			/**
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */
			void getSessionsList(SessionsMap *oSessions) override;

			/**
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */
			void exportSession(const Session *iSession) override;

			/**
			  Retrieve details of sessions passed in input/output map
			  @param ioSessions The sessions to import, the function enrich it with details
			 */
			void getSessionsDetails(SessionsMap *oSessions) override;

			std::string getName() override { return "Navbike400"; };
			DeviceId getDeviceId() override { return { 0x10C4, 0xEA61 }; };

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
