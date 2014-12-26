#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <list>
#include <stdexcept>
#include <sstream>
#include <tr1/memory>

#ifdef WINDOWS
#include <ctime>
#include <cstring>

struct tm * localtime_r (const time_t *timer, struct tm *result);
struct tm * gmtime_r (const time_t *timer, struct tm *result);
#endif

class Point;

class SExcept: public std::ostringstream 
{
	public:
		friend class StreamExcept;
		virtual ~SExcept() { throw std::runtime_error(str()); }

	private:
		SExcept() {}
};

class StreamExcept: public std::tr1::shared_ptr<SExcept> 
{
	public:
		static StreamExcept sthrow() { return StreamExcept(new SExcept()); }

	private:
		StreamExcept(SExcept *p):std::tr1::shared_ptr<SExcept>(p) {}
};

// If we use a reference as first argument, the following statement:
// sthrow() << xxxx ...
// will not be valid.
// Anyway, thanks to the smart pointer the introduced overhead is low.
template<typename T> StreamExcept
operator<< (StreamExcept e, const T& x)
{
	*e << x;
	return e;
}

void trimString(std::string &toTrim);
std::string durationAsString(double sec, bool with_millis = false);
std::list<std::string> splitString(std::string toSplit, std::string separator = ",");

double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d);
double distanceEarth(const Point& p1, const Point& p2);

#endif
