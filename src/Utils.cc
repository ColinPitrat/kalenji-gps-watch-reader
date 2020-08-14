#include "Utils.h"
#include "bom/Point.h"

#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

#ifdef WINDOWS
struct tm * locatime_r (const time_t *timer, struct tm *result)
{
  *result = *localtime();
}

struct tm * gmtime_r (const time_t *timer, struct tm *result)
{
  *result = gmtime();
}

void setenv(const char* var, const char* value, int overwrite)
{
  std::string expr = std::string(var) + "=" + std::string(value);
  putenv(expr.c_str());
}

void unsetenv(const char* var)
{
  putenv(var);
}
#endif

time_t mktime_utc(struct tm *tm)
{
  time_t ret;
  char *tz;

  tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
  ret = mktime(tm);
  if (tz)
    setenv("TZ", tz, 1);
  else
    unsetenv("TZ");
  tzset();
  return ret;
}

std::string Formatter::str() const
{
    return _stream.str();
}

Formatter::operator std::string() const
{
    return _stream.str();
}

int testDir(const std::string& path, bool create_if_not_exist)
{
    // First verify that write access is granted
	if(access(path.c_str(), W_OK) != 0)
	{
		switch(errno)
		{
			case ENOENT:
				if(create_if_not_exist)
				{
					#ifdef WINDOWS
					mkdir(path.c_str());
					#else
					mkdir(path.c_str(), 0777);
					#endif
					return 1;
				}
				else
				{
					std::cerr << "Error: " << path << " doesn't exist and I couldn't create it" << std::endl;
					return -1;
				}
			case EACCES:
			case EROFS:
				std::cerr << "Error: Don't have write access in " << path << std::endl;
				return -1;
			case ENOTDIR:
				std::cerr << "Error: An element in " << path << " is not a directory" << std::endl;
				return -1;
			default:
				std::cerr << "Error: Unknown reason (" << errno << ":" << strerror(errno) << ") preventing write access to " << path << std::endl;
				return -1;
		}
	}

    // Then check this is a directory
    struct stat fileStat;
    if(stat(path.c_str(), &fileStat) == 0)
    {
        if(fileStat.st_mode & S_IFDIR) return 0;
        std::cerr << "Error: " << path << " is not a directory" << std::endl;
        return -1;
    }
    std::cerr << "Error: Unexpected issue (" << errno << ":" << strerror(errno) << ") when calling stat on " << path << std::endl;
    return -1;
}

void trimString(std::string &toTrim)
{
	size_t startpos = toTrim.find_first_not_of(" \t");
	if( startpos != std::string::npos )
	{
		toTrim = toTrim.substr(startpos);
	}
	size_t endpos = toTrim.find_last_not_of(" \t");
	if( endpos != std::string::npos )
	{
		toTrim = toTrim.substr(0, endpos + 1);
	}
}

std::string durationAsString(double sec, bool with_millis)
{
	uint32_t days = sec / (24*3600);
	sec -= days * 24 * 3600;
	uint32_t hours = sec / 3600;
	sec -= hours * 3600;
	uint32_t minutes = sec / 60;
	sec -= minutes * 60;
	uint32_t seconds = sec;
	sec -= seconds;
	uint32_t millis = sec * 1000;
	std::string result;
	std::ostringstream oss;
	if(days != 0)
		oss << days << "d ";
	if(hours != 0 or days != 0)
		oss << hours << "h";
	if(minutes != 0 or hours != 0 or days !=0)
		oss << minutes << "m";
	oss << seconds << "s";
	if(with_millis)
		oss << std::setw(3) << std::setfill('0') << millis;
	return oss.str();
}

std::list<std::string> splitString(std::string toSplit, std::string separator)
{
    std::list<std::string> result;
    size_t begin = 0;
    size_t end = 0;
    while(end != std::string::npos)
    {
        end = toSplit.find(separator, begin);
        std::string value = toSplit.substr(begin, end-begin);
        begin = end + separator.length();
	// none is a special value to specify empty lists
	if(value != "none")
	{
		result.push_back(value);
	}
    }
    return result;
}

static const double PI = 3.14159265358979323846;
static const double DEG_TO_RAD = PI / 180.0;
/// Earth's quatratic mean radius for WGS-84
static const double EARTH_RADIUS_IN_METERS = 6372797.560856;

/** Computes the arc, in radian, between two WGS-84 positions.
 *
 * The result is equal to <code>Distance(from,to)/EARTH_RADIUS_IN_METERS</code>
 *    <code>= 2*asin(sqrt(h(d/EARTH_RADIUS_IN_METERS )))</code>
 *
 * where:<ul>
 *    <li>d is the distance in meters between 'from' and 'to' positions.</li>
 *    <li>h is the haversine function: <code>h(x)=sin²(x/2)</code></li>
 * </ul>
 *
 * The haversine formula gives:
 *    <code>h(d/R) = h(from.lat-to.lat)+h(from.lon-to.lon)+cos(from.lat)*cos(to.lat)</code>
 *
 * http://en.wikipedia.org/wiki/Law_of_haversines
 */ 
double ArcInRadians(double lat1,double long1,double lat2,double long2)
{
	double latitudeArc  = (lat1 - lat2) * DEG_TO_RAD;
	double longitudeArc = (long1 - long2) * DEG_TO_RAD;
	double latitudeH = sin(latitudeArc * 0.5);
	latitudeH *= latitudeH;
	double lontitudeH = sin(longitudeArc * 0.5);
	lontitudeH *= lontitudeH;
	double tmp = cos(lat1*DEG_TO_RAD) * cos(lat2*DEG_TO_RAD);
	return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH));
}

uint32_t str_to_int(const std::string& intAsString)
{
	std::istringstream iss(intAsString);
	uint32_t result(0);
	iss >> result;
	return result;
}

/** Computes the distance, in meters, between two WGS-84 positions.
 *
 * The result is equal to <code>EARTH_RADIUS_IN_METERS*ArcInRadians(from,to)</code>
 */
double distanceEarth(double lat1,double lon1,double lat2,double lon2)
{
  return EARTH_RADIUS_IN_METERS*ArcInRadians(lat1, lon1, lat2, lon2);
}

double distanceEarth(const Point& p1, const Point& p2)
{
	return distanceEarth(p1.getLatitude(), p1.getLongitude(), p2.getLatitude(), p2.getLongitude());
}
