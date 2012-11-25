#include "Kalenji.h"
#include "../Common.h"
#include <cstring>
#include <iomanip>

namespace device
{
	REGISTER_DEVICE(Kalenji);

	const int     Kalenji::lengthDataDevice = 5;
	unsigned char Kalenji::dataDevice[lengthDataDevice] = { 0x02, 0x00, 0x01, 0x85, 0x84 };
	const int     Kalenji::lengthDataList = 5;
	unsigned char Kalenji::dataList[lengthDataList] = { 0x02, 0x00, 0x01, 0x78, 0x79 };
	const int     Kalenji::lengthDataMore = 5;
	unsigned char Kalenji::dataMore[lengthDataMore] = { 0x02, 0x00, 0x01, 0x81, 0x80 };

	void Kalenji::init()
	{
		_dataSource->init(0x0483, 0x5740);
		unsigned char *responseData;
		size_t transferred;
		_dataSource->write_data(dataDevice, lengthDataDevice);
		_dataSource->read_data(&responseData, &transferred);
	}

	void Kalenji::getSessionsList(SessionsMap *oSessions)
	{
		unsigned char *responseData;
		size_t received;
		_dataSource->write_data(dataList, lengthDataList);
		_dataSource->read_data(&responseData, &received);

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
		int nbRecords = size / 24;
		if(nbRecords * 24 != size)
		{
			std::cerr << "Size is not a multiple of 24 in getList !" << std::endl;
			// TODO: Throw an exception
		}
		for(int i = 0; i < nbRecords; ++i)
		{
			// Decoding of basic info about the session
			unsigned char *line = &responseData[24*i+3];
			SessionId id = SessionId(line, line+16);
			uint32_t num = (line[19] << 8) + line[18];
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

			uint32_t nb_points = line[6] + (line[7] << 8);
			double duration = (line[8] + (line[9] << 8) + (line[10] << 16) + (line[11] << 24)) / 10.0;
			uint32_t distance = line[12] + (line[13] << 8) + (line[14] << 16) + (line[15] << 24);

			// nb_laps has no interest as we read laps later except to display it in the list of sessions before import
			uint32_t nb_laps = line[16] + (line[17] << 8);

			Session mySession(id, num, time, nb_points, duration, distance, nb_laps);
			oSessions->insert(SessionsMapElement(id, mySession));
		}
	}

	void Kalenji::getSessionsDetails(SessionsMap *oSessions)
	{
		// Sending the query with the list of sessions to retrieve
		{
			int length = 7 + 2*oSessions->size();
			unsigned char data[length];
			data[0] = 0x02;
			data[1] = 0x00;
			data[2] = length - 4;
			data[3] = 0x80;
			data[4] = oSessions->size() & 0xFF;
			data[5] = (oSessions->size() & 0xFF00) / 256;
			int i = 6;
			for(SessionsMap::iterator it = oSessions->begin(); it != oSessions->end(); ++it)
			{
				data[i++] = it->second.getNum() & 0xFF;
				data[i++] = (it->second.getNum() & 0xFF00) / 256;
			}
			unsigned char checksum = 0;
			for(int i = 2; i < length-1; ++i)
			{
				checksum ^= data[i];
			}
			data[length-1] = checksum;

			_dataSource->write_data(data, length);
		}

		// Parsing the result
		// TODO: Extract some stuff in some functions so that it's easier to read and to factorize code 
		// TODO: Use only one session pointer, one session ID and one find. Only check it's the same at all step to be sure
		size_t received = 0;
		unsigned char *responseData;
		do
		{
			// First response 80 retrieves info concerning the session
			{
				// TODO: Use more info from this first call (some data global to the session: calories, grams, ascent, descent ...)
				_dataSource->read_data(&responseData, &received);
				if(responseData[0] == 0x8A) break;
				if(responseData[0] != 0x80)
				{
					std::cerr << "Unexpected header for getSessionsDetails (step 2): " << (int)responseData[0] << std::endl;
					// TODO: throw an exception
				}
				int size = responseData[2] + (responseData[1] << 8);
				// TODO: Is checking the size everywhere really usefull ? Checks could be factorized !
				if(size + 4 != received)
				{
					std::cerr << "Unexpected size in header for getSessionsDetails (step 1): " << size << " != " << received << " - 4" << std::endl;
					// TODO: throw an exception
				}
				SessionId id(responseData + 3, responseData + 19);
				Session *session = &(oSessions->find(id)->second);
				double max_speed = (responseData[55] + (responseData[56] << 8)) / 100.0;
				double avg_speed = (responseData[57] + (responseData[58] << 8)) / 100.0;
				session->setMaxSpeed(max_speed);
				session->setAvgSpeed(avg_speed);
				uint32_t ascent = responseData[70] + (responseData[71] << 8);
				uint32_t descent = responseData[72] + (responseData[73] << 8);
				session->setAscent(ascent);
				session->setDescent(descent);
			}

			// Second response 80 retrieves info concerning the laps of the session. 
			_dataSource->write_data(dataMore, lengthDataMore);
			_dataSource->read_data(&responseData, &received);
			do
			{
				if(responseData[0] == 0x8A) break;
				if(responseData[0] != 0x80)
				{
					std::cerr << "Unexpected header for getSessionsDetails (step 2): " << (int)responseData[0] << std::endl;
					// TODO: throw an exception
				}

				int size = responseData[2] + (responseData[1] << 8);
				if(size + 4 != received)
				{
					std::cerr << "Unexpected size in header for getSessionsDetails (step 2): " << size << " != " << received << " - 4" << std::endl;
					// TODO: throw an exception
				}
				SessionId id(responseData + 3, responseData + 19);
				Session *session = &(oSessions->find(id)->second);
				int nbRecords = (size - 24)/ 44;
				if(nbRecords * 44 != size - 24)
				{
					std::cerr << "Size is not a multiple of 44 plus 24 in getSessionsDetails (step 2): " << nbRecords << "*44 != " << size << "-24" << "!" << std::endl;
					// TODO: throw an exception
				}
				// -8<--- DIRTY: Ugly bug in the firmware, some laps have ff ff where there should be points ids
				uint32_t prevLastPoint = 0;
				uint32_t firstPointOfRow = responseData[40+27] + (responseData[41+27] << 8);
				uint32_t lastPointOfRow = firstPointOfRow + nbRecords;
				// ->8---
				for(int i = 0; i < nbRecords; ++i)
				{ // Decoding and addition of the lap
					unsigned char *line = &responseData[44*i + 27];
					static uint32_t sum_calories = 0;
					// time_t lap_end = lap_start + (line[0] + (line[1] << 8) + (line[2] << 16) + (line[3] << 24)) / 10;
					double duration = (line[4] + (line[5] << 8) + (line[6] << 16) + (line[7] << 24)) / 10.0;
					// last_lap_end = lap_end;

					uint32_t length = line[8] + (line[9] << 8) + (line[10] << 16) + (line[11] << 24);
					double max_speed = (line[12] + (line[13] << 8)) / 100.0;
					double avg_speed = (line[14] + (line[15] << 8)) / 100.0;
					uint32_t max_hr = line[20];
					uint32_t avg_hr = line[21];
					uint32_t calories = line[26] + (line[27] << 8);
					// Calories for lap given by watch is the sum of alll past laps (this looks like a bug ?! this may change with later firmwares !)
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
					uint32_t firstPoint = line[40] + (line[41] << 8);
					uint32_t lastPoint = line[42] + (line[43] << 8);
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
					Lap *lap = new Lap(firstPoint, lastPoint, duration, length, max_speed, avg_speed, max_hr, avg_hr, calories, grams, descent, ascent);
					session->addLap(lap);
				}
				_dataSource->write_data(dataMore, lengthDataMore);
				_dataSource->read_data(&responseData, &received);
			} while(responseData[25] == 0xaa);

			// Third response 80 retrieves info concerning the points of the session. There can be many.
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
				SessionId id(responseData + 3, responseData + 19);
				session = &(oSessions->find(id)->second);
				std::list<Point*> points = session->getPoints();
				time_t current_time = session->getTime();
				if(!points.empty())
				{
					current_time = points.back()->getTime();
				}
				int nbRecords = (size - 24)/ 20;
				if(nbRecords * 20 != size - 24)
				{
					std::cerr << "Size is not a multiple of 20 plus 24 in getSessionsDetails (step 3) !" << std::endl;
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
						unsigned char *line = &responseData[20*i + 27];
						double lat = (line[0] + (line[1] << 8) + (line[2] << 16) + (line[3] << 24)) / 1000000.0;
						double lon = (line[4] + (line[5] << 8) + (line[6] << 16) + (line[7] << 24)) / 1000000.0;
						// Altitude can be signed (yes, I already saw negative ones with the watch !) and is on 16 bits
						int16_t alt = line[8] + (line[9] << 8);
						double speed = ((double)(line[10] + (line[11] << 8)) / 100.0);
						uint16_t bpm = line[12];
						uint16_t fiability = line[13];
						cumulated_tenth += line[16] + (line[17] << 8);
						current_time += cumulated_tenth / 10;
						cumulated_tenth = cumulated_tenth % 10;
						Point *point = new Point(lat, lon, alt, speed, current_time, cumulated_tenth, bpm, fiability);
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
				if(keep_going)
				{
					_dataSource->write_data(dataMore, lengthDataMore);
					_dataSource->read_data(&responseData, &received);
				}
			}
			std::cout << "Retrieved session from " << session->getBeginTime() << std::endl;
			if(responseData[0] == 0x8A) break;

			_dataSource->write_data(dataMore, lengthDataMore);
		}
		// Redundant with all the if / break above !
		while(responseData[0] != 0x8A);
	}

	void Kalenji::exportSession(Session *iSession)
	{
		unsigned int nbPoints = iSession->getPoints().size();
		if(nbPoints > KALENJI_EXPORT_MAX_POINTS)
		{
			std::cerr << "Export with Kalenji watches doesn't support more than " << KALENJI_EXPORT_MAX_POINTS << " points. This profile contains " << nbPoints << " points." << std::endl;
			std::cerr << "I'm not (yet?) able to reduce it !" << std::endl;
			return;
		}
		unsigned int payloadSize = 33+8*nbPoints;
		unsigned int bufferSize = payloadSize + 4;
		unsigned char *buffer = new unsigned char[bufferSize];
		buffer[0] = 0x02;
		buffer[1] = payloadSize / 256;
		buffer[2] = payloadSize % 256;
		buffer[3] = 0x93;
		strncpy((char*)&buffer[4], iSession->getName().c_str(), 15);
		buffer[19] = 0x0;

		unsigned int distance = iSession->getDistance();
		buffer[20] = distance % 256;
		buffer[21] = (distance / 256) % 256;
		buffer[22] = (distance / (256*256)) % 256;
		buffer[23] = (distance / (256*256*256)) % 256;

		// TODO: doesn't seem to work (or just not taken into account by the watch ?)
		uint32_t ascent = iSession->getAscent();
		buffer[24] = ascent % 256;
		buffer[25] = (ascent / 256) % 256;
		uint32_t descent = iSession->getDescent();
		buffer[26] = descent % 256;
		buffer[27] = (descent / 256) % 256;

		unsigned int duration = 10*iSession->getDuration();
		buffer[28] = duration % 256;
		buffer[29] = (duration / 256) % 256;
		buffer[30] = (duration / (256*256)) % 256;
		buffer[31] = (duration / (256*256*256)) % 256;

		buffer[32] = nbPoints % 256;
		buffer[33] = (nbPoints / 256) % 256;

		// TODO: Understand what is in bytes 30 and 31
		buffer[34] = 0x0;
		buffer[35] = 0x0;

		unsigned int i = 0;
		for(std::list<Point*>::iterator it = iSession->getPoints().begin(); it != iSession->getPoints().end(); ++it)
		{
			unsigned int lat = (*it)->getLatitude() * 1000000;
			buffer[36+i++] = lat % 256;
			buffer[36+i++] = (lat / 256) % 256;
			buffer[36+i++] = (lat / (256*256)) % 256;
			buffer[36+i++] = (lat / (256*256*256)) % 256;
			unsigned int lon = (*it)->getLongitude() * 1000000;
			buffer[36+i++] = lon % 256;
			buffer[36+i++] = (lon / 256) % 256;
			buffer[36+i++] = (lon / (256*256)) % 256;
			buffer[36+i++] = (lon / (256*256*256)) % 256;
		}

		unsigned char checksum = 0;
		for(unsigned int j = 1; j < bufferSize - 1; ++j)
		{
			checksum ^= buffer[j];
		}
		buffer[bufferSize-1] = checksum;

		_dataSource->write_data(buffer, bufferSize);
		unsigned char *responseData;
		size_t transferred;
		_dataSource->read_data(&responseData, &transferred);
		if(transferred == 4 && responseData[0] == 0x95 && responseData[1] == 0 && responseData[2] == 0 && responseData[3] == 0)
		{
			std::cout << "No space left on your watch. Please do some cleaning and retry." << std::endl;
		}
		else if(transferred != 4 || responseData[0] != 0x93 || responseData[1] != 0 || responseData[2] != 0 || responseData[3] != 0)
		{
			std::cout << "Invalid response: " << std::hex;
			for(int i = 0; i < transferred; ++i)
			{
				std::cout << std::setw(2) << std::setfill('0') << (int)responseData[i] << " ";
			}
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Transferred session " << iSession->getName() << std::endl;
		}
	}
}
