#ifndef _DEVICE_GEONAUTE710_HPP_
#define _DEVICE_GEONAUTE710_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for OnMove 710 GPS watch. 
	 */
	class OnMove200 : public Device
	{
		public:
			OnMove200() {};

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

			void dumpInt2(std::ostream &oStream, unsigned int iInt);
			void dumpInt4(std::ostream &oStream, unsigned int iInt);
			void dumpString(std::ostream &oStream, const std::string &iString, size_t iLength);
			/** 
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */ 
			virtual void exportSession(Session *iSession);

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			virtual void getSessionsDetails(SessionsMap *oSessions);
			
			virtual void parseOMHFile(const unsigned char* bytes, Session* session);
			virtual void parseOMDFile(const unsigned char* bytes, int length, Session* session);

      int bytesToInt2(unsigned char b0, unsigned char b1);
      int bytesToInt4(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
      unsigned char* readAllBytes(std::string filename, size_t& size);
      bool fileExists(std::string filename);

			virtual std::string getName() { return "OnMove200"; };
			virtual unsigned int getVendorId() { return 0x283E; };
			virtual unsigned int getProductId() { return 0x0007; };

		private:
			std::string getPath();
			DECLARE_DEVICE(OnMove200);
	};
}

#endif
