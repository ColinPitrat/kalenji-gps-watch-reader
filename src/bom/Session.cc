#include "Session.h"
#include "../Utils.h"
#include <iostream>

void Session::getSummary(std::ostream& os) const
{
	os << std::setw(5) << this->getNum() << " - " << this->getBeginTime(true);
	os << " " << std::setw(5) << this->getNbLaps() << " laps ";
	os << std::setw(10) << (double)this->getDistance() / 1000 << " km ";
	os << std::setw(15) << durationAsString(this->getDuration());
}

std::ostream& operator<<(std::ostream& os, const Session& session) {
	session.getSummary(os);
	return os;
}
