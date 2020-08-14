#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <list>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <cstdint>

#ifdef WINDOWS
#include <ctime>
#include <cstring>

void localtime_r (const time_t *timer, struct tm *result);
void gmtime_r (const time_t *timer, struct tm *result);
#endif

time_t mktime_utc(struct tm *tm);

class Point;

#define THROW_STREAM(stream) throw std::runtime_error(Formatter() << stream);  // NOLINT: parenthesis around 'stream' would prevent using << in THROW_STREAM

class Formatter
{
    public:
        Formatter() = default;
        ~Formatter() = default;
        Formatter(const Formatter &) = delete;
        Formatter & operator= (Formatter &) = delete;

        template <typename Type>
        Formatter & operator<< (const Type & value)
        {
            _stream << value;
            return *this;
        }

        std::string str() const;
        operator std::string() const;

    private:
        std::stringstream _stream;
};

int testDir(const std::string& path, bool create_if_not_exist);

void trimString(std::string &toTrim);
std::string durationAsString(double sec, bool with_millis = false);
std::list<std::string> splitString(std::string toSplit, std::string separator = ",");

uint32_t str_to_int(const std::string& intAsString);

double distanceEarth(double lat1, double lon1, double lat2, double lon2);
double distanceEarth(const Point& p1, const Point& p2);

#endif
