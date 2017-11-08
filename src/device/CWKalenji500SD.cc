#include "CWKalenji500SD.h"
#include <cstring>
#include <iomanip>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
	REGISTER_DEVICE(CWKalenji500SD);

	const int     CWKalenji500SD::lengthReset = 7;
	unsigned char CWKalenji500SD::dataReset[lengthReset] = { 0xA4, 0x01, 0x4A, 0x00, 0xEF, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthSetNetwork = 15;
	unsigned char CWKalenji500SD::dataSetNetwork[lengthSetNetwork] = { 0xA4, 0x09, 0x46, 0x00, 0xA8, 0xA4, 0x23, 0xB9, 0xF5, 0x5E, 0x63, 0xC1, 0x74, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthTransmitPower = 8;
	unsigned char CWKalenji500SD::dataTransmitPower[lengthTransmitPower] = { 0xA4, 0x02, 0x47, 0x00, 0x03, 0xE2, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthAssignChannel = 9;
	unsigned char CWKalenji500SD::dataAssignChannel[lengthAssignChannel] = { 0xA4, 0x03, 0x42, 0x00, 0x00, 0x00, 0xE5, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthChannelPeriod = 9;
	unsigned char CWKalenji500SD::dataChannelPeriod[lengthChannelPeriod] = { 0xA4, 0x03, 0x43, 0x00, 0x00, 0x08, 0xEC, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthChannelRFFreq = 8;
	unsigned char CWKalenji500SD::dataChannelRFFreq[lengthChannelRFFreq] = { 0xA4, 0x02, 0x45, 0x00, 0x32, 0xD1, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthSearchTimeout = 8;
	unsigned char CWKalenji500SD::dataSearchTimeout[lengthSearchTimeout] = { 0xA4, 0x02, 0x44, 0x00, 0xFF, 0x1D, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthChannelID = 11;
	unsigned char CWKalenji500SD::dataChannelID[lengthChannelID] = { 0xA4, 0x05, 0x51, 0x00, 0x00, 0x00, 0x01, 0x05, 0xF4, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthOpenChannel = 7;
	unsigned char CWKalenji500SD::dataOpenChannel[lengthOpenChannel] = { 0xA4, 0x01, 0x4B, 0x00, 0xEE, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthOpenRxChannel = 7;
	unsigned char CWKalenji500SD::dataOpenRxChannel[lengthOpenRxChannel] = { 0xA4, 0x01, 0x5B, 0x00, 0xFE, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthRequestMessage = 8;
	unsigned char CWKalenji500SD::dataRequestMessage51[lengthRequestMessage] = { 0xA4, 0x02, 0x4D, 0x00, 0x51, 0xBA, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataRequestMessage54[lengthRequestMessage] = { 0xA4, 0x02, 0x4D, 0x00, 0x54, 0xBF, 0x00, 0x00 };
	// I have really no idea of what should be in message for ack data ... I put a default one but this may fail !
	const int     CWKalenji500SD::lengthAckData = 15;
	unsigned char CWKalenji500SD::dataAckData[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x02, 0x32, 0x04, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataAckData2[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA3, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataAckData3[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataAckData4[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x09, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00 };
	// The one to get a session. Byte at index 6 must be set to session ID. Byte at index 12 must be xored with the session ID.
	unsigned char CWKalenji500SD::dataAckData5[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAF, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataAckData6[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5, 0x00, 0x00 };
	const int     CWKalenji500SD::lengthBurstData = 15;
	unsigned char CWKalenji500SD::dataBurstData[lengthBurstData] = { 0xA4, 0x09, 0x50, 0x00, 0x44, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00 };
	unsigned char CWKalenji500SD::dataBurstData2[lengthBurstData] = { 0xA4, 0x09, 0x50, 0xA0, 0x42, 0x4F, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00 };

	void CWKalenji500SD::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::endl);
	}

	bool CWKalenji500SD::receive(unsigned char iEndPoint, unsigned char **oData, size_t* oLength, char iMessage, char iMessageAnswered, char iError)
	{
		do
		{
			// TODO: add checks of read_data returned value to all calls in this file
			_dataSource->read_data(0x81, oData, oLength);
			DEBUG_CMD(std::cout << "Payload: ");
			DEBUG_CMD(dump(*oData, *oLength));
			size_t payloadLength;
			do
			{
				payloadLength = (int)(*oData)[1] + 4;
				DEBUG_CMD(dump(*oData, payloadLength));
				if( ((*oData)[2] == iMessage || iMessage == 0) && ((*oData)[4] == iMessageAnswered || iMessageAnswered == 0) )
				{
					return (*oData)[5] == iError;
				}
				*oLength -= payloadLength;
				*oData += payloadLength;
			}
			while(*oLength > 4 && *oLength >= ((size_t)(*oData)[1] + 4));
		} while( true );

		return false;
	}

	void CWKalenji500SD::init(const DeviceId& deviceId)
	{
		_dataSource->init(deviceId.vendorId, deviceId.productId);
		unsigned char *responseData;
		size_t transferred;

		// Step 1: Some control transfer, necessary to initialize the device ?
		{
			unsigned char data[256];
			/*
			unsigned char refdata_1[18]= { 0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0xCF, 0x0F, 0x08, 0x10, 0x00, 0x01, 0x01, 0x02, 0x03, 0x01 };
			unsigned char refdata_2[ 9]= { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x02, 0x80, 0x32 };
			unsigned char refdata_3[32]= { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x02, 0x80, 0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x02, 0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x01, 0x07, 0x05, 0x01, 0x02, 0x40, 0x00, 0x01 };
			unsigned char refdata_4[ 4]= { 0x04, 0x03, 0x09, 0x04 };
			unsigned char refdata_5[30]= { 0x1E, 0x03, 0x41, 0x00, 0x4E, 0x00, 0x54, 0x00, 0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, 0x53, 0x00, 0x74, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x32, 0x00, 0x00, 0x00 };
			unsigned char refdata_6[42]= { 0x2A, 0x03, 0x31, 0x00, 0x33, 0x00, 0x37, 0x00, 0x00, 0x00, 0x89, 0x00, 0xAD, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0xAD, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x53, 0x00, 0x49, 0x00, 0x00, 0x00, 0x42, 0x00 };
			*/

			// TODO: Check corresponding refdata
			_dataSource->control_transfer(0x80, 0x06, 0x100, 0x0,   data, 0x12);
			// refdata_1
			_dataSource->control_transfer(0x80, 0x06, 0x200, 0x0,   data, 0x9);
			// refdata_2
			_dataSource->control_transfer(0x80, 0x06, 0x200, 0x0,   data, 0x20);
			// refdata_3
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x302, 0x409, data, 0xFF);
			// refdata_5
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x303, 0x409, data, 0xFF);
			// refdata_6
			_dataSource->control_transfer(0x80, 0x06, 0x100, 0x0,   data, 0x12);
			// refdata_1
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x302, 0x409, data, 0xFF);
			// refdata_5
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x303, 0x409, data, 0xFF);
			// refdata_6
			_dataSource->control_transfer(0x80, 0x06, 0x100, 0x0,   data, 0x12);
			// refdata_1
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x302, 0x409, data, 0xFF);
			// refdata_5
			_dataSource->control_transfer(0x80, 0x06, 0x300, 0x0,   data, 0xFF);
			// refdata_4
			_dataSource->control_transfer(0x80, 0x06, 0x303, 0x409, data, 0xFF);
			// refdata_6
		}

		// Step 2: Initialize ANT protocol
		// First ask for channel capabilities
		DEBUG_CMD(std::cout << "Request message 54 for capabilities: " << std::endl);
		_dataSource->write_data(0x01, dataRequestMessage54, lengthRequestMessage);
		if(receive(0x81, &responseData, &transferred, 0x54))
		{
			/*
			int max_chan = responseData[3];
			int max_net = responseData[4];
			int std_opts = responseData[5];
			int adv_opts = responseData[6];
			int adv_opts2 = responseData[7];
			int max_data_chan = responseData[8];
			*/
		}
		else
		{
			std::cout << " !!! Wrong message received !!!" << std::endl;
		}
		DEBUG_CMD(std::cout << "Reset message (expect 6F answer): " << std::endl);
		// Then do a device reset using ANT function for it
		_dataSource->write_data(0x01, dataReset, lengthReset);
		if(!receive(0x81, &responseData, &transferred, 0x6F))
		{
			std::cout << "Reset of device failed" << std::endl;
		}
		// And  then we do the init sequence
		_dataSource->write_data(0x01, dataSetNetwork, lengthSetNetwork);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x46))
		{
			std::cout << "Setting network failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataTransmitPower, lengthTransmitPower);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x47))
		{
			std::cout << "Power transmition failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataAssignChannel, lengthAssignChannel);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x42))
		{
			std::cout << "Assigning data channel failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataChannelPeriod, lengthChannelPeriod);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x43))
		{
			std::cout << "Setting channel period failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataChannelRFFreq, lengthChannelRFFreq);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x45))
		{
			std::cout << "Setting RFFreq channel failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataSearchTimeout, lengthSearchTimeout);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x44))
		{
			std::cout << "Setting search timeout failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataChannelID, lengthChannelID);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x51))
		{
			std::cout << "Setting data channel ID failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataOpenChannel, lengthOpenChannel);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x4B))
		{
			std::cout << "Opening data channel failed" << std::endl;
		}
		_dataSource->write_data(0x01, dataOpenRxChannel, lengthOpenRxChannel);
		if(!receive(0x81, &responseData, &transferred, 0x40, 0x5B))
		{
			DEBUG_CMD(std::cout << "Data RX channel busy" << std::endl);
			// This can fail and return an error that requires asking for a channel with A4 02 4D 00 51 BA (<- asking for a sending of a A4 05 51)
			if(responseData[5] == 0x19)
			{
				_dataSource->write_data(0x01, dataRequestMessage51, lengthRequestMessage);
				receive(0x81, &responseData, &transferred, 0x51);
			}
		}
		for(int i = 0; i < 2; ++i)
		{
			_dataSource->write_data(0x01, dataAckData, lengthAckData);
			receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x05);
		}
		_dataSource->write_data(0x01, dataAckData2, lengthAckData);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x05);
		receive(0x81, &responseData, &transferred, 0x50, 0x43, 0x3B);
		receive(0x81, &responseData, &transferred, 0x50, 0x44, 0x84);

		_dataSource->write_data(0x01, dataBurstData, lengthBurstData);
		_dataSource->write_data(0x01, dataBurstData2, lengthBurstData);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x0A);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x05);
		receive(0x81, &responseData, &transferred, 0x50, 0x43, 0x3B);
		receive(0x81, &responseData, &transferred, 0x50, 0x44, 0x84);
		receive(0x81, &responseData, &transferred, 0x50, 0x4A, 0x29);

		_dataSource->write_data(0x01, dataBurstData, lengthBurstData);
		_dataSource->write_data(0x01, dataBurstData2, lengthBurstData);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x0A);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x05);
		_dataSource->write_data(0x01, dataAckData3, lengthAckData);
		receive(0x81, &responseData, &transferred, 0x40, 0x01, 0x05);
	}

	void CWKalenji500SD::release()
	{
		// At the end, we send acked data 6 and we receive one confirmation with code 5 and 2 with code 2. Then we send reset and receive 0x6F.
		unsigned char *responseData;
		size_t received;
		_dataSource->write_data(0x01, dataAckData6, lengthAckData);
		if(receive(0x81, &responseData, &received, 0x40, 0x01, 0x05))
		{
			DEBUG_CMD(std::cout << "Ack for acked data 6" << std::endl);
		}
		else
		{
			std::cout << "Ack received but with unexpected message code. I don't know what this means." << std::endl;
		}
		for(int i = 0; i < 2; ++i)
		{
			DEBUG_CMD(std::cout << "Ack loop " << i << std::endl);
			if(receive(0x81, &responseData, &received, 0x40, 0x01, 0x02))
			{
				DEBUG_CMD(std::cout << "Ack 02 for acked data 6" << std::endl);
			}
			else
			{
				std::cout << "Ack received but with unexpected message code. I don't know what this means." << std::endl;
			}
		}
		DEBUG_CMD(std::cout << "Final reset" << std::endl);
		_dataSource->write_data(0x01, dataReset, lengthReset);
		_dataSource->read_data(0x81, &responseData, &received);
		if(responseData[2] != 0x6F || responseData[3] != 0x20)
		{
			std::cout << "Unexpected answer to final ack:" << std::endl;
			dump(responseData, received);
		}
		DEBUG_CMD(std::cout << "CWKalenji500SD released" << std::endl);
	}

	void CWKalenji500SD::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "Get sessions list !" << std::endl);
		size_t received = 0;
		unsigned char *responseData;
		std::vector<unsigned char> buffer;
		do
		{
			_dataSource->read_data(0x81, &responseData, &received);
			DEBUG_CMD(dump(responseData, received));
			if(responseData[2] == 0x4E)
			{
				// Ignore broadcast messages
				//DEBUG_CMD(std::cout << "Received 0x4E - ignored" << std::endl);
			}
			else if(responseData[2] == 0x50 && (responseData[3] == 0x00 || responseData[3] == 0x20 || responseData[3] == 0x40 || responseData[3] == 0x60 || responseData[3] == 0xA0))
			{
				//DEBUG_CMD(std::cout << "Received 0x50 - 0x[0246A]0" << std::endl);
				for(size_t i = 0; i < received - 5; ++i)
				{
					buffer.push_back(responseData[i+4]);
				}
			}
		} while (responseData[2] != 0x50 || responseData[3] != 0xA0);
		int nb_sessions = (buffer.size()  - 16) / 24;
		for(int i = 0; i < nb_sessions; ++i)
		{
			DEBUG_CMD(std::cout << "Decode summary of session " << i << std::endl);
			// Decoding of basic info about the session
			unsigned char *line = &buffer[24*i+16];
			SessionId id = SessionId(line, line+1);
			uint32_t num = line[0];

			tm time;
			memset(&time, 0, sizeof(time));
			// In tm, year is year since 1900. GPS returns year since 2000
			time.tm_year  = 100 + (line[5] % 16) + 10*(line[5] / 16);
			// In tm, month is between 0 and 11.
			time.tm_mon   = (line[4] % 16) + 10*(line[4] / 16) - 1;
			time.tm_mday  = (line[3] % 16) + 10*(line[3] / 16);
			time.tm_hour  = (line[2] % 16) + 10*(line[2] / 16);
			time.tm_min   = (line[1] % 16) + 10*(line[1] / 16);
			time.tm_sec   = 0;
			time.tm_isdst = -1;

			double duration = 0;
			uint32_t distance = 0;
			uint32_t nb_laps = line[13];

			Session mySession(id, num, time, 0, duration, distance, nb_laps);
			oSessions->insert(SessionsMapElement(id, mySession));
		}
	}

	void CWKalenji500SD::getSessionsDetails(SessionsMap *oSessions)
	{
		size_t received = 0;
		unsigned char *responseData;
		_dataSource->write_data(0x01, dataAckData4, lengthAckData);
		receive(0x81, &responseData, &received, 0x40, 0x01, 0x05);
		receive(0x81, &responseData, &received, 0x50, 0x43, 0x3B);
		receive(0x81, &responseData, &received, 0x50, 0x44, 0x89);
		// Issue 5: it appears that the 21 can be 23. Maybe other value sometime ?
		// Let's just be happy with a third 50 !
		//receive(0x81, &responseData, &received, 0x50, 0x21, 0x4A);
		receive(0x81, &responseData, &received, 0x50);
		for(auto& session : *oSessions)
		{
			std::cout << "Retrieve session " << (int) session.second.getId().back() << std::endl;
			time_t current_time = session.second.getTime();
			dataAckData5[6] = session.second.getId().back();
			dataAckData5[12] = 0xAF ^ dataAckData5[6];
			_dataSource->write_data(0x01, dataAckData5, lengthAckData);
			receive(0x81, &responseData, &received, 0x40, 0x01, 0x05);

			std::vector<unsigned char> buffer;
			int ligne = 0;
			int nb_laps = session.second.getNbLaps();
			int no_burst = 0;
			do
			{
				DEBUG_CMD(std::cout << "Retrieving data" << std::endl);
				_dataSource->read_data(0x81, &responseData, &received);
				DEBUG_CMD(dump(responseData, received));
				if(responseData[2] == 0x4E)
				{
					// We consider that if 3 consecutive broadcast message arrive, we don't have anymore burst
					// There's certainly a better way but I didn't find it yet !
					no_burst++;
					// Ignore broadcast messages
					//DEBUG_CMD(std::cout << "Received 0x4E - ignored" << std::endl);
				}
				else if(responseData[2] == 0x50 && 
					(responseData[3] == 0x00 || responseData[3] == 0x20 || 
					 responseData[3] == 0x40 || responseData[3] == 0x60 || 
					 responseData[3] == 0xA0 || responseData[3] == 0xC0 || 
					 responseData[3] == 0xE0 ))
				{
					ligne++;
					/*
					if(responseData[3] == 0xE0 || responseData[3] == 0xA0)
						nb_laps = (ligne - 2) / 2;
					*/
					no_burst = 0;
					//DEBUG_CMD(std::cout << "Received 0x50 - 0x[0246AE]0" << std::endl);
					for(size_t i = 0; i < received - 5; ++i)
					{
						buffer.push_back(responseData[i+4]);
					}
				}
			} while (no_burst < 3);
			// I first thought it ends with a line beginning with C0 and ending with FF 
			// while (responseData[2] != 0x50 || responseData[3] != 0xC0 || responseData[11] != 0xFF );

			// end point on end of data
			size_t end = buffer.size();
			size_t offset = 16;
			double total_duration = 0;
			unsigned int total_distance = 0;
			for(int i = 0; i < nb_laps; ++i)
			{
				DEBUG_CMD(std::cout << "Lap " << i << std::endl);
				unsigned char *lap = &buffer[offset];
				double duration = ((lap[1] % 16) + 10.0*(lap[1] / 16)) / 100 
				                + ((lap[2] % 16) + 10.0*(lap[2] / 16))
				             + 60*((lap[3] % 16) + 10.0*(lap[3] / 16));
				unsigned int distance = lap[5] + 256 * lap[6] + 256 * 256 * lap[7];
				unsigned int avg_hr = lap[8];
				unsigned int max_hr = lap[9];
				unsigned int max_speed = lap[10] + 256 * lap[11];
				total_distance += distance;
				total_duration += duration;
				offset += 16;
				auto l = new Lap(0, 0, duration, distance, max_speed, 0, max_hr, avg_hr, 0, 0, 0, 0);
				session.second.addLap(l);
			}
			session.second.setDistance(total_distance);
			session.second.setDuration(total_duration);
			offset += 16;
			int nb_points = 0;
			std::cout << std::dec; 
			std::list<Lap*> laps = session.second.getLaps();
			auto it_lap = laps.begin();
			double cumulated_duration = 0;
			DEBUG_CMD(std::cout << "Points ..." << std::endl);
			while(offset + 3 < end)
			{
				DEBUG_CMD(std::cout << ".");
				// Every 42 points (or 16 lines) there is a 0xFF 0xFF finishing the line
				if(nb_points > 0 && nb_points % 42 == 0) offset += 2;
				if(nb_points > 0 && nb_points % 168 == 0) offset += 16;

				// Decoding of basic info about the session
				unsigned char *point = &buffer[offset];
				// Session end with Fx for the high weight byte of speed
				if(point[2] >= 0xF0) break;

				uint32_t hr = point[0];
				double speed = (point[1] + 256.0*point[2]) / 100;

				auto p = new Point(FieldUndef, FieldUndef, FieldUndef, speed, current_time + 5*nb_points, 0, hr, 3);
				session.second.addPoint(p);

				if(nb_points == 0)
				{
					DEBUG_CMD(std::cout << "First point is start point of first lap");
					(*it_lap)->setStartPoint(p);
					(*it_lap)->setFirstPointId(nb_points);
					cumulated_duration += (*it_lap)->getDuration();
				}
				while(5*nb_points >= cumulated_duration)
				{
					DEBUG_CMD(std::cout << "Points are for next lap");
					it_lap++;
					cumulated_duration += (*it_lap)->getDuration();
					if(it_lap != laps.end())
					{
						(*it_lap)->setStartPoint(p);
						(*it_lap)->setFirstPointId(nb_points);
						(*it_lap)->setEndPoint(p);
						(*it_lap)->setLastPointId(nb_points);
					}
				}
				if(it_lap != laps.end())
				{
					(*it_lap)->setEndPoint(p);
					(*it_lap)->setLastPointId(nb_points);
				}

				offset += 3;
				nb_points++;
			}
			DEBUG_CMD(std::cout << std::endl << "End of points ..." << std::endl);
		}
	}

	void CWKalenji500SD::exportSession(const Session *iSession)
	{
		std::cerr << "Unsupported export of session with CW Kalenji 500 SD" << std::endl;
	}
}
