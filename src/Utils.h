#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <list>
#include <stdexcept>
#include <sstream>
#include <tr1/memory>

class SExcept: public std::ostringstream {
public:
	friend class StreamExcept;
	virtual ~SExcept() { throw std::runtime_error(str()); }

private:
	SExcept() {}
};

class StreamExcept: public std::tr1::shared_ptr<SExcept> {
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

std::string durationAsString(double sec, bool with_hundredth = false);
std::list<std::string> splitString(std::string toSplit, std::string separator = ",");

#endif
