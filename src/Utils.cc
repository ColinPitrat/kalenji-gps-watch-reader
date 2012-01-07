#include "Utils.h"

#include <sstream>
#include <iomanip>
#include <stdint.h>

std::string durationAsString(double sec, bool with_hundredth)
{
	uint32_t days = sec / (24*3600);
	sec -= days * 24 * 3600; 
	uint32_t hours = sec / 3600;
	sec -= hours * 3600; 
	uint32_t minutes = sec / 60;
	sec -= minutes * 60; 
	uint32_t seconds = sec;
	sec -= seconds;
	uint32_t hundredth = sec * 100;
	std::string result;
	std::ostringstream oss;
	if(days != 0)
		oss << days << "d ";
	if(hours != 0 or days != 0)
		oss << hours << "h";
	if(minutes != 0 or hours != 0 or days !=0)
		oss << minutes << "m";
	oss << seconds << "s";
	if(with_hundredth)
		oss << std::setw(2) << std::setfill('0') << hundredth;
	return oss.str();
}
