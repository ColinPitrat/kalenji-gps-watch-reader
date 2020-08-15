#include "PylePGSPW1_v2.h"
#include <cstring>
#include <iomanip>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

#define RESPONSE_BUFFER_SIZE 65536

namespace device
{
	REGISTER_DEVICE(PylePGSPW1_v2);

	const int     PylePGSPW1_v2::lengthDataDevice = 8;
	unsigned char PylePGSPW1_v2::dataDevice[lengthDataDevice] = { 0x56, 0x5A, 0x01, 0x01, 0x00, 0x00, 0xFF, 0xFF };
	const int     PylePGSPW1_v2::lengthDataList = 8;
	unsigned char PylePGSPW1_v2::dataList[lengthDataList] = { 0x56, 0x5A, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00 };
	const int     PylePGSPW1_v2::lengthDataEnd = 8;
	unsigned char PylePGSPW1_v2::dataEnd[lengthDataEnd] = { 0x56, 0x5A, 0x01, 0x02, 0x00, 0x00, 0xFF, 0xFF };
	unsigned char PylePGSPW1_v2::message[RESPONSE_BUFFER_SIZE];

	void PylePGSPW1_v2::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::dec << std::endl);
	}

	void PylePGSPW1_v2::readMessage(unsigned char **buffer, size_t *size)
	{
		DEBUG_CMD(std::cout << "PylePGSPW1_v2::readMessage()" << std::endl);
		unsigned char* responseData;
		*size = 0;
		size_t transferred = 0;
		size_t full_size = 0;
		// full_size + 4 because there are 4 additional bytes to the payload (1B for header, 2B for size and 2B for checksum)
		do
		{
			_dataSource->read_data(0x81, &responseData, &transferred);
			memcpy(&(message[*size]), responseData, transferred);
			*size += transferred;
			if(full_size == 0 && (*size) >= 8)
				full_size = ((message[5] << 8) + message[4]) << 3;
		}
		while((*size) < (full_size + 8) || (*size) < 8);
		DEBUG_CMD(std::cout << "Read a total size of " << *size << " - expected: " << full_size + 8 << std::endl;);
		*buffer = message;
	}

	void PylePGSPW1_v2::init(const DeviceId& deviceId)
	{
		DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - init device" << std::endl);
		_dataSource->init(deviceId.vendorId, deviceId.productId);
		unsigned char *responseData;
		size_t transferred;
		// Step 1: Some control transfer, necessary to initialize the device ?
		{
			unsigned char data[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			unsigned char dataIn[256];
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 1" << std::endl);
			_dataSource->control_transfer(0x40, 0x00, 0xFFFF, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 2" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x2000, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 3 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0xFF, 0x370B, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 4" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0180, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 5 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0xFF, 0x370B, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 6" << std::endl);
			_dataSource->control_transfer(0x40, 0x03, 0x0801, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 7" << std::endl);
			_dataSource->control_transfer(0x41, 0x13, 0x0000, 0x0, data, 0x10);
			DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - vendor device 8" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0x000C, 0x0, data, 0x0);
		}
		DEBUG_CMD(std::cout << "PylePGSPW1_v2::init() - send hello message" << std::endl);
		_dataSource->write_data(0x01, dataDevice, lengthDataDevice);
		readMessage(&responseData, &transferred);
	}

	void PylePGSPW1_v2::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "PylePGSPW1_v2::getSessionsList()" << std::endl);
		unsigned char *responseData;
		size_t received;
		Session *currentSession = nullptr;
		int32_t numSess = 0;
		bool first_lap = false;
		time_t current_time = 0;
		for (int m = 0; m < 100; m++)
		{
			dataList[6] = m & 0xFF;
			dataList[7] = (m & 0xFF00) >> 8;
			_dataSource->write_data(0x01, dataList, lengthDataList);
			readMessage(&responseData, &received);

			if(responseData[0] != 0x56 || responseData[1] != 0xA5)
			{
				std::cerr << "Unexpected header for getList: " << std::hex << (int) responseData[0] << std::dec << std::endl;
				// TODO: Throw an exception
			}
			// 8 bytes per point
			unsigned int offset = (m == 0) ? 24 : 8;
			size_t nbLines = (received - offset) / 8;
			if(nbLines * 8 != (received - offset))
			{
				std::cerr << "Size is not a multiple of 8 in getList: size=" << received << std::endl;
				// TODO: Throw an exception
			}
			DEBUG_CMD(std::cout << "Got " << nbLines << " lines" << std::endl);
			for(size_t i = 0; i < nbLines; ++i)
			{
				// Decoding of basic info about the session
				unsigned char *line = &responseData[8*i + offset];
				DEBUG_CMD(
						std::cout << "Line: " << std::hex
						<< static_cast<int>(line[0]) << " "
						<< static_cast<int>(line[1]) << " "
						<< static_cast<int>(line[2]) << " "
						<< static_cast<int>(line[3]) << " "
						<< static_cast<int>(line[4]) << " "
						<< static_cast<int>(line[5]) << " "
						<< static_cast<int>(line[6]) << " "
						<< static_cast<int>(line[7])
						<< std::dec << std::endl;
						);
				// Header of session
				if(line[0] == 0xFF && line[1] == 0xFF && line[2] == 0xFF && line[3] == 0xFF && line[4] == 0xFF && line[5] == 0xFF && line[6] == 0xFF && line[7] == 0xFF)
				{
					// Ignore
					DEBUG_CMD(std::cout << "Skip" << std::endl);
				}
				else if(line[0] == 0xFF && line[1] == 0xFF && line[2] == 0xFF && line[3] == 0xFF && line[4] == 0xFF && line[5] == 0xFF)
				{
					DEBUG_CMD(std::cout << "Session start" << std::endl);
					first_lap = true;
					SessionId id = SessionId(line+6, line+8);
					Session mySession;
					currentSession = &(oSessions->insert(SessionsMapElement(id, mySession)).first->second);
					currentSession->setId(id);
					currentSession->setNum(numSess);
					numSess++;
				}
				// Header of lap
				else if(line[0] == 0xFF && line[1] == 0xFF && (line[2] == 0xE0 || line[2] == 0xFE))
				{
					tm time;
					// In tm, year is year since 1900. GPS returns year since 2000
					time.tm_year = 100 + (line[6] & 0x7F);
					// In tm, month is between 0 and 11.
					time.tm_mon = ((line[6] & 0x80) >> 7) + ((line[7] & 0x07) << 1) - 1;
					time.tm_mday = (line[7] & 0xF8) >> 3;
					time.tm_hour = line[3];
					time.tm_min = line[4];
					time.tm_sec = line[5];
					time.tm_isdst = -1;
					DEBUG_CMD(std::cout << "Header for lap " << (uint32_t) line[6] << "-" << (uint32_t) line[7] << " = " << time.tm_year << "-" << time.tm_mon << "-" << time.tm_mday << std::endl);

					current_time = mktime(&time);

					if(first_lap)
					{
						currentSession->setTime(time);
						first_lap = false;
					}

					// TODO: Code lap decoding and addition
				}
				// Cardio of the past 5 points
				else if(line[0] == 0xFF && line[1] == 0xE0)
				{
					DEBUG_CMD(std::cout << "Cardio (not decoded yet)" << std::endl);
					// TODO: Cardio decoding and filling the last 5 points
				}
				// Point
				else
				{
					// TODO: Check formulas (in particular negative lat and positive lon) + time between point (configurable ?)
					double lat = ((line[0] << 16) + (line[1] << 8) + line[2]) / 60000.0;
					double lon = ((line[3] << 16) + (line[4] << 8) + line[5]) / 60000.0;
					int16_t alt = ((line[7] << 8) + line[6]) - 2000;
					DEBUG_CMD(std::cout << "Point (" << lat << ", " << lon << ", " << alt << ")" << std::endl);
					auto point = new Point(lat, lon, alt, FieldUndef, current_time, 0, 0, 3);
					currentSession->addPoint(point);
					current_time += 10;
				}
			}
		}
		_dataSource->write_data(0x01, dataEnd, lengthDataEnd);
		readMessage(&responseData, &received);
	}

	void PylePGSPW1_v2::getSessionsDetails(SessionsMap *oSessions)
	{
		// TODO: do nothing
		unsigned char data[256] = { 0 };
		DEBUG_CMD(std::cout << "PylePGSPW1_v2::getSessionsDetails() - closing interface 1" << std::endl);
		_dataSource->control_transfer(0x40, 0x00, 0x0000, 0x0, data, 0x0);
	}
}
