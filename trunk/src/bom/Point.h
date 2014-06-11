#ifndef _BOM_POINT_H
#define _BOM_POINT_H

#include <sstream>
#include <iostream>
#include <ctime>
#include <stdint.h>
#include "../bom/Field.h"

class Point
{
	public:
		// TODO: Improve the way fiability is handled (0 / 3 doesn't make sense)
                Point() : _time(0), _tenth(0), _lat(FieldUndef), _lon(FieldUndef), _alt(FieldUndef), _speed(FieldUndef), _bpm(FieldUndef), _fiability(3), _important(false), _distance(FieldUndef) 
		{ };

                Point(Field<double> lat, Field<double> lon, Field<int16_t> alt, Field<double> speed, time_t time, uint32_t tenth, Field<uint16_t> bpm, uint16_t fiability) 
		  : _time(time), _tenth(tenth), _lat(lat), _lon(lon), _alt(alt), _speed(speed), _bpm(bpm), _fiability(fiability), _important(false), _distance(FieldUndef)
		{ };

		void setLatitude(Field<double> lat)        { _lat = lat; };
		void setLongitude(Field<double> lon)       { _lon = lon; };
		void setAltitude(Field<int16_t> alt)       { _alt = alt; };
		void setSpeed(Field<double> speed)         { _speed = speed; };
		void setHeartRate(Field<uint16_t> bpm)     { _bpm = bpm; };
		void setFiability(uint16_t f)              { _fiability = f; };
		void setImportant(bool i)                  { _important = i; };
		void setTime(time_t time)                  { _time = time; };
		void setDistance(Field<uint32_t> distance) { _distance = distance;} 
		
		const Field<double>& getLatitude() const    { return _lat; };
		const Field<double>& getLongitude() const   { return _lon; };
		const Field<int16_t>& getAltitude() const   { return _alt; };
		const Field<double>& getSpeed() const       { return _speed; };
		const Field<uint16_t>& getHeartRate() const { return _bpm; };
		uint16_t getFiability() const               { return _fiability; };
		bool isImportant() const                    { return _important; };
		time_t getTime() const                      { return _time; };
		const Field<uint32_t> getDistance() const   { return _distance;}
		// TODO: Is it really the right place ? We may want to do it with any time ! To move in a "utils" part
		const std::string getTimeAsString(bool human_readable=false) const
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
		Field<uint32_t> _distance; //Distance in meter from the begining: if -1, must be computed by the session see Session::getDistanceForPoint(Point point);
};

#endif
