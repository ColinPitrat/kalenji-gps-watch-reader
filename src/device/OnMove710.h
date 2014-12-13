#ifndef _DEVICE_GEONAUTE710_HPP_
#define _DEVICE_GEONAUTE710_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for OnMove 710 GPS watch. 
	 */
	class OnMove710 : public Device
	{
		public:
			OnMove710() {};

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
			
			virtual void parseGHTFile(const unsigned char* bytes,Session* session);
			virtual void parseGHLFile(unsigned char* bytes, int length, Session* session);
			virtual void parseGHPFile(unsigned char* bytes, int length, Session* session);


			virtual std::string getName() { return "OnMove710"; };
			virtual unsigned int getVendorId() { return 0x283E; };
			virtual unsigned int getProductId() { return 0x0007; };

		private:
			std::string getPath();
			DECLARE_DEVICE(OnMove710);
	};
}

#endif
