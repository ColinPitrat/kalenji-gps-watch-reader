#include "Keymaze.h"
#include "../Common.h"
#include <cstring>
#include <iomanip>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

#define RESPONSE_BUFFER_SIZE 4096

namespace device
{
	REGISTER_DEVICE(Keymaze);

	const int     Keymaze::lengthDataDevice = 5;
	unsigned char Keymaze::dataDevice[lengthDataDevice] = { 0x02, 0x00, 0x01, 0x85, 0x84 };
	const int     Keymaze::lengthDataList = 5;
	unsigned char Keymaze::dataList[lengthDataList] = { 0x02, 0x00, 0x01, 0x78, 0x79 };
	const int     Keymaze::lengthDataMore = 5;
	unsigned char Keymaze::dataMore[lengthDataMore] = { 0x02, 0x00, 0x01, 0x81, 0x80 };
	unsigned char Keymaze::message[RESPONSE_BUFFER_SIZE];

	void Keymaze::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::endl);
	}

	void Keymaze::readMessage(unsigned char **buffer, size_t *index)
	{
		DEBUG_CMD(std::cout << "Keymaze::readMessage()" << std::endl);
		unsigned char* responseData;
		size_t prev_index = *index = 0;
		size_t transferred;
		size_t full_size = 0;
		// full_size + 4 because there are 4 additional bytes to the payload (1B for header, 2B for size and 2B for checksum)
		do
		{
			_dataSource->read_data(0x83, &responseData, &transferred);
			memcpy(&(message[*index]), responseData, transferred);
			*index += transferred;
			if(full_size == 0 && (*index) >= 3)
				full_size = (message[1] << 8) + message[2];
		}
		while((*index) < (full_size + 4) || (*index) < 3);
		DEBUG_CMD(std::cout << "Read a total size of " << *index << " - expected: " << full_size + 4 << std::endl;);
		*buffer = message;
	}

	void Keymaze::init()
	{
		DEBUG_CMD(std::cout << "Keymaze::init() - init device" << std::endl);
		_dataSource->init(getVendorId(), getProductId());
		unsigned char *responseData;
		size_t transferred;
		// Step 1: Some control transfer, necessary to initialize the device ?
		{
			unsigned char data[256] = { 0, 0xE1, 0, 0, 0, 0, 0x8 };
			unsigned char dataIn[256];
			// TODO: Define which messages to send by looking at the ULZ
			// Lots of vendor device
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 1 (in 0xC0)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 2" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0404, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 3 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 4 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8383, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 5 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 6" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0404, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 7 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 8 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8383, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 9 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 10" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0404, 0x60, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 11 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 12 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8383, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 13 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 14" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0404, 0x61, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 15 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8484, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 16 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x8383, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 17 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0081, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 18" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 19" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0001, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 20" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0002, 0x0044, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - wakeup device" << std::endl);
			// Set feature - remote wakeup
			_dataSource->control_transfer(0x00, 0x03, 0x0001, 0x0, data, 0x0);
			// Class interface
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 1" << std::endl);
			_dataSource->control_transfer(0x21, 0x20, 0x0000, 0x0, data, 0x7);
			// Lots of vendor device again
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 21 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0080, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 22" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 23 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0081, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 24" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0001, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 25 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0080, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 26" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			// Class interface
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 2" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 3" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			// Vendor device
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 29 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0080, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 30" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 31 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0080, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 32" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 33 (in)" << std::endl);
			_dataSource->control_transfer(0xC0, 0x01, 0x0080, 0x0, dataIn, 0x1);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 34" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0000, 0x1, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 35" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0B0B, 0x2, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 36" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0909, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - vendor device 37" << std::endl);
			_dataSource->control_transfer(0x40, 0x01, 0x0808, 0x0, data, 0x0);
			// Class interface
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 2" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 3" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			// Vendor device again
			_dataSource->control_transfer(0x40, 0x01, 0x0505, 0x1311, data, 0x0);
			// Class interface
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 4" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 5" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
			// Vendor device again
			_dataSource->control_transfer(0x40, 0x01, 0x0505, 0x1311, data, 0x0);
			// Class interface
			DEBUG_CMD(std::cout << "Keymaze::init() - class interface 6" << std::endl);
			_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
		}
		DEBUG_CMD(std::cout << "Keymaze::init() - send hello message" << std::endl);
		_dataSource->write_data(0x02, dataDevice, lengthDataDevice);
		readMessage(&responseData, &transferred);
		if(responseData[0] != 0x85)
		{
			std::cerr << "Unexpected header for initDevice: " << std::hex << (int) responseData[0] << std::dec << std::endl;
			// TODO: Throw an exception
		}
	}

	void Keymaze::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "Keymaze::getSessionsList()" << std::endl);
		unsigned char *responseData;
		size_t received;
		_dataSource->write_data(0x02, dataList, lengthDataList);
		readMessage(&responseData, &received);

		if(responseData[0] != 0x78)
		{
			std::cerr << "Unexpected header for getList: " << std::hex << (int) responseData[0] << std::dec << std::endl;
			// TODO: Throw an exception
		}
		int size = responseData[2] + (responseData[1] << 8);
		if(size + 4 != received)
		{
			std::cerr << "Unexpected size in header for getList: " << responseData[2] << " (!= " << received << " - 4)" << std::endl;
			// TODO: Throw an exception
		}
		// 31 bytes per session
		int nbRecords = size / 31;
		if(nbRecords * 31 != size)
		{
			std::cerr << "Size is not a multiple of 31 in getList !" << std::endl;
			// TODO: Throw an exception
		}
		for(int i = 0; i < nbRecords; ++i)
		{
			// Decoding of basic info about the session
			unsigned char *line = &responseData[31*i+3];
			SessionId id = SessionId(line, line+26);
			uint32_t num = (line[27] << 8) + line[28];
			tm time;
			// In tm, year is year since 1900. GPS returns year since 2000
			time.tm_year = 100 + line[0];
			// In tm, month is between 0 and 11.
			time.tm_mon = line[1] - 1;
			time.tm_mday = line[2];
			time.tm_hour = line[3];
			time.tm_min = line[4];
			time.tm_sec = line[5];
			time.tm_isdst = -1;

			uint32_t nb_laps = line[6];
			double duration = ((line[7] << 24) + (line[8] << 16) + (line[9] << 8) + line[10]) / 10.0;
			uint32_t distance = (line[11] << 24) + (line[12] << 16) + (line[13] << 8) + line[14];
			/* Not used
			uint32_t calories = (line[15] << 8) + line[16];
			uint32_t max_bpm = line[19];
			uint32_t avg_bpm = line[20];
			*/
			uint32_t nb_points = (line[25] << 8) + line[26];

			Session mySession(id, num, time, nb_points, duration, distance, nb_laps);
			oSessions->insert(SessionsMapElement(id, mySession));
		}
	}

	void Keymaze::getSessionsDetails(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "Keymaze::getSessionsDetails()" << std::endl);
		// Sending the query with the list of sessions to retrieve
		{
			int length = 7 + 2*oSessions->size();
			unsigned char data[length];
			data[0] = 0x02;
			data[1] = (length & 0xFF00) >> 8;
			data[2] = (length - 4) & 0xFF;
			data[3] = 0x80;
			data[4] = oSessions->size() & 0xFF;
			data[5] = (oSessions->size() & 0xFF00) >> 8;
			int i = 6;
			for(SessionsMap::iterator it = oSessions->begin(); it != oSessions->end(); ++it)
			{
				data[i++] = (it->second.getNum() & 0xFF00) >> 8;
				data[i++] = it->second.getNum() & 0xFF;
			}
			unsigned char checksum = 0;
			for(int i = 2; i < length-1; ++i)
			{
				checksum ^= data[i];
			}
			data[length-1] = checksum;

			_dataSource->write_data(0x02, data, length);
		}

		// Parsing the result
		// TODO: Extract some stuff in some functions so that it's easier to read and to factorize code 
		// TODO: Use only one session pointer, one session ID and one find. Only check it's the same at all step to be sure
		size_t received = 0;
		unsigned char *responseData;
		readMessage(&responseData, &received);
		do
		{
			// First response 80 retrieves info concerning the laps of the session. 
			// TODO: support multi laps sessions
			do
			{
				if(responseData[0] == 0x8A) break;
				if(responseData[0] != 0x80)
				{
					std::cerr << "Unexpected header for getSessionsDetails (step 1): " << (int)responseData[0] << std::endl;
					// TODO: throw an exception
				}

				int size = responseData[2] + (responseData[1] << 8);
				if(size + 4 != received)
				{
					std::cerr << "Unexpected size in header for getSessionsDetails (step 1): " << size << " != " << received << " - 4" << std::endl;
					// TODO: throw an exception
				}
				SessionId id(responseData + 3, responseData + 29);
				Session *session = &(oSessions->find(id)->second);
				// TODO: Check in a multilap session if the 27 first byte are really not repeated (session info, not lap info)
				// and if the lap info is 26 bytes long
				int nbRecords = (size - 27) / 26;
				if(nbRecords * 26 != size - 27)
				{
					std::cerr << "Size is not a multiple of 26 plus 27 in getSessionsDetails (step 1): " << nbRecords << "*26 != " << size << "-27" << "!" << std::endl;
					// TODO: throw an exception
				}
				// -8<--- DIRTY: Ugly bug in the firmware, some laps have ff ff where there should be points ids
				uint32_t prevLastPoint = 0;
				uint32_t firstPointOfRow = responseData[53] + (responseData[52] << 8);
				// TODO: Check if lastPointOfRow = responseData[55] + (responseData[54] << 8)
				uint32_t lastPointOfRow = firstPointOfRow + nbRecords;
				// ->8---
				for(int i = 0; i < nbRecords; ++i)
				{
					// Decoding and addition of the lap
					unsigned char *line = &responseData[26*i + 27 + 3];
					double duration = ((line[4] << 24) + (line[5] << 16) + (line[6] << 8) + line[7]) / 10.0;
					uint32_t length = (line[12] << 24) + (line[13] << 16) + (line[14] << 8) + line[15];
					uint32_t calories = (line[16] << 8) + line[17];
					uint32_t max_hr = line[20];
					uint32_t avg_hr = line[21];
					uint32_t firstPoint = (line[22] << 8)  + line[23];
					uint32_t lastPoint = (line[24] << 8) + line[25];
					Lap *lap = new Lap(firstPoint, lastPoint, duration, length, FieldUndef, FieldUndef, max_hr, avg_hr, calories, FieldUndef, FieldUndef, FieldUndef);
					session->addLap(lap);
					/*
					TODO: Check and fix all this when I have a multilap session example
					// time_t lap_end = lap_start + (line[0] + (line[1] << 8) + (line[2] << 16) + (line[3] << 24)) / 10;
					// last_lap_end = lap_end;

					double max_speed = (line[12] + (line[13] << 8)) / 100.0;
					double avg_speed = (line[14] + (line[15] << 8)) / 100.0;
					// Calories for lap given by watch is the sum of all past laps (this looks like a bug ?! this may change with later firmwares !)
					std::list<Lap*> laps = session->getLaps();
					if(laps.empty())
					{
						sum_calories = 0;
					}
					else
					{
						if(calories < sum_calories)
						{
							std::cerr << "Error: Calories for this lap is smaller than previous one ! It means a firmware bug has been fixed !" << std::endl;
							std::cerr << "       Please notify project maintainer. If possible provide your firmware version." << std::endl;
							std::cerr << "       Info concerning calories will be wrong." << std::endl;
						}
						calories -= sum_calories;
					}
					sum_calories += calories;
					uint32_t grams = line[28] + (line[29] << 8);
					uint32_t descent = line[30] + (line[31] << 8);
					uint32_t ascent = line[32] + (line[33] << 8);
					// -8<--- DIRTY: Ugly bug in the firmware, some laps have ff ff where there should be points ids
					uint32_t nextFirstPoint = lastPointOfRow;
					if(i < nbRecords - 1)
					{
						nextFirstPoint = line[40+44] + (line[41+44] << 8);
					}
					if(firstPoint == 0xffff && lastPoint == 0xffff)
					{
						firstPoint = prevLastPoint;
						lastPoint = nextFirstPoint;
					}
					else
					{
						prevLastPoint = lastPoint;
					}
					// ->8---
					*/
				}
				_dataSource->write_data(0x02, dataMore, lengthDataMore);
				readMessage(&responseData, &received);
				// TODO: Find a good condition to end the loop
			} while(responseData[25] == 0xaa);

			// Following response 80 retrieves info concerning the points of the session. There can be many.
			Session *session;
			uint32_t id_point = 0;
			bool keep_going = true;
			uint32_t cumulated_tenth = 0;
			while(keep_going)
			{
				if(responseData[0] == 0x8A) break;

				if(responseData[0] != 0x80)
				{
					std::cerr << "Unexpected header for getSessionsDetails (step 3): " << (int)responseData[0] << std::endl;
					// TODO: throw an exception
				}
				int size = responseData[2] + (responseData[1] << 8);
				if(size + 4 != received)
				{
					std::cerr << "Unexpected size in header for getSessionsDetails (step 3): " << size << " != " << received << " - 4" << std::endl;
					// TODO: throw an exception
				}
				SessionId id(responseData + 3, responseData + 29);
				session = &(oSessions->find(id)->second);
				std::list<Point*> points = session->getPoints();
				time_t current_time = session->getTime();
				if(!points.empty())
				{
					current_time = points.back()->getTime();
				}
				int nbRecords = (size - 31)/ 15;
				if(nbRecords * 15 != size - 31)
				{
					std::cerr << "Size is not a multiple of 15 plus 31 in getSessionsDetails (step 2) ! " << size << " " << nbRecords << std::endl;
					// TODO: throw an exception
				}
				std::list<Lap*>::iterator lap = session->getLaps().begin();
				while(id_point > (*lap)->getLastPointId() && lap != session->getLaps().end())
				{
					++lap;
				}
				// TODO: Cleaner way to handle id_point ?
				for(int i = 0; i < nbRecords; ++i)
				{
					//std::cout << "We should have " << (*lap)->getFirstPointId() << " <= " << id_point << " <= " << (*lap)->getLastPointId() << std::endl;
					{ // Decoding and addition of the point
						unsigned char *line = &responseData[15*i + 31 + 3];
						DEBUG_CMD(std::cout << "Decoding point from: ";);
						DEBUG_CMD(dump(line, 15));
						double lat = ((line[0] << 24) + (line[1] << 16) + (line[2] << 8) + line[3]) / 1000000.0;
						double lon = ((line[4] << 24) + (line[5] << 16) + (line[6] << 8) + line[7]) / 1000000.0;
						int16_t alt = (line[8] << 8) + line[9];
						// TODO: Ensure this is speed !
						double speed = ((double)((line[10] << 8)+ line[11]) / 100.0);
						uint16_t bpm = line[12];
						cumulated_tenth += line[14];
						current_time += cumulated_tenth / 10;
						cumulated_tenth = cumulated_tenth % 10;
						Point *point = new Point(lat, lon, alt, speed, current_time, cumulated_tenth, bpm, 3);
						session->addPoint(point);
					}
					if(id_point == (*lap)->getFirstPointId())
					{
						(*lap)->setStartPoint(session->getPoints().back());
					}
					while(id_point >= (*lap)->getLastPointId() && lap != session->getLaps().end())
					{
						// This if is a safe net but should never be used (unless laps are not in order or first lap doesn't start at 0 or ...)
						if((*lap)->getStartPoint() == NULL)
						{
							std::cerr << "Error: lap has no start point and yet I want to go to the next lap ! (lap: " << (*lap)->getFirstPointId() << " - " << (*lap)->getLastPointId() << ")" << std::endl;
							(*lap)->setStartPoint(session->getPoints().back());
						}
						(*lap)->setEndPoint(session->getPoints().back());
						++lap;
						//std::cout << "Calling setStartPoint for " << id_point << "on lap (" << (*lap)->getFirstPointId() << " - " << (*lap)->getLastPointId() << ")" << std::endl;
						if(lap != session->getLaps().end())
						{
							(*lap)->setStartPoint(session->getPoints().back());
						}
					}
					id_point++;
				}
				keep_going = !session->isComplete();
				_dataSource->write_data(0x02, dataMore, lengthDataMore);
				readMessage(&responseData, &received);
			}
			std::cout << "Retrieved session from " << session->getBeginTime() << std::endl;
			if(responseData[0] == 0x8A) break;
		}
		// Redundant with all the if / break above !
		while(responseData[0] != 0x8A);
		DEBUG_CMD(std::cout << "Keymaze::getSessionsDetails() - class interface 1" << std::endl);
		_dataSource->control_transfer(0x21, 0x22, 0x0000, 0x0, data, 0x0);
	}
}
