#include "OnMove710.h"
#include <cstring>
#include <iomanip>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <dirent.h>
#include <sys/stat.h>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
	REGISTER_DEVICE(OnMove710);


	int bytesToInt2(unsigned char b0, unsigned char b1)
	{
		int Int = b0 | ( (int)b1 << 8 );
		return Int;
	}

	int bytesToInt4(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3)
	{
		int Int = b0 | ( (int)b1 << 8 ) | ( (int)b2 << 16 ) | ( (int)b3 << 24 );
		return Int;
	}

	unsigned char* readAllBytes(std::string filename, size_t& size)
	{
		std::ifstream fl(filename.c_str());
		fl.seekg( 0, std::ios::end );
		size_t len = fl.tellg();
		char* buffer = new char[len];
		fl.seekg(0, std::ios::beg);
		fl.read(buffer, len);
		fl.close();
		size = len;
		return (unsigned char*)buffer;
	}

	bool fileExists(std::string filename)
	{
		struct stat fileInfo;
		return stat(filename.c_str(), &fileInfo) == 0;
	}

	tm parseFilename(std::string filename)
	{
		std::string yearStr = filename.substr(0,1);
		std::string monthStr = filename.substr(1,1);
		std::string dayStr = filename.substr(2,1);
		std::string hoursStr = filename.substr(3,1);
		std::string minutesStr = filename.substr(4,2);
		std::string secondsStr = filename.substr(6,2);

		int year = (int)strtol(yearStr.c_str(),NULL,36);
		int month = (int)strtol(monthStr.c_str(),NULL,36);
		int day = (int)strtol(dayStr.c_str(),NULL,36);
		int hours = (int)strtol(hoursStr.c_str(),NULL,36);
		int minutes = atoi(minutesStr.c_str());
		int seconds = atoi(secondsStr.c_str());

		tm time;
		time.tm_year  = 100 + year;// In tm, year is year since 1900. GPS returns year since 2000
		time.tm_mon   = month - 1;// In tm, month is between 0 and 11.
		time.tm_mday  = day;
		time.tm_hour  = hours;
		time.tm_min   = minutes;
		time.tm_sec   = seconds;
		time.tm_isdst = -1;

		return time;
	}

	void OnMove710::dump(unsigned char *data, int length)
	{
		DEBUG_CMD(std::cout << std::hex);
		for(int i = 0; i < length; ++i)
		{
			DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
		}
		DEBUG_CMD(std::cout << std::endl);
	}


	void OnMove710::init()
	{
		//check if getPath() is a valid path
		DIR* folder = opendir(getPath().c_str());
		if (folder == NULL)
		{
			std::cout<< "Error: path '" << getPath() << "' does not exist (check option -p <path> on command line or line path=<path> in configuration file)." << std::endl;
			throw std::exception();
		}
		closedir(folder);
	}

	void OnMove710::release()
	{
		DEBUG_CMD(std::cout << "OnMove710: Release (nothing to do)" << std::endl);
	}

	std::string OnMove710::getPath()
	{
		return _configuration["path"];
	}

	void OnMove710::getSessionsList(SessionsMap *oSessions)
	{
		DEBUG_CMD(std::cout << "OnMove710: Get sessions list !" << std::endl);

		DIR* folder = NULL;
		struct dirent* file = NULL;
		folder = opendir(getPath().c_str());
		if (folder == NULL)
		{
			std::cerr << "Couldn't open dir " << getPath() << std::endl;
			throw std::exception();
		}

		std::set<std::string> filenamesPrefix;

		while ((file = readdir(folder)) != NULL)
		{
			std::string fn = std::string(file->d_name);
			if(strstr(file->d_name,".GHP") || strstr(file->d_name,".GHT") || strstr(file->d_name,".GHL"))
			{
				std::string fileprefix = fn.substr(0,fn.length() - 4);
				std::string filepathprefix = getPath() + std::string("/") + fileprefix;
				//check if all 3 files (with the 3 extensions) exists
				if(fileExists(filepathprefix + ".GHP") && fileExists(filepathprefix + ".GHT") && fileExists(filepathprefix + ".GHL"))
				{
					filenamesPrefix.insert(fileprefix);
				}
				else
				{
					std::cout << "Discarding " << fileprefix << std::endl;
				}
			}
		}

		closedir(folder);

		int i = 0;
		std::set<std::string>::iterator iter;
		for (iter=filenamesPrefix.begin(); iter!=filenamesPrefix.end(); ++iter)
		{
			std::string fileprefix = *iter;

			DEBUG_CMD(std::cout << "Decode summary of session " << fileprefix << std::endl);
			// Decoding of basic info about the session
			std::vector<char> _sessionId(fileprefix.begin(), fileprefix.end());
			SessionId id = SessionId(_sessionId);
			uint32_t num = i++; //Just increment by one each time

			tm time = parseFilename(fileprefix);

			double duration = 0;
			uint32_t distance = 0;
			uint32_t nbLaps = 999;

			Session mySession(id, num, time, 0, duration, distance, nbLaps);

			// Properly fill necessary session info (duration, distance, nbLaps)
			std::string ghtFilename = getPath() + std::string("/") + fileprefix + std::string(".GHT");
			size_t size = -1;
			unsigned char* buffer = readAllBytes(ghtFilename, size);
			parseGHTFile(buffer, &mySession);
			delete buffer;

			oSessions->insert(SessionsMapElement(id, mySession));
		}
	}

	void OnMove710::getSessionsDetails(SessionsMap *oSessions)
	{
		for(SessionsMap::iterator it = oSessions->begin(); it != oSessions->end(); ++it)
		{
			Session* session = &(it->second);
			SessionId sessionId = it->second.getId();
			std::string filenamePrefix(sessionId.begin(),sessionId.end());	
			std::cout << "Retrieve session " << filenamePrefix << std::endl;

			filenamePrefix = getPath() + std::string("/") + filenamePrefix;

			unsigned char* buffer;
			size_t size = -1;

			std::string ghtFilename = filenamePrefix + std::string(".GHT");
			buffer = readAllBytes(ghtFilename,size);
			parseGHTFile(buffer,session);
			delete buffer;

			std::string ghlFilename = filenamePrefix + std::string(".GHL");
			buffer = readAllBytes(ghlFilename,size);
			parseGHLFile(buffer,size,session);
			delete buffer;

			std::string ghpFilename = filenamePrefix + std::string(".GHP");
			buffer = readAllBytes(ghpFilename,size);
			parseGHPFile(buffer,size,session);
			delete buffer;
		}
	}

	void OnMove710::dumpInt2(std::ostream &oStream, unsigned int aInt)
	{
		oStream << (char)(aInt & 0xFF) << (char)((aInt & 0xFF00) >> 8);
	}

	void OnMove710::dumpInt4(std::ostream &oStream, unsigned int aInt)
	{
		oStream << (char)(aInt & 0xFF) << (char)((aInt & 0xFF00) >> 8) << (char)((aInt & 0xFF0000) >> 16) << (char)((aInt & 0xFF000000) >> 24);
	}

	void OnMove710::dumpString(std::ostream &oStream, const std::string &aString, size_t aLength)
	{
		size_t toCopy = aString.length();
		if(aLength <= toCopy) toCopy = aLength - 1;
		oStream.write(aString.c_str(), toCopy);
		for(size_t i = toCopy; i < aLength; ++i)
		{
			oStream.put('\0');
		}
	}

	void OnMove710::exportSession(Session *iSession)
	{
		std::string filename;
		int filenumber = 0;
		do
		{
			std::ostringstream oss;
			oss << getPath() << "/" << "E9HG" << std::setw(4) << std::setfill('0') << (filenumber % 10000) << ".GHR";
			filename = oss.str();
			filenumber++;
		} while(fileExists(filename));

		std::ofstream fl;
		fl.open(filename.c_str(), std::ios::out | std::ios::binary);
		// First is session name (align on 8 or 16 bytes ?)
		dumpString(fl, iSession->getName(), 16);
		// Then comes distance on 4 bytes
		dumpInt4(fl, iSession->getDistance());
		// 4 bytes of padding ?
		dumpInt4(fl, 0);
		// Ascent & descent, each on 2 bytes
		dumpInt2(fl, iSession->getAscent());
		dumpInt2(fl, iSession->getDescent());
		// Number of points
		dumpInt4(fl, iSession->getPoints().size());
		// Then 8 bytes per point: 4 bytes for latitude, 4 bytes for longitude
		int i = 0;
		for(std::list<Point*>::iterator it = iSession->getPoints().begin(); it != iSession->getPoints().end(); ++it)
		{
			unsigned int lat = (*it)->getLatitude() * 1000000;
			unsigned int lon = (*it)->getLongitude() * 1000000;
			dumpInt4(fl, lat);
			dumpInt4(fl, lon);
			++i;
			// TODO: Maximum number of points should be 100 ?
			if(i >= 200)
			{
				std::cerr << "Error: Too much points to export - truncating session" << std::endl;
				break;
			}
		}
		// Then padding up to 200 points
		while(i < 200)
		{
			dumpInt4(fl, 0);
			dumpInt4(fl, 0);
			++i;
		}
		fl.close();
		std::cout << "Transferred session " << iSession->getName() << std::endl;
	}

	void OnMove710::parseGHPFile(unsigned char* bytes, int length, Session* session)
	{
		unsigned char* chunk;
		time_t current_time = session->getTime();
		uint32_t cumulated_tenth = 0;
		uint32_t id_point = 0;
		std::list<Lap*>::iterator lap = session->getLaps().begin();

		for(int i=0; i<length; i=i+20)
		{
			chunk = &bytes[i];
			double latitude = ((double)bytesToInt4(chunk[0],chunk[1],chunk[2],chunk[3])) / 1000000.;//bytesToInt(chunk.slice(0, 4)) / 1000000,
			double longitude = ((double)bytesToInt4(chunk[4],chunk[5],chunk[6],chunk[7])) / 1000000.;//bytesToInt(chunk.slice(4, 8)) / 1000000,
			int16_t altitude = bytesToInt2(chunk[8],chunk[9]);//bytesToInt(chunk.slice(8, 10)),
			double speed = ((double)bytesToInt2(chunk[10],chunk[11])) / 100.0;//bytesToInt(chunk.slice(10, 12)) *10/3600,//[dm/h]=>[m/s]
			uint16_t heartRate = (unsigned char)chunk[12];//bytesToInt(chunk[12]),
			uint16_t status = chunk[13];//bytesToInt(chunk[13]),
			//TODO: Don't know what at 14-15
			Point *p = new Point(latitude, longitude, altitude, speed, current_time, cumulated_tenth*100, heartRate, status);
			session->addPoint(p);
			// cumulated_tenth contains delay before next point
			cumulated_tenth += bytesToInt4(chunk[16],chunk[17],chunk[18],chunk[19]);//bytesToInt(chunk.slice(16, 20))//[s/10]

			current_time += cumulated_tenth / 10;
			cumulated_tenth = cumulated_tenth % 10;

			if(lap != session->getLaps().end() && id_point == (*lap)->getFirstPointId())
			{
				(*lap)->setStartPoint(session->getPoints().back());
			}
			while(lap != session->getLaps().end() && id_point >= (*lap)->getLastPointId())
			{
				// This if is a safe net but should never be used (unless laps are not in order or first lap doesn't start at 0 or ...)
				if((*lap)->getStartPoint() == NULL)
				{
					std::cerr << "Error: lap has no start point and yet I want to go to the next lap ! (lap: " << (*lap)->getFirstPointId() << " - " << (*lap)->getLastPointId() << ")" << std::endl;
					(*lap)->setStartPoint(session->getPoints().back());
				}
				(*lap)->setEndPoint(session->getPoints().back());
				++lap;
				if(lap != session->getLaps().end())
				{
					(*lap)->setStartPoint(session->getPoints().back());
				}
			}
			id_point++;
		}
	}

	void OnMove710::parseGHLFile(unsigned char* bytes, int length, Session *session)
	{
		unsigned char* chunk;
		DEBUG_CMD(int lapIndex = 0);
		for(int i=0;i<length;i=i+48)
		{
			chunk = &bytes[i];

			//double accruedTime = bytesToInt4(chunk[0],chunk[1],chunk[2],chunk[3]) / 10.0;//bytesToInt(chunk.slice(0, 4)) * 100, // [s/10] => [ms]
			double totalTime = bytesToInt4(chunk[4],chunk[5],chunk[6],chunk[7]) / 10.0;//bytesToInt(chunk.slice(4, 8)) * 1000, // [s] => [ms]
			int totalDistance = bytesToInt4(chunk[8],chunk[9],chunk[10],chunk[11]);//bytesToInt(chunk.slice(8, 12)),
			double maxSpeed = bytesToInt2(chunk[12],chunk[13]) / 100.0;//bytesToInt(chunk.slice(12, 14)) * 10 / 3600, // [dm/h] => [m/s]
			double averageSpeed = bytesToInt2(chunk[14],chunk[15]) / 100.0;//bytesToInt(chunk.slice(14, 16))*10/3600,[dm/h]=>[m/s]
			/* Really sure it's maxPace and averagePace ?! */
			//int maxPace = bytesToInt2(chunk[16],chunk[17]);//bytesToInt(chunk.slice(16, 18)),
			//int averagePace =bytesToInt2(chunk[18],chunk[19]);//bytesToInt(chunk.slice(18, 20)),
			int maxHeartRate = chunk[20];//bytesToInt(chunk[20]),
			int averageHeartRate = chunk[21];//bytesToInt(chunk[21]),
			int averageCalory = bytesToInt2(chunk[22],chunk[23]);//bytesToInt(chunk.slice(22, 24)),
			//int maxCalory =bytesToInt2(chunk[24],chunk[25]);//bytesToInt(chunk.slice(24, 26)),
			//int totalCalory = bytesToInt2(chunk[26],chunk[27]);//bytesToInt(chunk.slice(26, 28)),
			int weightLoss = bytesToInt2(chunk[28],chunk[29]);//bytesToInt(chunk.slice(28, 30)),
			//int descent = bytesToInt2(chunk[30],chunk[31]);//bytesToInt(chunk.slice(30, 32)),
			//int ascent = bytesToInt2(chunk[32],chunk[33]);//bytesToInt(chunk.slice(32, 34)),
			//int trainingType = chunk[34];//bytesToInt(chunk[34]),//0:not training; 1:warmup; 2:action; 3:rest; 4:break; 5: cool
			//int wAccruedDistance = bytesToInt2(chunk[35],chunk[36]);//bytesToInt(chunk.slice(35, 37)),
			//int cAccruedDistance = chunk[37];//bytesToInt(chunk[37]),
			/* Previous values were not working on the dump I had */
			uint32_t startPoint = bytesToInt2(chunk[40],chunk[41]);//bytesToInt(chunk.slice(38, 40)),
			uint32_t endPoint = bytesToInt2(chunk[42],chunk[43]);//bytesToInt(chunk.slice(40, 42)),
			/* Not sur about this !!! */
			int averageAscent = bytesToInt2(chunk[44],chunk[45]);//bytesToInt(chunk.slice(42, 44)),
			int averageDescent = bytesToInt2(chunk[46],chunk[47]);//bytesToInt(chunk.slice(44, 46))

			DEBUG_CMD(std::cout << "Lap " << lapIndex++ << std::endl);
			//TODO:
			/*uint32_t firstPointId;
			  uint32_t lastPointId;
			  double duration;
			  uint32_t distance;
			  double max_speed;
			  double avg_speed;
			  double max_hr;
			  double avg_hr;
			  uint32_t calories;
			  uint32_t grams;
			  uint32_t descent;
			  uint32_t ascent;*/
			Lap *l = new Lap(startPoint, endPoint, totalTime, totalDistance, maxSpeed, averageSpeed, maxHeartRate, averageHeartRate, averageCalory, weightLoss, averageDescent, averageAscent);
			session->addLap(l);
		}
	}

	void OnMove710::parseGHTFile(const unsigned char* bytes,Session* session)
	{
		/*
		int year = bytes[0];
		int month = bytes[1];
		int day = bytes[2];
		int hour = bytes[3];
		int minutes = bytes[4];
		int seconds = bytes[5];
		*/

		int totalPoint = bytesToInt2(bytes[6],bytes[7]);//bytesToInt(bytes.slice(6, 8)),
		double totalTime = bytesToInt4(bytes[8],bytes[9],bytes[10],bytes[11]) / 10.0;//bytes.slice(8, 12)) * 100, // [s/10] => [s]
		int totalDistance = bytesToInt4(bytes[12],bytes[13],bytes[14],bytes[15]);//bytesToInt(bytes.slice(12, 16)),
		int lapCount = bytesToInt2(bytes[16],bytes[17]);//bytesToInt(bytes.slice(16, 18)),
		//int ptRec = bytesToInt2(bytes[18],bytes[19]);//bytesToInt(bytes.slice(18, 20)),
		//int lapRec = bytesToInt2(bytes[20],bytes[21]);//bytesToInt(bytes.slice(20, 22)),
		//int isChallenge = bytes[22];/*TODO check why'!!'*///!!bytesToInt(bytes[22]),
		//int challengeType = bytes[23];
		char challengeName[16];
		strncpy(challengeName,(const char*)&bytes[24],16);//bytesToString(bytes.slice(24, 40).filter(BYTE_UTILS.suspiciousBytesForString)),
		//int guestTotalDuration =  bytesToInt4(bytes[40],bytes[41],bytes[42],bytes[43]);//bytesToInt(bytes.slice(40, 44)),
		//int guestDistance = bytesToInt4(bytes[44],bytes[45],bytes[46],bytes[47]);//bytesToInt(bytes.slice(44, 48)),
		//int guestAverageSpeed = bytesToInt2(bytes[48],bytes[49]) / 100.0;//bytesToInt(bytes.slice(48, 50)) * 10 / 3600,//[dm/h]=>[m/s]
		//int guestAveragePace = bytesToInt2(bytes[50],bytes[51]);//bytesToInt(bytes.slice(50, 52)),
		double maxSpeed = bytesToInt2(bytes[52],bytes[53]) / 100.0;//bytesToInt(bytes.slice(52, 54)) * 10 / 3600, // [dm/h] => [m/s]
		double averageSpeed = bytesToInt2(bytes[54],bytes[55]) / 100.0;//bytesToInt(bytes.slice(54, 56)) * 10 / 3600,//[dm/h]=>[m/s]
		//int maxPace = bytesToInt2(bytes[56],bytes[57]);//bytesToInt(bytes.slice(56, 58)),
		//int averagePace = bytesToInt2(bytes[58],bytes[59]);//bytesToInt(bytes.slice(58, 60)),
		int maxHeartRate =  bytes[60];
		int averageHeartRate = bytes[61];
		//int averageCalory = bytesToInt2(bytes[62],bytes[63]);//bytesToInt(bytes.slice(62, 64)),
		//int maxCalory = bytesToInt2(bytes[64],bytes[65]);//bytesToInt(bytes.slice(64, 66)),
		int totalCalory = bytesToInt2(bytes[66],bytes[67]);//bytesToInt(bytes.slice(66, 68)),
		int weightLoss = bytesToInt2(bytes[68],bytes[69]);//bytesToInt(bytes.slice(68, 70)), // [g]
		int ascent = bytesToInt2(bytes[70],bytes[71]);//bytesToInt(bytes.slice(70, 72)),
		int descent = bytesToInt2(bytes[72],bytes[73]);//bytesToInt(bytes.slice(72, 74)),
		//int activityType = bytes[74]; // 0 : free ; 1 : training ; 2 : challenge ; 3 : indoor
		//int selectRoute = bytes[75];  // 0 or 1
		char trainingName[16];//bytesToString(bytes.slice(76, 92).filter(BYTE_UTILS.suspiciousBytesForString)),
		strncpy(trainingName,(const char*)&bytes[76],16);
		//int averageAscent = bytesToInt2(bytes[92],bytes[93]);// BYTE_UTILS.bytesToInt(bytes.slice(92, 94)),
		//int averageDescent = bytesToInt2(bytes[94],bytes[95]);// BYTE_UTILS.bytesToInt(bytes.slice(94, 96))

		session->setNbLaps(lapCount);
		session->setNbPoints(totalPoint);
		session->setDistance(totalDistance);
		session->setDuration(totalTime);
		session->setMaxSpeed(maxSpeed);
		session->setAvgSpeed(averageSpeed);
		session->setMaxHr(maxHeartRate);
		session->setAvgHr(averageHeartRate);
		session->setCalories(totalCalory);
		session->setGrams(weightLoss);
		session->setAscent(ascent);
		session->setDescent(descent);
	}
}
