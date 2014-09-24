#include "OnMove100.h"
#include "../Common.h"
#include <cstring>
#include <iostream>
#include <iomanip>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
	REGISTER_DEVICE(OnMove100);

	const int     OnMove100::lengthDeviceInit = 32;
	unsigned char OnMove100::deviceInit[lengthDeviceInit] = { 0xee, 0xee, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	const int     OnMove100::lengthGetData = 32;
	unsigned char OnMove100::getData[lengthGetData] = { 0x08, 0x08, 0xaa, 0xaa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

	void OnMove100::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::endl);
	}

	void OnMove100::init()
	{
		DEBUG_CMD(std::cout << "OnMove100::init() - init device" << std::endl);
		_dataSource->init(getVendorId(), getProductId());
		// Step 1 - USB to UART initialization ?
		{
			unsigned char data[256] = { 0 };
			unsigned char dataIn[256];

			DEBUG_CMD(std::cout << "OnMove100::init() - step 1" << std::endl);
			_dataSource->control_transfer(0x0, 0x9, 0x0, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 2" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x100, 0x0, dataIn, 18);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 3" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x300, 0x0, dataIn, 256);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 4" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x302, 0x409, dataIn, 256);

			/* Same as step 5 but buffer to small
			DEBUG_CMD(std::cout << "OnMove100::init() - step 5" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x200, 0x0, dataIn, 9);
			*/

			DEBUG_CMD(std::cout << "OnMove100::init() - step 6" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x200, 0x0, dataIn, 32);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 7" << std::endl);
			_dataSource->control_transfer(0x0, 0x9, 0x1, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 8" << std::endl);
			_dataSource->control_transfer(0x40, 0x0, 0xFFFF, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 9" << std::endl);
			_dataSource->control_transfer(0x40, 0x1, 0x2000, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 10" << std::endl);
			_dataSource->control_transfer(0xc0, 0xFF, 0x370b, 0x0, dataIn, 1);

			/* Same as step 10 - useless ?
			DEBUG_CMD(std::cout << "OnMove100::init() - step 11" << std::endl);
			_dataSource->control_transfer(0xc0, 0xFF, 0x370b, 0x0, dataIn, 1);
			*/

			DEBUG_CMD(std::cout << "OnMove100::init() - step 12" << std::endl);
			data[0] = 0; data[1] = 0xc2; data[2] = 1; data[3] = 0;
			_dataSource->control_transfer(0x40, 0x1e, 0x0, 0x0, data, 4);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 13" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);

			/* Same as step 13 - useless ?
			DEBUG_CMD(std::cout << "OnMove100::init() - step 14" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 15" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 16" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);

			DEBUG_CMD(std::cout << "OnMove100::init() - step 17" << std::endl);
			_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);
			*/
		}
		// Step 2 - Device initialization ?
		{
			const size_t sizeInitResponse = 2;
			//unsigned char initResponse[sizeInitResponse] = { 0x45, 0x0c, 0x0c, 0x18, 0x01, 0x65 };
			// In issue 23, a dump attached as 45 0c 04 18 00 as answer, considered correct by OnConnect software
			// So let's only check 45 0c as it's the common part to all dumps until now
			unsigned char initResponse[sizeInitResponse] = { 0x45, 0x0c };
			size_t received = 0;
			unsigned char *responseData;
			int attempts = 0;
			do
			{
				try
				{
					attempts++;
					DEBUG_CMD(std::cout << "OnMove100::init() - device init attempt " << attempts << std::endl);
					_dataSource->write_data(0x01, deviceInit, lengthDeviceInit);
					_dataSource->read_data(0x81, &responseData, &received);
				}
				catch(...)
				{
					unsigned char data[256] = { 0 };
					DEBUG_CMD(std::cout << "OnMove100::init() - step 13 (retry device init)" << std::endl);
					_dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);
				}
			} while(memcmp(responseData, initResponse, sizeInitResponse) && attempts < 10);
		}
	}

	void OnMove100::release()
	{
	}

	double OnMove100::decodeCoordinate(unsigned char *buffer)
	{
		uint64_t deg = 0;
		deg += 10*((buffer[0] & 0xF0) >> 4) + (buffer[0] & 0xF);
		deg *= 100;
		deg += 10*((buffer[1] & 0xF0) >> 4) + (buffer[1] & 0xF);
		uint64_t min = 0;
		for(int i = 0; i < 3; ++i)
		{
			min *= 100;
			min += 10*((buffer[i+2] & 0xF0) >> 4) + (buffer[i+2] & 0xF);
		}
		double pos_neg = 1.0;
		// W (west) and S (south) correspond to negative coordinates
		if(buffer[5] == 'W' || buffer[5] == 'S')
		{
			pos_neg = -1.0;
		}
		return pos_neg * (deg + (min/600000.0));
	}

#define READ_MORE_DATA if(received > 36) { responseData += 36; received -= 36; } else { _dataSource->read_data(0x81, &responseData, &received); }

	void OnMove100::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "OnMove100: Get sessions list !" << std::endl);
		unsigned char *responseData;
		size_t received;
		_dataSource->write_data(0x01, getData, lengthGetData);
		_dataSource->read_data(0x81, &responseData, &received);
		if(responseData[35] != 0xff)
		{
			std::cout << "Unexpected line termination " << responseData[35] << " instead of 0xFF." << std::endl;
		}
		READ_MORE_DATA;
		// First lines to be reverse engineered (sessions global infos ?)
		while(responseData[35] == 0xfd)
		{
			try
			{
				tm time;
				memset(&time, 0, sizeof(time));
				time.tm_sec = responseData[0];
				time.tm_min = responseData[1];
				time.tm_hour = responseData[2];
				time.tm_mday = responseData[3];
				// In tm, month is between 0 and 11.
				time.tm_mon = responseData[4] - 1;
				// In tm, year is year since 1900. GPS returns year since 2000
				time.tm_year = 100 + responseData[5];
				time.tm_isdst = -1;
				DEBUG_CMD(std::cout << "Session from: " << time.tm_year + 1900 << "-" << time.tm_mon + 1 << "-" << time.tm_mday << " " << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << std::endl);
				int nb_points = (responseData[14] << 8) + responseData[15];
				int num_session = responseData[34];
				SessionId id = SessionId(responseData[34], responseData[34]+1);
				// TODO: Find duration, distance and # laps (watch doesn't support laps ?)
				Session mySession(id, num_session, time, nb_points, 0, 0, 0);
				oSessions->insert(SessionsMapElement(id, mySession));
				//Session *session = &(oSessions->find(id)->second);
				// Ignore second line (for now ?)
				READ_MORE_DATA;
			}
			catch(std::runtime_error e)
			{
				DEBUG_CMD(std::cout << "OnMove100: read_data failed: " << e.what() << " - Retrying a get data." << std::endl;)
				_dataSource->write_data(0x01, getData, lengthGetData);
			}
			READ_MORE_DATA;
		}
		while(responseData[35] == 0xfa)
		{
			DEBUG_CMD(std::cout << "OnMove100: Read session !" << std::endl);
			// First line ending with fa is the header of the session
			// It's exactly the same as the first line ending with fd
			/*
			tm time;
			time.tm_sec = responseData[0];
			time.tm_min = responseData[1];
			time.tm_hour = responseData[2];
			time.tm_mday = responseData[3];
			// In tm, month is between 0 and 11.
			time.tm_mon = responseData[4] - 1;
			// In tm, year is year since 1900. GPS returns year since 2000
			time.tm_year = 100 + responseData[5];
			time.tm_isdst = -1;
			DEBUG_CMD(std::cout << "Session from: " << time.tm_year + 1900 << "-" << time.tm_mon + 1 << "-" << time.tm_mday << " " << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << std::endl);
			int nb_points = (responseData[14] << 8) + responseData[15];
			int num_session = responseData[34];
			SessionId id = SessionId(responseData[34], responseData[34]+1);
			// TODO: Find duration, distance and # laps (watch doesn't support laps ?)
			Session mySession(id, num_session, time, nb_points, 0, 0, 0);
			oSessions->insert(SessionsMapElement(id, mySession));
			Session *session = &(oSessions->find(id)->second);
			time_t current_time = session->getTime();
			*/
			SessionId id = SessionId(responseData[34], responseData[34]+1);
			Session *session = &(oSessions->find(id)->second);
			time_t current_time = session->getTime();
			READ_MORE_DATA;
			// Can a session have more than 65536 points ? (bytes 32 & 33 contains the point number)
			while(responseData[32] != 0 || responseData[33] != 0)
			{
				//dump(responseData, 36);
				double distance = (responseData[0] + (responseData[1] << 8) + (responseData[2] << 16) + (responseData[3] << 24)) / 100.0;
				/*
				int seconds = responseData[6];
				int minutes = responseData[7];
				int hours = responseData[8];
				*/
				double latitude = decodeCoordinate(&responseData[14]);
				double longitude = decodeCoordinate(&responseData[20]);
				double elapsed = responseData[28]*3600 + responseData[29]*60 + responseData[30] + responseData[31]/100.0;
				int hundredth = responseData[31];
				//int pointID = (responseData[32] << 8) + responseData[33];
				//int sessionNum = responseData[34];
				double speed = (distance * 3.6) / elapsed;
				if(elapsed == 0) speed = 0;
				// TODO: Find out altitude, speed
				Point *myPoint = new Point(latitude, longitude, FieldUndef, speed, current_time+elapsed, hundredth, FieldUndef, 3);
				session->addPoint(myPoint);
				READ_MORE_DATA;
			}
		}
		DEBUG_CMD(std::cout << "OnMove100: Finished !" << std::endl);
	}

#undef READ_MORE_DATA

	void OnMove100::getSessionsDetails(SessionsMap *oSessions) 
	{
	}

	void OnMove100::exportSession(Session *iSession) 
	{
		std::cerr << "Unsupported export of session with OnMove 100" << std::endl;
	}
}
