#include "CWKalenji500SD.h"
#include "../Common.h"
#include <cstring>
#include <iomanip>

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
	// I have really no idea of what should be in message for ack data ... I put a default one but this may fail !
	const int     CWKalenji500SD::lengthAckData = 15;
	unsigned char CWKalenji500SD::dataAckData[lengthAckData] = { 0xA4, 0x09, 0x4F, 0x00, 0x44, 0x02, 0x32, 0x04, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00 };

	void CWKalenji500SD::init()
	{
		_dataSource->init(0x0FCF, 0x1008);
		unsigned char *responseData;
		size_t transferred;
		// Don't know if it's mandatory, but it seems that the device sends 17 input messages
		for(int i = 0; i < 17; ++i)
		{
			_dataSource->read_data(&responseData, &transferred);
		}
		// Then we do a device reset using ANT function for it
		_dataSource->write_data(dataReset, lengthReset);
		_dataSource->read_data(&responseData, &transferred);
		// We can then do the init sequence
		// TODO: we should always check that answer is A2 03 40 00 XY 00 CS with XY = dataXxxx[2] and CS = checksum
		_dataSource->write_data(dataSetNetwork, lengthSetNetwork);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataTransmitPower, lengthTransmitPower);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataAssignChannel, lengthAssignChannel);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataChannelPeriod, lengthChannelPeriod);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataChannelRFFreq, lengthChannelRFFreq);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataSearchTimeout, lengthSearchTimeout);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataChannelID, lengthChannelID);
		_dataSource->read_data(&responseData, &transferred);
		_dataSource->write_data(dataOpenChannel, lengthOpenChannel);
		_dataSource->read_data(&responseData, &transferred);
		// This can fail and return an error that requires asking for a channel with A4 02 4D 00 51 BA (<- asking for a sending of a A4 05 51)
		_dataSource->write_data(dataOpenRxChannel, lengthOpenRxChannel);
		_dataSource->read_data(&responseData, &transferred);
		if(responseData[5] == 0x19)
		{
			_dataSource->write_data(dataRequestMessage51, lengthRequestMessage);
			_dataSource->read_data(&responseData, &transferred);
		}
	}

	void CWKalenji500SD::release()
	{
		unsigned char *responseData;
		size_t transferred;
		_dataSource->write_data(dataReset, lengthReset);
		_dataSource->read_data(&responseData, &transferred);
	}

	void CWKalenji500SD::getSessionsList(SessionsMap *oSessions)
	{
		// Test code for now, just read what comes in
		size_t received = 0;
		unsigned char *responseData;
		do
		{
			// TODO: Use more info from this first call (some data global to the session: calories, grams, ascent, descent ...)
			_dataSource->read_data(&responseData, &received);
			if(responseData[2] == 0x4E)
			{
				_dataSource->write_data(dataAckData, lengthAckData);
			}
		} while(responseData[0] != 0x8A);
	}

	void CWKalenji500SD::getSessionsDetails(SessionsMap *oSessions)
	{
		std::cerr << "Unsupported getSessionsDetails with CW Kalenji 500 SD" << std::endl;
	}

	void CWKalenji500SD::exportSession(Session *iSession)
	{
		std::cerr << "Unsupported export of session with CW Kalenji 500 SD" << std::endl;
	}
}
