#include <libusb.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <map>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <algorithm>
#include <iterator>

#ifndef DEBUG
#define DEBUG 0
#endif

typedef std::vector<char> SessionId;

std::string durationAsString(double sec, bool with_hundredth = false)
{
	uint32_t days = sec / (24*3600);
	sec -= days * 24 * 3600; 
	uint32_t hours = sec / 3600;
	sec -= hours * 3600; 
	uint32_t minutes = sec / 60;
	sec -= minutes * 60; 
	uint32_t seconds = sec;
	sec -= seconds;
	uint32_t hundredth = sec * 100;
	std::string result;
	std::ostringstream oss;
	if(days != 0)
		oss << days << "d ";
	if(hours != 0 or days != 0)
		oss << hours << "h";
	if(minutes != 0 or hours != 0 or days !=0)
		oss << minutes << "m";
	oss << seconds << "s";
	if(with_hundredth)
		oss << std::setw(2) << std::setfill('0') << hundredth;
	return oss.str();
}

class Point
{
	public:
		Point(double _lat, double _lon, int16_t _alt, double _speed, time_t _time, uint32_t _tenth, uint16_t _bpm, uint16_t _fiability) 
		{ 
			lat = _lat; 
			lon = _lon; 
			alt = _alt; 
			speed = _speed; 
			time = _time; 
			tenth = _tenth; 
			bpm = _bpm;
			fiability = _fiability;
		};
		double getLatitude() { return lat; };
		double getLongitude() { return lon; };
		int16_t getAltitude() { return alt; };
		double getSpeed() { return speed; };
		uint16_t getHeartRate() { return bpm; };
		uint16_t getFiability() { return fiability; };
		std::string getTime() 
		{  
			char buffer[256];
			tm *time_tm = localtime(&time);
			std::stringstream format_string;
			format_string << "%Y-%m-%dT%H:%M:%S." << tenth << "Z";
			if(tenth > 9)
			{
				std::cout << "Error: tenth bigger than 9 ! This is totally unexpected ..." << std::endl;
			}
			strftime(buffer, 256, format_string.str().c_str(), time_tm);
			return std::string(buffer);
		};

	private:
		double lat;
		double lon;
		int16_t alt;
		double speed;
		time_t time;
		uint32_t tenth;
		uint16_t bpm;
		uint16_t fiability;
};

class Lap
{
	public:
		Lap(uint32_t _firstPoint, uint32_t _lastPoint, double _duration, uint32_t _length, double _max_speed, double _avg_speed, uint32_t _calories, uint32_t _grams, uint32_t _descent, uint32_t _ascent)
		{ 
			firstPoint = _firstPoint; 
			lastPoint = _lastPoint; 
			start = NULL;
			end = NULL;
			duration = _duration; 
			length = _length; 
			max_speed = _max_speed; 
			avg_speed = _avg_speed; 
			calories = _calories; 
			grams = _grams; 
			descent = _descent; 
			ascent = _ascent;
		};

		void setStartPoint(Point *_start) { start = _start; };
		void setEndPoint(Point *_end) { end = _end; };

		uint32_t getFirstPointId() { return firstPoint; };
		uint32_t getLastPointId() { return lastPoint; };
		Point *getStartPoint() { return start; };
		Point *getEndPoint() { return end; };
		double getDuration() { return duration; };
		uint32_t getCalories() { return calories; };
		uint32_t getLength() { return length; };

	private:
		uint32_t firstPoint;
		uint32_t lastPoint;
		Point *start;
		Point *end;
		double duration;
		uint32_t length;
		double max_speed;
		double avg_speed;
		uint32_t calories;
		uint32_t grams;
		uint32_t descent;
		uint32_t ascent;
};

class SessionInfo
{
	public:
		SessionInfo(unsigned char* line)
		{
			/*
			std::cout << std::hex;
			for(int i = 0; i < 24; ++i)
			{
				std::cout << std::setw(2) << std::setfill('0') << (int) line[i] << " ";
			}
			std::cout << std::dec << std::endl;
			*/
			id = SessionId(line, line+16);
			num = (line[17] << 8) + line[18];
			// In tm, year is year since 1900. GPS returns year since 2000
			time.tm_year = 100 + line[0];
			// In tm, month is between 0 and 11.
			time.tm_mon = line[1] - 1;
			time.tm_mday = line[2];
			time.tm_hour = line[3];
			time.tm_min = line[4];
			time.tm_sec = line[5];
			time.tm_isdst = -1;
			//std::cout << "TM is:" << time.tm_year << "-" << time.tm_mon << "-" << time.tm_mday << " " << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << std::endl;

			nb_points = line[6] + (line[7] << 8);
			duration = (line [8] + (line[9] << 8) + (line[10] << 16) + (line[11] << 24)) / 10;
			distance = line [12] + (line[13] << 8) + (line[14] << 16) + (line[15] << 24);

			// nb_laps has no interest as we read laps later except to display it in the list of sessions before import
			nb_laps = line[16] + (line[17] << 8);
			// TODO: also read race type and runner's place (if opponent)

			current_time = mktime(&time);
			cumulated_tenth = 0;
		};

		void print()
		{
			char timeString[255];
			size_t length = strftime(timeString, 255, "%c", &time);
			std::cout << std::hex << std::setw(8) << num << std::dec;
			std::cout << " " << timeString << " (" << nb_points << " points) " << std::endl;
			for(std::list<Point>::iterator it = points.begin(); it != points.end(); ++it)
			{
				std::cout << "    (" << it->getLatitude() << ", " << it->getLongitude() << ", " << it->getAltitude() << ")" << std::endl;
			}
			std::cout << std::endl;
		}

		void addPoint(unsigned char* line)
		{
			// TODO: All this decoding should be in point constructor !
			double lat = ((double)(line[0] + (line[1] << 8) + (line[2] << 16) + (line[3] << 24))) / 1000000;
			double lon = ((double)(line[4] + (line[5] << 8) + (line[6] << 16) + (line[7] << 24))) / 1000000;
			// Altitude can be signed (yes, I already saw negative ones with the watch !) and is on 16 bits
			int16_t alt = line[8] + (line[9] << 8);
			double speed = ((double)(line[10] + (line[11] << 8)) / 100);
			uint16_t bpm = line[12];
			uint16_t fiability = line[13];
			cumulated_tenth += line[16] + (line[17] << 8);
			current_time += cumulated_tenth / 10;
			cumulated_tenth = cumulated_tenth % 10;
			Point point(lat, lon, alt, speed, current_time, cumulated_tenth, bpm, fiability);
			points.push_back(point);
		}

		void addLap(unsigned char* line)
		{
			static uint32_t sum_calories = 0;
			// time_t lap_end = lap_start + (line[0] + (line[1] << 8) + (line[2] << 16) + (line[3] << 24)) / 10;
			double duration = (line[4] + (line[5] << 8) + (line[6] << 16) + (line[7] << 24)) / 10;
			// last_lap_end = lap_end;

			uint32_t length = line[8] + (line[9] << 8) + (line[10] << 16) + (line[11] << 24);
			double max_speed = (line[12] + (line[13] << 8)) / 100;
			double avg_speed = (line[14] + (line[15] << 8)) / 100;
			uint32_t calories = line[26] + (line[27] << 8);
			// Calories for lap given by watch is the sum of alll past laps (this looks like a bug ?! this may change with later firmwares !)
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
			Lap lap(firstPoint, lastPoint, duration, length, max_speed, avg_speed, calories, grams, descent, ascent);
			laps.push_back(lap);
		}

		std::string getBeginTime(bool human_readable=false) 
		{ 
			time_t begin_time = mktime(&time);
			char buffer[256];
			tm *begin_time_tm = localtime(&begin_time);
			if(human_readable)
				strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", begin_time_tm);
			else
				strftime(buffer, 256, "%Y-%m-%dT%H:%M:%SZ", begin_time_tm);
			return std::string(buffer);
		};

		uint32_t getNum() { return num; };
		bool isComplete() 
		{ 
			if(DEBUG == 1)
			{
				std::cout << "isComplete on " << num << ": " << nb_points << " == " << points.size() << std::endl; 
			}
			return nb_points == points.size(); 
		};

		std::list<Lap> &getLaps() { return laps; };
		std::list<Point> &getPoints() { return points; };

		SessionId getId() { return id; };
		std::string getName() { return "No name"; };
		//tm *getTime() { return &time; };
		int getYear() { return time.tm_year + 1900; };
		int getMonth() { return time.tm_mon + 1; };
		int getDay() { return time.tm_mday; };
		int getHour() { return time.tm_hour; };
		int getMinutes() { return time.tm_min; };
		int getSeconds() { return time.tm_sec; };
		double getDuration() { return duration; };
		uint32_t getDistance() { return distance; };
		uint32_t getNbLaps() { return nb_laps; };

	private:
		SessionId id;
		uint32_t num;
		tm time;
		uint32_t nb_points;
		double duration;
		uint32_t distance;
		uint32_t nb_laps;
		std::list<Lap> laps;
		std::list<Point> points;
		time_t current_time;
		uint32_t cumulated_tenth;
};

typedef std::map<SessionId, SessionInfo> SessionsMap;
typedef std::pair<SessionId, SessionInfo> SessionsMapElement;

bool checkUSBOperation(int rc)
{
	if(rc < 0)
	{
		std::cerr << "Transmission error: " << rc << " - " << errno;
		switch(rc)
		{
			case LIBUSB_ERROR_IO:
				std::cerr << " LIBUSB_ERROR_IO";
				break;
			case LIBUSB_ERROR_ACCESS:
				std::cerr << " LIBUSB_ERROR_ACCESS";
				break;
			case LIBUSB_ERROR_NO_DEVICE:
				std::cerr << " LIBUSB_ERROR_NO_DEVICE";
				break;
			case LIBUSB_ERROR_TIMEOUT:
				std::cerr << " LIBUSB_ERROR_TIMEOUT";
				break;
			case LIBUSB_ERROR_BUSY:
				std::cerr << " LIBUSB_ERROR_BUSY";
				break;
			default:
				std::cerr << " Unknown error";
				break;
		}
		std::cerr << std::endl;
		return false;
	}
	return true;
}

// TODO: Yes I know, all those global vars are ugly ... 
#define TIMEOUT 5000
#define RESPONSE_BUFFER_SIZE 4096
unsigned char responseData[RESPONSE_BUFFER_SIZE];
const int lengthDataDevice = 5;
unsigned char dataDevice[lengthDataDevice] = { 0x02, 0x00, 0x01, 0x85, 0x84 };
const int lengthDataList = 5;
unsigned char dataList[lengthDataList] = { 0x02, 0x00, 0x01, 0x78, 0x79 };
const int lengthMoreData = 5;
unsigned char moreData[lengthMoreData] = { 0x02, 0x00, 0x01, 0x81, 0x80 };
std::map<std::string, std::string> configuration;
std::string logfilename;
// Used if input file is given
bool useInputStream;
std::vector<std::string> inputStream;
uint32_t inputIndex;

bool USBsend(libusb_device_handle *USBDevice, unsigned char *data, size_t length)
{
	bool ok = true;
	if(!useInputStream)
	{
		// Display what is sent
		if(DEBUG == 1)
		{
			std::cout << "Sending data: " << std::endl;

			/*
			   std::cout.write(reinterpret_cast<char*>(data), length);
			   std::cout << std::endl;
			 */

			std::cout << std::hex;
			for(int i = 0; i < length; ++i)
			{
				std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ";
			}
			std::cout << std::dec << std::endl;
		}
		int transferred;
		int rc = libusb_bulk_transfer(USBDevice, 0x03, data, length, &transferred, TIMEOUT);
		ok = checkUSBOperation(rc);
		if(transferred != length)
		{
			std::cerr << "Error: transferred (" << transferred << ") != length (" << length << ")" << std::endl;
			ok = false;
		}
	}
	return ok;
}

int USBreceive(libusb_device_handle *USBDevice)
{
	int transferred;
	if(useInputStream)
	{
		transferred = inputStream[inputIndex].length();
		memcpy(responseData, inputStream[inputIndex].c_str(), transferred);
		inputIndex++;
	}
	else
	{
		int rc = libusb_bulk_transfer(USBDevice, 0x81, responseData, RESPONSE_BUFFER_SIZE, &transferred, TIMEOUT);
		checkUSBOperation(rc);

		// Display result of call
		if(DEBUG == 1)
		{
			std::cout << "Received data: " << std::endl;

			/*
			   std::cout.write(reinterpret_cast<char*>(responseData), transferred);
			   std::cout << std::endl;
			 */

			std::cout << std::hex;
			for(int i = 0; i < transferred; ++i)
			{
				std::cout << std::setw(2) << std::setfill('0') << (int) responseData[i] << " ";
			}
			std::cout << std::dec << std::endl;
		}
		if(configuration["log_transactions"] == "yes")
		{
			std::ofstream log_file_stream(logfilename.c_str(), std::ios_base::out | std::ios_base::app);
			log_file_stream << std::hex;
			for(int i = 0; i < transferred; ++i)
			{
				log_file_stream << std::setw(2) << std::setfill('0') << (int) responseData[i] << " ";
			}
			log_file_stream << std::dec << std::endl;
			log_file_stream.close();
		}
	}
	return transferred;
}

bool openUSBDevice(libusb_device_handle **USBHandle)
{
	libusb_context *myUSBContext;
	libusb_device **listOfDevices;
	bool found = false;
	int rc = 0;

	libusb_init(&myUSBContext);
	ssize_t nbDevices = libusb_get_device_list(myUSBContext, &listOfDevices);
	if (nbDevices < 0)
	{
		std::cerr << "Error retrieving USB devices list: " << nbDevices << std::endl;
		return false;
	}
	for(int i = 0; i < nbDevices; ++i)
	{
		libusb_device_descriptor deviceDescriptor;
		libusb_get_device_descriptor(listOfDevices[i], &deviceDescriptor);
		if(deviceDescriptor.idVendor == 0x0483 && deviceDescriptor.idProduct == 0x5740)
		{
			libusb_open(listOfDevices[i], USBHandle);
			found = true;
		}
	}

	if(found)
	{
		rc = libusb_kernel_driver_active(*USBHandle, 0);
		found = found && checkUSBOperation(rc);

		if (rc > 0)
		{
			rc = libusb_detach_kernel_driver(*USBHandle, 0); 
			found = found && checkUSBOperation(rc);
		}

		rc = libusb_claim_interface(*USBHandle, 0);
		found = found && checkUSBOperation(rc);

		rc = libusb_claim_interface(*USBHandle, 1);
		found = found && checkUSBOperation(rc);
	}

	libusb_free_device_list(listOfDevices, 1);
	return found;
}

bool closeUSBDevice(libusb_device_handle *USBHandle)
{
	int rc;
	if (USBHandle) 
	{
		rc = libusb_release_interface(USBHandle, 0);
		checkUSBOperation(rc);
		rc = libusb_release_interface(USBHandle, 1);
		checkUSBOperation(rc);

		libusb_close(USBHandle);
	}
	libusb_exit(NULL); 
}

bool getDeviceInfo(libusb_device_handle *USBDevice)
{
	USBsend(USBDevice, dataDevice, lengthDataDevice);
	USBreceive(USBDevice);

	return true;
}

bool getList(libusb_device_handle *USBDevice, SessionsMap *sessions)
{
	USBsend(USBDevice, dataList, lengthDataList);
	int received = USBreceive(USBDevice);

	if(responseData[0] != 0x78)
	{
		std::cerr << "Unexpected header for getList: " << std::hex << (int) responseData[0] << std::dec << std::endl;
		return false;
	}
	int size = responseData[2] + (responseData[1] << 8);
	if(size + 4 != received)
	{
		std::cerr << "Unexpected size in header for getList: " << responseData[2] << " (!= " << received << " - 4)" << std::endl;
		return false;
	}
	int nbRecords = size / 24;
	if(nbRecords * 24 != size)
	{
		std::cerr << "Size is not a multiple of 24 in getList !" << std::endl;
		return false;
	}
	for(int i = 0; i < nbRecords; ++i)
	{
		SessionInfo mySession(&responseData[24*i+3]);
		sessions->insert(SessionsMapElement(mySession.getId(), mySession));
	}
	return true;
}

bool getSessionsDetails(libusb_device_handle *USBDevice, SessionsMap *sessions)
{
	int length = 7 + 2*sessions->size();
	unsigned char data[length];
	data[0] = 0x02;
	data[1] = 0x00;
	data[2] = length - 4;
	data[3] = 0x80;
	data[4] = sessions->size() & 0xFF;
	data[5] = sessions->size() & 0xFF00;
	int i = 6;
	for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); ++it)
	{
		data[i++] = it->second.getNum() & 0xFF;
		data[i++] = it->second.getNum() & 0xFF00;
	}
	int received = 0;
	unsigned char checksum = 0;
	for(int i = 2; i < length-1; ++i)
	{
		checksum ^= data[i];
	}
	data[length-1] = checksum;

	USBsend(USBDevice, data, length);

	do
	{
		// TODO: Store info retrieved by this first call somewhere (some data global to the session: max & avg speed, calories, grams, ascent, descent ...)
		USBreceive(USBDevice);
		if(responseData[0] == 0x8A) break;

		USBsend(USBDevice, moreData, lengthMoreData);
		received = USBreceive(USBDevice);
		if(responseData[0] == 0x8A) break;
		if(responseData[0] != 0x80)
		{
			std::cerr << "Unexpected header for getSessionsDetails (step 2): " << (int)responseData[0] << std::endl;
			return false;
		}
		int size = responseData[2] + (responseData[1] << 8);
		if(size + 4 != received)
		{
			std::cerr << "Unexpected size in header for getSessionsDetails (step 2): " << size << " != " << received << " - 4" << std::endl;
			return false;
		}
		SessionId id(responseData + 3, responseData + 19);
		SessionInfo *session = &(sessions->find(id)->second);
		int nbRecords = (size - 24)/ 44;
		if(nbRecords * 44 != size - 24)
		{
			std::cerr << "Size is not a multiple of 44 plus 24 in getSessionsDetails (step 2) !" << std::endl;
			return false;
		}
		for(int i = 0; i < nbRecords; ++i)
		{
			session->addLap(&responseData[44*i + 27]);
		}

		uint32_t id_point = 0;
		bool keep_going = true;
		while(keep_going)
		{
			USBsend(USBDevice, moreData, lengthMoreData);
			received = USBreceive(USBDevice);
			if(responseData[0] == 0x8A) break;

			if(responseData[0] != 0x80)
			{
				std::cerr << "Unexpected header for getSessionsDetails (step 3): " << (int)responseData[0] << std::endl;
				return false;
			}
			int size = responseData[2] + (responseData[1] << 8);
			if(size + 4 != received)
			{
				std::cerr << "Unexpected size in header for getSessionsDetails (step 3): " << size << " != " << received << " - 4" << std::endl;
				return false;
			}
			SessionId id(responseData + 3, responseData + 19);
			SessionInfo *session = &(sessions->find(id)->second);
			int nbRecords = (size - 24)/ 20;
			if(nbRecords * 20 != size - 24)
			{
				std::cerr << "Size is not a multiple of 20 plus 24 in getSessionsDetails (step 3) !" << std::endl;
				return false;
			}
			std::list<Lap>::iterator lap = session->getLaps().begin();
			while(id_point >= lap->getLastPointId() && lap != session->getLaps().end())
			{
				++lap;
			}
			// TODO: Cleaner way to handle id_point ?
			for(int i = 0; i < nbRecords; ++i)
			{
				//std::cout << "We should have " << lap->getFirstPointId() << " <= " << id_point << " <= " << lap->getLastPointId() << std::endl;
				session->addPoint(&responseData[20*i + 27]);
				if(id_point == lap->getFirstPointId())
				{
					lap->setStartPoint(&(session->getPoints().back()));
				}
				while(id_point >= lap->getLastPointId() && lap != session->getLaps().end())
				{
					// This if is a safe net but should never be used (unless laps are not in order or first lap doesn't start at 0 or ...)
					if(lap->getStartPoint() == NULL)
					{
						std::cerr << "Error: lap has no start point and yet I want to go to the next lap ! (lap: " << lap->getFirstPointId() << " - " << lap->getLastPointId() << ")" << std::endl;
						lap->setStartPoint(&(session->getPoints().back()));
					}
					lap->setEndPoint(&(session->getPoints().back()));
					++lap;
					//std::cout << "Calling setStartPoint for " << id_point << "on lap (" << lap->getFirstPointId() << " - " << lap->getLastPointId() << ")" << std::endl;
					lap->setStartPoint(&(session->getPoints().back()));
				}
				id_point++;
			}
			keep_going = !session->isComplete();
		}
		std::cout << "Retrieved session from " << session->getBeginTime() << std::endl;
		if(responseData[0] == 0x8A) break;

		USBsend(USBDevice, moreData, lengthMoreData);
	}
	// Redundant with all the if / break above !
	while(responseData[0] != 0x8A);

	return true;
}

bool createGPX(SessionInfo *session)
{
	std::stringstream filename;
	filename << configuration["directory"] << "/"; 
	filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_"; 
	filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".gpx";
	std::cout << "Creating " << filename.str() << std::endl;
	std::ofstream mystream(filename.str().c_str());
	// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
	mystream.precision(8);
	mystream << "<?xml version=\"1.0\"?>" << std::endl;
	mystream << "<gpx version=\"1.1\"" << std::endl;
	mystream << "     creator=\"Kalenji 300 Reader\"" << std::endl;
	mystream << "     xmlns=\"http://www.topografix.com/GPX/1/1\"" << std::endl;
	mystream << "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << std::endl;
	mystream << "     xmlns:gpxdata=\"http://www.cluetrust.com/XML/GPXDATA/1/0\"" << std::endl;
	mystream << "     xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1" << std::endl;
	mystream << "                          http://www.topografix.com/GPX/1/1/gpx.xsd\">" << std::endl;

	// TODO: Improve metadata (add link among others)
	mystream << "  <metadata>" << std::endl;
	mystream << "    <name>" << session->getName() << "</name>" << std::endl;
	mystream << "    <time>" << session->getBeginTime() << "</time>" << std::endl;
	mystream << "  </metadata>" << std::endl;

	mystream << "  <trk>" << std::endl;
	mystream << "    <trkseg>" << std::endl;
	std::list<Point> points = session->getPoints();
	for(std::list<Point>::iterator it = points.begin(); it != points.end(); ++it)
	{
		// TODO: More info on point ? (cadence, hr when available)
		// TODO: Give an option to filter on fiability, or add it in GPX ?
		if(it->getFiability() == 3)
		{
			mystream << "      <trkpt lat=\"" << it->getLatitude() << "\" lon=\"" << it->getLongitude() << "\">" << std::endl;
			mystream << "        <ele>" << it->getAltitude() << "</ele>" << std::endl;
			mystream << "        <time>" << it->getTime() << "</time>" << std::endl;
			mystream << "        <extensions>" << std::endl;
			mystream << "          <gpxdata:hr>" << it->getHeartRate() << "</gpxdata:hr>" << std::endl;
			mystream << "        </extensions>" << std::endl;
			mystream << "      </trkpt>" << std::endl;
		}
	}
	mystream << "    </trkseg>" << std::endl;
	mystream << "  </trk>" << std::endl;
	mystream << "  <extensions>" << std::endl;
	int lap = 0;
	std::list<Lap> laps = session->getLaps();
	for(std::list<Lap>::iterator it = laps.begin(); it != laps.end(); ++it)
	{
		if(it->getStartPoint() == NULL || it->getEndPoint() == NULL)
		{
			std::cerr << "Oups ! I've got a lap without ";
			if(it->getStartPoint() == NULL)
				std::cerr << "start";
			else
				std::cerr << "end";
			std::cerr << " point: (" << it->getFirstPointId() << " - " << it->getLastPointId() << "). This shouldn't happen ! Report a bug ..." << std::endl;
		}
		else
		{
			lap++;
			mystream << "    <gpxdata:lap>" << std::endl;
			mystream << "      <gpxdata:index>" << lap << "</gpxdata:index>" << std::endl;
			mystream << "      <gpxdata:startPoint lat=\"" << it->getStartPoint()->getLatitude() << "\" lon=\"" << it->getStartPoint()->getLongitude() << "\"/>" << std::endl;
			mystream << "      <gpxdata:endPoint lat=\"" << it->getEndPoint()->getLatitude() << "\" lon=\"" << it->getEndPoint()->getLongitude() << "\" />" << std::endl;
			mystream << "      <gpxdata:startTime>" << it->getStartPoint()->getTime() << "</gpxdata:startTime>" << std::endl;
			mystream << "      <gpxdata:elapsedTime>" << it->getDuration() << "</gpxdata:elapsedTime>" << std::endl;
			mystream << "      <gpxdata:calories>" << it->getCalories() << "</gpxdata:calories>" << std::endl;
			mystream << "      <gpxdata:distance>" << it->getLength() << "</gpxdata:distance>" << std::endl;
			// Heart rate is not available on my device but would be nice to add for CW 700:
			mystream << "      <gpxdata:summary name=\"AverageHeartRateBpm\" kind=\"avg\">0</gpxdata:summary>" << std::endl;
			// I didn't find a way to differentiate manual lap taking versus automatic (triggered by time or distance)
			// This is the correct syntax, but pytrainer doesn't support it
			//mystream << "      <gpxdata:trigger kind=\"" << configuration["trigger"] << "\" />" << std::endl;
			mystream << "      <gpxdata:trigger>" << configuration["trigger"] << "</gpxdata:trigger>" << std::endl;
			// What can I tell about this ?! Mandatory when using gpxdata (as the two previous one) so I put it with a default value ...
			mystream << "      <gpxdata:intensity>active</gpxdata:intensity>" << std::endl;
			mystream << "    </gpxdata:lap>" << std::endl;
		}
	}
	mystream << "  </extensions>" << std::endl;
	mystream << "</gpx>" << std::endl;
	mystream.close();
	return true;
}

int testDir(std::string path, bool create_if_not_exist)
{
	if(access(path.c_str(), W_OK) != 0)
	{
		switch(errno)
		{
			case ENOENT:
				if(create_if_not_exist)
				{
					mkdir(path.c_str(), 0777);
					return 1;
				}
				else
				{
					std::cerr << "Error: " << path << " doesn't exist and I couldn't create it" << std::endl;
					return -1;
				}
			case EACCES:
			case EROFS:
				std::cerr << "Error: Don't have write access in " << path << std::endl;
				return -1;
			case ENOTDIR:
				std::cerr << "Error: " << path << " is not a directory" << std::endl;
				return -1;
			default:
				std::cerr << "Error: Unknown reason (" << errno << ") preventing write access to " << path << std::endl;
				return -1;
		}
	}
	return 0;
}

bool checkAndCreateDir(std::string path)
{
	int dir_status = testDir(path, true);
	// If dir was tentatively created, second attempt
	if(dir_status > 0) dir_status = testDir(path, false);
	if(dir_status < 0) return false;
	return true;
}

void initLogFile()
{
	char buffer[256];
	time_t t = time(NULL);
	strftime(buffer, 256, "%Y%m%d_%H%M%S", localtime(&t));
	std::stringstream log_filename;
	log_filename << configuration["log_transactions_directory"] << "/"; 
	log_filename << "kalenji_reader_" << buffer << ".log";
	logfilename = log_filename.str();
}

void usage(char *progname)
{
	std::cout << "Usage: " << progname << " [-h | -i <input_file>]" << std::endl;
	std::cout << "  - h: help:       Show this help message " << std::endl;
	std::cout << "  - i: input file: Provide input file instead of reading from device" << std::endl;
}

bool readConf(int argc, char** argv)
{
	useInputStream = false;
	int option;
	while((option = getopt(argc, argv, ":hi:")) != -1)
	{
		switch(option)
		{
			case 'i':
				useInputStream = true;
				inputIndex = 0;
				if(access(optarg, R_OK) == 0)
				{
					std::string line;
					std::ifstream inputfile(optarg);
					if (inputfile.is_open())
					{
						while ( inputfile.good() )
						{
							std::string thisLine;
							getline(inputfile, line);
							std::stringstream iss(line);
							iss >> std::hex;
							std::copy(std::istream_iterator<unsigned int>(iss), std::istream_iterator<unsigned int>(), std::back_inserter(thisLine));
							inputStream.push_back(thisLine);
						}
						inputfile.close();
					}
					else 
					{
						std::cerr << "Unable to open " << inputfile; 
						return false;
					}
				}
				break;
			case 'h':
				usage(argv[0]);
				break;
			case '?':
				std::cerr << " Error, unknown option " << (char)optopt << std::endl;
				usage(argv[0]);
				return false;
		}
	}
	// Default conf
	configuration["directory"] = "/tmp/kalenji_import";
	configuration["import"] = "all";
	configuration["trigger"] = "manual";
	configuration["log_transactions"] = "yes";
	// Default value for log_transactions_directory is defined later (depends on directory)

	// Load from ~/.kalenji_readerrc
	char *homeDir = getenv("HOME");
	if(!homeDir)
	{
		std::cerr << "No home dir found ! This is strange ... Why would $HOME not be set for your user ?" << std::endl;
		return false;
	}

	std::string rcfile = std::string(homeDir) + "/.kalenji_readerrc";
	if(access(rcfile.c_str(), R_OK) == 0)
	{
		std::string line;
		std::ifstream conf_file(rcfile.c_str());
		if (conf_file.is_open())
		{
			while ( conf_file.good() )
			{
				getline(conf_file, line);
				size_t cut_place = line.find("=");
				if(cut_place != std::string::npos)
				{
					std::string key = line.substr(0, cut_place);
					std::string value = line.substr(cut_place+1);
					configuration[key] = value;
				}
			}
			conf_file.close();
		}
		else 
		{
			std::cerr << "Unable to open " << rcfile; 
			return false;
		}
	}
    if(configuration.count("log_transactions_directory") == 0)
    {
        configuration["log_transactions_directory"] = configuration["directory"] + "/logs";
    }
    if(useInputStream)
    {
	    // When using an input stream, we don't want the user to be prompted as we read everything and ignore all sending
	    configuration["import"] = "all";
    }
    initLogFile();
    return true;
}

void filterSessionsToImport(SessionsMap *sessions)
{
	std::string to_import_string;
	if(configuration["import"] == "ask")
	{
		// Display sessions that can be imported, prompt for list of sessions to import
		std::cout << "Sessions available for import:" << std::endl;
		for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); ++it)
		{
			// TODO: Use a SessionInfo method instead !
			std::cout << std::setw(5) << it->second.getNum() << " - " << it->second.getBeginTime(true); 
			std::cout << " " << std::setw(5) << it->second.getNbLaps() << " laps ";
			std::cout << std::setw(10) << (double)it->second.getDistance() / 1000 << " km ";
			std::cout << std::setw(15) << durationAsString(it->second.getDuration()) << std::endl;
		}
		std::cout << "List of sessions to import (space separated - 'all' to import everything): " << std::endl;

/*
		// It's a pity, but this doesn't work (only stops after a wrong entry)
		std::copy(std::istream_iterator<uint32_t>(std::cin), std::istream_iterator<uint32_t>(), std::back_inserter(to_import));
*/
		getline(std::cin, to_import_string);
	}
	else
	{
		to_import_string = configuration["import"];
	}

	if(to_import_string != "all")
	{
		std::vector<uint32_t> to_import;
		std::stringstream iss(to_import_string);
		std::copy(std::istream_iterator<uint32_t>(iss), std::istream_iterator<uint32_t>(), std::back_inserter(to_import));

		// TODO: Check for error in user entry. Re-ask if there is one !

		// Remove sessions that are not in the list of selected sessions
		for(SessionsMap::iterator it = sessions->begin(); it != sessions->end(); )
		{
			bool keep = false;
			for(std::vector<uint32_t>::iterator it2 = to_import.begin(); it2 != to_import.end(); ++it2)
			{
				if(it->second.getNum() == *it2)
				{
					keep = true;
				}
			}
			if(!keep) sessions->erase(it++);
			else ++it;
		}
	}
}

int main(int argc, char *argv[])
{
	// TODO: Handle command line options
	if(!readConf(argc, argv)) return -1;

	// First attempt, creating dir if it doesn't exist
	if(!checkAndCreateDir(configuration["directory"])) return -1;
	if(configuration["log_transactions"] == "yes" && !checkAndCreateDir(configuration["log_transactions_directory"])) return -1;

	libusb_device_handle *USBDevice;
	if(!openUSBDevice(&USBDevice) && !useInputStream)
	{
		std::cerr << "Device not found or error opening USB device. Is your watch correctly plugged ?" << std::endl;
		return -2;
	}

	getDeviceInfo(USBDevice);
	SessionsMap sessions;
	// Retrieve the list of sessions
	getList(USBDevice, &sessions);

	// If import = ask, prompt the user for sessions to import. 
	// TODO: could filter already imported sessions (for example import = new)
	// TODO: also prompt here for trigger type (and other info not found in the watch ?). This means at session level instead of global but could also be at lap level !
	filterSessionsToImport(&sessions);

	getSessionsDetails(USBDevice, &sessions);
	// TODO: in case of error we should finish reading until getting 8Axxx so that the device is in a clean state

	for(SessionsMap::iterator it = sessions.begin(); it != sessions.end(); ++it)
	{
		createGPX(&(it->second));
	}

	return 0;
}
