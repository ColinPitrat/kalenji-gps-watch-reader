#ifndef _BOM_POINT_H
#define _BOM_POINT_H

#include <sstream>
#include <iostream>

class Point
{
	public:
		Point(double lat, double lon, int16_t alt, double speed, time_t time, uint32_t tenth, uint16_t bpm, uint16_t fiability) 
			 : _lat(lat), _lon(lon), _alt(alt), _speed(speed), _time(time), _tenth(tenth), _bpm(bpm), _fiability(fiability)
		{ };

		double getLatitude()    { return _lat; };
		double getLongitude()   { return _lon; };
		int16_t getAltitude()   { return _alt; };
		double getSpeed()       { return _speed; };
		uint16_t getHeartRate() { return _bpm; };
		uint16_t getFiability() { return _fiability; };
		time_t getTime()        { return _time; };
		// TODO: Is it really the right place ? We may want to do it with any time ! To move in a "utils" part
		std::string getTimeAsString() 
		{  
			char buffer[256];
			tm *time_tm = localtime(&_time);
			std::stringstream format_string;
			format_string << "%Y-%m-%dT%H:%M:%S." << _tenth << "Z";
			if(_tenth > 9)
			{
				std::cout << "Error: tenth bigger than 9 ! This is totally unexpected ... " << _tenth << std::endl;
			}
			strftime(buffer, 256, format_string.str().c_str(), time_tm);
			return std::string(buffer);
		};

	private:
		double   _lat;
		double   _lon;
		int16_t  _alt;
		double   _speed;
		time_t   _time;
		uint32_t _tenth;
		uint16_t _bpm;
		uint16_t _fiability;
};

#endif
