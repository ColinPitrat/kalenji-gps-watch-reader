#ifndef _BOM_POINT_H
#define _BOM_POINT_H

#include <sstream>
#include <iostream>
#include <ctime>
#include "../bom/Field.h"

class Point
{
	public:
		// TODO: Improve the way fiability is handled (0 / 3 doesn't make sense)
		Point() : _lat(FieldUndef), _lon(FieldUndef), _alt(FieldUndef), _speed(FieldUndef), _time(0), _tenth(0), _bpm(FieldUndef), _fiability(3), _important(false)
		{ };

		Point(Field<double> lat, Field<double> lon, Field<int16_t> alt, Field<double> speed, time_t time, uint32_t tenth, Field<uint16_t> bpm, uint16_t fiability) 
			 : _lat(lat), _lon(lon), _alt(alt), _speed(speed), _time(time), _tenth(tenth), _bpm(bpm), _fiability(fiability), _important(false)
		{ };

		void setLatitude(Field<double> lat)    { _lat = lat; };
		void setLongitude(Field<double> lon)   { _lon = lon; };
		void setAltitude(Field<int16_t> alt)   { _alt = alt; };
		void setSpeed(Field<double> speed)     { _speed = speed; };
		void setHeartRate(Field<uint16_t> bpm) { _bpm = bpm; };
		void setFiability(uint16_t f)          { _fiability = f; };
		void setImportant(bool i)              { _important = i; };
		void setTime(time_t time)              { _time = time; };

		Field<double> getLatitude()    { return _lat; };
		Field<double> getLongitude()   { return _lon; };
		Field<int16_t> getAltitude()   { return _alt; };
		Field<double> getSpeed()       { return _speed; };
		Field<uint16_t> getHeartRate() { return _bpm; };
		uint16_t getFiability()        { return _fiability; };
		bool isImportant()             { return _important; };
		time_t getTime()               { return _time; };
		// TODO: Is it really the right place ? We may want to do it with any time ! To move in a "utils" part
		std::string getTimeAsString(bool human_readable=false)
		{  
			char buffer[256];
			tm *time_tm = localtime(&_time);
			std::stringstream format_string;
			if(human_readable)
				format_string << "%Y-%m-%d %H:%M:%S." << _tenth;
			else
				format_string << "%Y-%m-%dT%H:%M:%S." << _tenth << "Z";
			strftime(buffer, 256, format_string.str().c_str(), time_tm);
			return std::string(buffer);
		};

	private:
		time_t   _time;
		uint32_t _tenth;
		Field<double>   _lat;
		Field<double>   _lon;
		Field<int16_t>  _alt;
		Field<double>   _speed;
		Field<uint16_t> _bpm;
		uint16_t _fiability;
		bool _important;
};

#endif
