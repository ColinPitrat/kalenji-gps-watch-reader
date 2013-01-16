#ifndef _BOM_LAP_H
#define _BOM_LAP_H

#include "Point.h"

class Lap
{
	public:
		Lap() : _firstPointId(0),      _lastPointId(0), 
			_start(NULL),          _end(NULL), 
			_duration(0),          _distance(0), 
			_max_speed(0),         _avg_speed(0), 
			_max_hr(FieldUndef),   _avg_hr(FieldUndef), 
			_calories(FieldUndef), _grams(FieldUndef), 
			_descent(FieldUndef),  _ascent(FieldUndef)
		{ };

		Lap(uint32_t firstPointId,    uint32_t lastPointId, 
		    double duration,          uint32_t distance, 
		    double max_speed,         double avg_speed, 
		    Field<double> max_hr,     Field<double> avg_hr, 
		    Field<uint32_t> calories, Field<uint32_t> grams, 
		    Field<uint32_t> descent,  Field<uint32_t> ascent) :  _firstPointId(firstPointId), _lastPointId(lastPointId), 
									 _start(NULL), _end(NULL), 
									 _duration(duration), _distance(distance), 
									 _max_speed(max_speed), _avg_speed(avg_speed), 
									 _max_hr(max_hr), _avg_hr(avg_hr), 
									 _calories(calories), _grams(grams), 
									 _descent(descent), _ascent(ascent)
		{ };

		void setLapNum(uint32_t lapNum)         { _lapNum = lapNum; };
		void setStartPoint(Point *start)        { _start = start; };
		void setEndPoint(Point *end)            { _end = end; };

		void setDuration(double duration)       { _duration = duration; };
		void setDistance(uint32_t distance)     { _distance = distance; };
		void setMaxSpeed(uint32_t max_speed)    { _max_speed = max_speed; };
		void setAvgSpeed(uint32_t avg_speed)    { _avg_speed = avg_speed; };
		void setMaxHeartrate(uint32_t max_hr)   { _max_hr = max_hr; };
		void setAvgHeartrate(uint32_t avg_hr)   { _avg_hr = avg_hr; };
		void setCalories(uint32_t calories)     { _calories = calories; };
		void setGrams(uint32_t grams)           { _grams = grams; };
		void setAscent(uint32_t ascent)         { _ascent = ascent; };
		void setDescent(uint32_t descent)       { _descent = descent; };

		uint32_t getLapNum()              { return _lapNum; };
		uint32_t getFirstPointId()        { return _firstPointId; };
		uint32_t getLastPointId()         { return _lastPointId; };
		Point *getStartPoint()            { return _start; };
		Point *getEndPoint()              { return _end; };

		double getDuration()              { return _duration; };
		uint32_t getDistance()            { return _distance; };
		uint32_t getMaxSpeed()            { return _max_speed; };
		uint32_t getAvgSpeed()            { return _avg_speed; };
		Field<uint32_t> getMaxHeartrate() { return _max_hr; };
		Field<uint32_t> getAvgHeartrate() { return _avg_hr; };
		Field<uint32_t> getCalories()     { return _calories; };
		Field<uint32_t> getGrams()        { return _grams; };
		Field<uint32_t> getAscent()       { return _ascent; };
		Field<uint32_t> getDescent()      { return _descent; };

	private:
		uint32_t _lapNum;
		uint32_t _firstPointId;
		uint32_t _lastPointId;
		Point *_start;
		Point *_end;

		// TODO: Add begin time, as using the time of the first point can be very unprecise !
		double _duration;
		uint32_t _distance;
		double _max_speed;
		double _avg_speed;
		Field<uint32_t> _max_hr;
		Field<uint32_t> _avg_hr;
		Field<uint32_t> _calories;
		Field<uint32_t> _grams;
		Field<uint32_t> _ascent;
		Field<uint32_t> _descent;
};

#endif
