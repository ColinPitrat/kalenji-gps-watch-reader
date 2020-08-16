#include "Kalenji.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "../device/Kalenji.h"
#include "../source/USB.h"

namespace output
{
	REGISTER_OUTPUT(Kalenji);

	void Kalenji::dump(const Session *session, std::map<std::string, std::string> &configuration)
	{
		source::Source *dataSource = new source::USB();
		device::Device *dev = new device::Kalenji();
		dev->setSource(dataSource);
		dev->init(dev->getDeviceId());
		dev->exportSession(session);
		delete dev;
		delete dataSource;
	}
	/*
	This is the format used by gh600 - as download doesn't work I don't think this one will either !
	{
		// TODO: Send the content to USB instead of a file
		std::stringstream filename;
		filename << configuration["directory"] << "/";
		filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_";
		filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".data";
		std::cout << "Creating " << filename.str() << std::endl;

		std::ofstream mystream(filename.str().c_str());
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		// TODO: truncate by 136 points ?
		mystream << std::hex;
		// TODO: dataLength
		mystream << "02";
		std::vector<Point*> points = session->getPoints();
		int nbPoints = points.size();
		mystream << std::setw(4) << 27 + (15 * nbPoints);
		mystream << "91"; // 90 for the first one ?!
		mystream << session->getYear() - 2000 << session->getMonth() << session->getDay();
		mystream << session->getHour() << session->getMinutes() << session->getSeconds();
		mystream << std::setw(8) << session->getDuration();
		mystream << std::setw(8) << session->getDistance();
		mystream << std::setw(4) << 0; // session->getCalories();
		mystream << std::setw(4) << session->getMaxSpeed();
		mystream << std::setw(8) << nbPoints;
		mystream << std::setw(4) << 0; //firstPoint;
		mystream << std::setw(4) << 0; //lastPoint;
		for(std::vector<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			// TODO: track points
			mystream << std::setw(8) << (*it)->getLatitude() * 1000000;
			mystream << std::setw(8) << (*it)->getLongitude() * 1000000;
			mystream << std::setw(4) << (*it)->getAltitude();
			mystream << std::setw(4) << (*it)->getSpeed() * 100;
			mystream << std::setw(2) << (*it)->getHeartRate();
			int interval = 10;
			mystream << std::setw(4) << interval;
		}
		// 02%(payload)s%(isFirst)s%(trackInfo)s%(from)s%(to)s%(trackpoints)s%(checksum)s
		mystream << std::setw(2) << 0; //checksum;

	}
	*/
}
