#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <list>

std::string durationAsString(double sec, bool with_hundredth = false);
std::list<std::string> splitString(std::string toSplit, std::string separator = ",");

#endif
