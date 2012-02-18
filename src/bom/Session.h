#ifndef _BOM_SESSION_H
#define _BOM_SESSION_H

#include <vector>
#include <list>
#include <map>
#include <stdint.h>
#include "../bom/Lap.h"
#include "../bom/Point.h"

typedef std::vector<char> SessionId;

class Session
{
	public:
		Session() : _id(0), _num(0), _nb_points(0), _duration(0), _distance(0), _nb_laps(0)
		{
			_name = "No name";
		}

		Session(SessionId id, uint32_t num, tm time, uint32_t nb_points, double duration, uint32_t distance, uint32_t nb_laps) : 
			         _id(id), _num(num), _time(time), _nb_points(nb_points), _duration(duration), _distance(distance), _nb_laps(nb_laps) 
		{
			_name = "No name";
			_time_t = mktime(&_time);
		}

		~Session()
		{
			for(std::list<Point*>::iterator it = _points.begin(); it != _points.end(); ++it)
			{
				delete (*it);
			}
			_points.clear();
			for(std::list<Lap*>::iterator it = _laps.begin(); it != _laps.end(); ++it)
			{
				delete (*it);
			}
			_laps.clear();
		}

		void addPoint(Point* point) { _points.push_back(point); }
		void addLap(Lap *lap)       { _laps.push_back(lap); }

		void setName(std::string name)       { _name = name; };
		void setTime(tm time)                { _time = time; _time_t = mktime(&_time); };
		void setMaxSpeed(double max_speed)   { _max_speed = max_speed; };
		void setAvgSpeed(double avg_speed)   { _avg_speed = avg_speed; };
		void setAscent(uint32_t ascent)      { _ascent = ascent; };
		void setDescent(uint32_t descent)    { _descent = descent; };

		// TODO: As for time manipulation done in Point, to move in a "utils" part
		std::string getBeginTime(bool human_readable=false) 
		{ 
			char buffer[256];
			if(human_readable)
				strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &_time);
			else
				strftime(buffer, 256, "%Y-%m-%dT%H:%M:%SZ", &_time);
			return std::string(buffer);
		};

		uint32_t getNum() { return _num; };
		bool isComplete() { return _nb_points == _points.size(); };

		std::list<Lap*> &getLaps()     { return _laps; };
		std::list<Point*> &getPoints() { return _points; };

		// TODO: Check what is used, what is not, what should be added (like getLastPointTime() that would check if Point is empty) ...
		SessionId getId()      { return _id; };
		std::string getName()  { return _name; };
		int getYear()          { return _time.tm_year + 1900; };
		int getMonth()         { return _time.tm_mon + 1; };
		int getDay()           { return _time.tm_mday; };
		int getHour()          { return _time.tm_hour; };
		int getMinutes()       { return _time.tm_min; };
		int getSeconds()       { return _time.tm_sec; };
		uint32_t getAscent()   { return _ascent; };
		uint32_t getDescent()  { return _descent; };
		double getDuration()   { return _duration; };
		uint32_t getDistance() { return _distance; };
		double getMaxSpeed()   { return _max_speed; };
		double getAvgSpeed()   { return _avg_speed; };
		uint32_t getNbLaps()   { return _nb_laps; };
		time_t getTime()       { return _time_t; };

	private:
		SessionId _id;
		std::string _name;
		uint32_t _num;
		tm _time;
		time_t _time_t;
		uint32_t _nb_points;
		double _duration;
		uint32_t _ascent;
		uint32_t _descent;
		uint32_t _distance;
		double _max_speed;
		double _avg_speed;
		uint32_t _nb_laps;
		std::list<Lap*> _laps;
		std::list<Point*> _points;
};

typedef std::map<SessionId, Session> SessionsMap;
typedef std::pair<SessionId, Session> SessionsMapElement;

#endif
