#ifndef _BOM_SESSION_H
#define _BOM_SESSION_H

#include <vector>
#include <list>
#include <map>
#include <cstdint>
#include <ctime>
#include <cstring>
#include "../bom/Lap.h"
#include "../bom/Point.h"
#include "../Utils.h"

typedef std::vector<char> SessionId;

class Session
{
	public:
		Session() : _id(0), _name("No name"), _num(0), _nb_points(0),
		            _duration(0), _distance(0), _max_speed(FieldUndef), _avg_speed(FieldUndef),
			    _max_hr(FieldUndef), _avg_hr(FieldUndef), _calories(FieldUndef), _grams(FieldUndef),
			    _ascent(FieldUndef), _descent(FieldUndef), _nb_laps(0)
		{ }

		Session(SessionId id, uint32_t num, tm time, uint32_t nb_points, double duration, uint32_t distance, uint32_t nb_laps) :
			         _id(std::move(id)), _name("No name"), _num(num), _local_time(time), _nb_points(nb_points),
				 _duration(duration), _distance(distance), _max_speed(FieldUndef), _avg_speed(FieldUndef),
				 _max_hr(FieldUndef), _avg_hr(FieldUndef), _calories(FieldUndef), _grams(FieldUndef),
				 _ascent(FieldUndef), _descent(FieldUndef), _nb_laps(nb_laps)
		{
			convertToGMT();
		}

		~Session()
		{
			for(auto it = _points.begin(); it != _points.end(); ++it)
			{
				delete (*it);
			}
			_points.clear();
			for(auto it = _laps.begin(); it != _laps.end(); ++it)
			{
				delete (*it);
			}
			_laps.clear();
		}

		void addPoint(Point* point) { _points.push_back(point); }
		void addLap(Lap *lap)       { _laps.push_back(lap); }

		void convertToGMT()
		{
			_time_t = mktime(&_local_time);
			gmtime_r(&_time_t, &_time);
		}

		void timesFromTimeT()
		{
			localtime_r(&_time_t, &_local_time);
			gmtime_r(&_time_t, &_time);
		}

		void setId(SessionId id)                   { _id = id; };
		void setNum(uint32_t num)                  { _num = num; };
		void setName(const std::string& name)      { _name = name; };
		// TODO(cpitrat): Convert to a single setter/source of truth for time
		void setTime(tm time)                      { _local_time = time; convertToGMT(); };
		void setTimeT(time_t time)                 { _time_t = time; timesFromTimeT(); };
		void setNbPoints(uint32_t nbPoints)        { _nb_points = nbPoints; };
		void setNbLaps(uint32_t nbLaps)            { _nb_laps = nbLaps; };

		void setDuration(double duration)                 { _duration = duration; };
		void setDistance(uint32_t distance)               { _distance = distance; };
		void setMaxSpeed(const Field<double>& max_speed)  { _max_speed = max_speed; };
		void setAvgSpeed(const Field<double>& avg_speed)  { _avg_speed = avg_speed; };
		void setMaxHr(const Field<uint32_t>& max_hr)      { _max_hr = max_hr; };
		void setAvgHr(const Field<uint32_t>& avg_hr)      { _avg_hr = avg_hr; };
		void setCalories(const Field<uint32_t>& calories) { _calories = calories; };
		void setGrams(const Field<uint32_t>& grams)       { _grams = grams; };
		void setAscent(const Field<uint32_t>& ascent)     { _ascent = ascent; };
		void setDescent(const Field<uint32_t>& descent)   { _descent = descent; };

		// TODO: As for time manipulation done in Point, to move in a "utils" part
		const std::string getBeginTime(bool human_readable=false) const
		{
			char buffer[256];
			if(human_readable)
				strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", &_time);
			else
				strftime(buffer, 256, "%Y-%m-%dT%H:%M:%SZ", &_time);
			return std::string(buffer);
		};

		uint32_t getNum() const            { return _num; };
		bool isComplete()                  { return _nb_points == _points.size(); };

		std::vector<Lap*> &getLaps()                 { return _laps; };
		const std::vector<Lap*> &getLaps() const     { return _laps; };
		std::vector<Point*> &getPoints()             { return _points; };
		const std::vector<Point*> &getPoints() const { return _points; };

		// TODO: Check what is used, what is not, what should be added (like getLastPointTime() that would check if Point is empty) ...
		const SessionId getId() const                  { return _id; };
		const std::string getName() const              { return _name; };
		void getSummary(std::ostream& os) const;
		int getYear() const                            { return _local_time.tm_year + 1900; };
		int getMonth() const                           { return _local_time.tm_mon + 1; };
		int getDay() const                             { return _local_time.tm_mday; };
		int getHour() const                            { return _local_time.tm_hour; };
		int getMinutes() const                         { return _local_time.tm_min; };
		int getSeconds() const                         { return _local_time.tm_sec; };

		double getDuration() const                     { return _duration; };
		uint32_t getDistance() const                   { return _distance; };
		const Field<double>& getMaxSpeed() const       { return _max_speed; };
		const Field<double>& getAvgSpeed() const       { return _avg_speed; };
		const Field<uint32_t>& getMaxHeartrate() const { return _max_hr; };
		const Field<uint32_t>& getAvgHeartrate() const { return _avg_hr; };
		const Field<uint32_t>& getCalories() const     { return _calories; };
		const Field<uint32_t>& getGrams() const        { return _grams; };
		const Field<uint32_t>& getAscent() const       { return _ascent; };
		const Field<uint32_t>& getDescent() const      { return _descent; };

		uint32_t getNbLaps() const                     { return _nb_laps; };
		time_t getTime() const                         { return _time_t; };

		/* If watches doesn't set the distance for each point, we must try to compute them.
		 * TODO: check against laps if computation is good enougth
		 */
		void ensurePointDistanceAreOk()
		{
			Point* previousPoint = nullptr;
			double totalDistance = 0; // counter to avoid precision lost (because we round to meter in point::distance;
			for(auto it = _points.begin(); it != _points.end(); ++it)
			{
				Point* point = (*it);
				if(!point->getDistance().isDefined())
				{
					if(previousPoint == nullptr)
					{
						point->setDistance(0); // First point: distance equal to zero
					}
					else
					{
						double distanceInMeter = distanceEarth(previousPoint->getLatitude(), previousPoint->getLongitude(), point->getLatitude(), point->getLongitude());
						totalDistance = totalDistance + distanceInMeter;
						point->setDistance(totalDistance);
					}
				}
				previousPoint = point;
			}
		}

	private:
		SessionId _id;
		std::string _name;
		uint32_t _num;
		tm _time;
		tm _local_time;
		time_t _time_t;
		uint32_t _nb_points;

		double _duration;
		uint32_t _distance;
		Field<double> _max_speed;
		Field<double> _avg_speed;
		Field<uint32_t> _max_hr;
		Field<uint32_t> _avg_hr;
		Field<uint32_t> _calories;
		Field<uint32_t> _grams;
		Field<uint32_t> _ascent;
		Field<uint32_t> _descent;

		uint32_t _nb_laps;
		std::vector<Lap*> _laps;
		std::vector<Point*> _points;
};

std::ostream& operator<<(std::ostream& os, const Session& session);

typedef std::map<SessionId, Session> SessionsMap;
typedef std::pair<SessionId, Session> SessionsMapElement;

#endif
