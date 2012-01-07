#ifndef _BOM_LAP_H
#define _BOM_LAP_H

#include "Point.h"

class Lap
{
	public:
		Lap(uint32_t firstPointId, uint32_t lastPointId, 
		    double duration, uint32_t length, 
		    double max_speed, double avg_speed, 
		    uint32_t calories, uint32_t grams, 
		    uint32_t descent, uint32_t ascent) : _firstPointId(firstPointId), _lastPointId(lastPointId), 
		                                         _start(NULL), _end(NULL), 
							 _duration(duration), _length(length), 
							 _max_speed(max_speed), _avg_speed(avg_speed), 
							 _calories(calories), _grams(grams), 
							 _descent(descent), _ascent(ascent)
		{ };

		void setStartPoint(Point *start) { _start = start; };
		void setEndPoint(Point *end)     { _end = end; };

		uint32_t getFirstPointId() { return _firstPointId; };
		uint32_t getLastPointId()  { return _lastPointId; };
		Point *getStartPoint()     { return _start; };
		Point *getEndPoint()       { return _end; };
		double getDuration()       { return _duration; };
		uint32_t getCalories()     { return _calories; };
		uint32_t getLength()       { return _length; };

	private:
		uint32_t _firstPointId;
		uint32_t _lastPointId;
		Point *_start;
		Point *_end;
		double _duration;
		uint32_t _length;
		double _max_speed;
		double _avg_speed;
		uint32_t _calories;
		uint32_t _grams;
		uint32_t _descent;
		uint32_t _ascent;
};

#endif
