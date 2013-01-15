#ifndef _DEVICE_KALENJI_HPP_
#define _DEVICE_KALENJI_HPP_

#include "Device.h"

namespace device
{
	/**
	  The interface for W Kalenji 300 GPS and CW Kalenji 700 GPS. 
	 */
	class CWKalenji500SD : public Device
	{
		public:
			CWKalenji500SD() {};

			/** 
			  Dump a message on stdout
			 */ 
			virtual void dump(unsigned char *iData, int iLength);

			/** 
			  Receive an answer, checking it's value and ignoring broadcast messages
			 */ 
			virtual bool receive(unsigned char iEndPoint, unsigned char **oData, size_t* oLength, char iMessage = 0, char iMessageAnswered = 0, char iError = 0);

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
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			virtual void getSessionsDetails(SessionsMap *oSessions);

			virtual std::string getName() { return "CWKalenji500SD"; };
			virtual unsigned int getVendorId() { return 0x0FCF; };
			virtual unsigned int getProductId() { return 0x1008; };

		private:
			DECLARE_DEVICE(CWKalenji500SD);

			static const int lengthReset;
			static unsigned char dataReset[];
			static const int lengthSetNetwork;
			static unsigned char dataSetNetwork[];
			static const int lengthTransmitPower;
			static unsigned char dataTransmitPower[];
			static const int lengthAssignChannel;
			static unsigned char dataAssignChannel[];
			static const int lengthChannelPeriod;
			static unsigned char dataChannelPeriod[];
			static const int lengthChannelRFFreq;
			static unsigned char dataChannelRFFreq[];
			static const int lengthSearchTimeout;
			static unsigned char dataSearchTimeout[];
			static const int lengthChannelID;
			static unsigned char dataChannelID[];
			static const int lengthOpenChannel;
			static unsigned char dataOpenChannel[];
			static const int lengthOpenRxChannel;
			static unsigned char dataOpenRxChannel[];
			static const int lengthRequestMessage;
			static unsigned char dataRequestMessage51[];
			static unsigned char dataRequestMessage54[];
			static const int lengthAckData;
			static unsigned char dataAckData[];
			static unsigned char dataAckData2[];
			static unsigned char dataAckData3[];
			static unsigned char dataAckData4[];
			static unsigned char dataAckData5[];
			static unsigned char dataAckData6[];
			static const int lengthBurstData;
			static unsigned char dataBurstData[];
			static unsigned char dataBurstData2[];
	};
}

#endif
