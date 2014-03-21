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
			_dataSource->control_transfer(0x80, 0x6, 0x0, 0x1, data, 18);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 3" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, dataIn, 18);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 4" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 5" << std::endl);
			_dataSource->control_transfer(0x80, 0x6, 0x0, 0x3, data, 256);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 6" << std::endl);
			data[0] = 4; data[1] = 3; data[2] = 9; data[3] = 4;
			_dataSource->control_transfer(0x0, 0x2, 0x4, 0x1, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 7" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 8" << std::endl);
			data[0] = 0x80; data[1] = 6; data[2] = 2; data[3] = 3; data[4] = 9; data[5] = 4; data[6] = 0; data[7] = 1;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 256);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 9" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x4a, 0x1, dataIn, 74);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 10" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 11" << std::endl);
			data[0] = 0x80; data[1] = 6; data[2] = 0; data[3] = 2; data[4] = 0; data[5] = 0; data[6] = 9; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 9);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 12" << std::endl);
			data[0] = 0x9; data[1] = 2; data[2] = 0x20; data[3] = 0; data[4] = 1; data[5] = 1; data[6] = 0; data[7] = 0x80; data[8] = 0x32;
			_dataSource->control_transfer(0x80, 0x2, 0x9, 0x1, data, 9);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 13" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 14" << std::endl);
			data[0] = 0x80; data[1] = 6; data[2] = 0; data[3] = 2; data[4] = 0; data[5] = 0; data[6] = 0x20; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 15" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x20, 0x1, dataIn, 0x20);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 16" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 17" << std::endl);
			data[0] = 0x0; data[1] = 9; data[2] = 1; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x80, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 18" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 19" << std::endl);
			data[0] = 0x40; data[1] = 0; data[2] = 0xFF; data[3] = 0xFF; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 20" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 21" << std::endl);
			data[0] = 0x40; data[1] = 1; data[2] = 0; data[3] = 0x20; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 22" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 23" << std::endl);
			data[0] = 0xc0; data[1] = 0xff; data[2] = 0xb; data[3] = 0x37; data[4] = 0; data[5] = 0; data[6] = 1; data[7] = 0;
			_dataSource->control_transfer(0x80, 0x2, 0x8, 0x0, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 24" << std::endl);
			data[0] = 2;
			_dataSource->control_transfer(0x80, 0x2, 0x1, 0x1, data, 1);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 25" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 26" << std::endl);
			data[0] = 0xc0; data[1] = 0xff; data[2] = 0xb; data[3] = 0x37; data[4] = 0; data[5] = 0; data[6] = 1; data[7] = 0;
			_dataSource->control_transfer(0x80, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 27" << std::endl);
			data[0] = 2;
			_dataSource->control_transfer(0x80, 0x2, 0x1, 0x1, data, 1);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 28" << std::endl);
			_dataSource->control_transfer(0x80, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 29" << std::endl);
			data[0] = 0x40; data[1] = 0x1e; data[2] = 0; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 4; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 30" << std::endl);
			data[0] = 0x0; data[1] = 0xc2; data[2] = 1; data[3] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x4, 0x1, data, 4);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 31" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 32" << std::endl);
			data[0] = 0x40; data[1] = 0x12; data[2] = 0xc; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 33" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 34" << std::endl);
			data[0] = 0x40; data[1] = 0x12; data[2] = 0xc; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 35" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 36" << std::endl);
			data[0] = 0x40; data[1] = 0x12; data[2] = 0xc; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 37" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 38" << std::endl);
			data[0] = 0x40; data[1] = 0x12; data[2] = 0xc; data[3] = 0; data[4] = 0; data[5] = 0; data[6] = 0; data[7] = 0;
			_dataSource->control_transfer(0x0, 0x2, 0x8, 0x0, data, 8);
			DEBUG_CMD(std::cout << "OnMove100::init() - step 39" << std::endl);
			_dataSource->control_transfer(0x0, 0x2, 0x0, 0x2, data, 0);
		}
		// Step 2 - Device initialization ?
		{
			size_t sizeInitResponse = 6;
			unsigned char initResponse[sizeInitResponse] = { 0x45, 0x0c, 0x0c, 0x18, 0x01, 0x65 };
			size_t received = 0;
			unsigned char *responseData;
			int attempts = 0;
			while(strncmp(responseData, initResponse, sizeInitResponse) && attempts < 10)
			{
				attempts++;
				DEBUG_CMD(std::cout << "OnMove100::init() - device init attempt " << attempts << std::endl);
				_dataSource->write_data(0x01, deviceInit, lengthDeviceInit);
				_dataSource->read_data(0x81, &responseData, &received);
			}
			_dataSource->write_data(0x01, getData, lengthGetData);
			// Read first line (byte 34 == 0)
			while(responseData[35] != 0xfa)
			{
				_dataSource->read_data(0x81, &responseData, &received);
			}
			// Last line read is the header
			_dataSource->read_data(0x81, &responseData, &received);
			// Stop condition ?
		}
	}

	void OnMove100::release()
	{
	}

	void OnMove100::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "OnMove100: Get sessions list !" << std::endl);
		unsigned char *responseData;
		size_t received;
	}

	void OnMove100::getSessionsDetails(SessionsMap *oSessions) 
	{
	}

	void OnMove100::exportSession(Session *iSession) 
	{
		std::cerr << "Unsupported export of session with OnMove 100" << std::endl;
	}
}
