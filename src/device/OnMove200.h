#ifndef _DEVICE_GEONAUTE200_HPP_
#define _DEVICE_GEONAUTE200_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for OnMove 200 GPS watch.
	 */
	class OnMove200 : public Device
	{
		public:
			OnMove200() {};

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

			void dumpInt2(std::ostream &oStream, unsigned int iInt);
			void dumpInt4(std::ostream &oStream, unsigned int iInt);
			void dumpString(std::ostream &oStream, const std::string &iString, size_t iLength);
			/**
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */
			void exportSession(const Session *iSession) override;

			/**
			  Send data to the source.
			  @param iData A pointer to data to be sent
			 */
			void getSessionsDetails(SessionsMap *oSessions) override;
			
			void parseOMHFile(const unsigned char* bytes, Session* session);
			void parseOMDFile(const unsigned char* bytes, int length, Session* session);

			int bytesToInt2(unsigned char b0, unsigned char b1);
			int bytesToInt4(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
			unsigned char* readAllBytes(const std::string& filename, size_t& size);
			bool fileExists(const std::string& filename);

			std::string getName() override { return "OnMove200"; };
			DeviceId getDeviceId() override { return { 0x283E, 0x0009 }; };

		private:
			std::string getPath();
			DECLARE_DEVICE(OnMove200);
	};
}

#endif
