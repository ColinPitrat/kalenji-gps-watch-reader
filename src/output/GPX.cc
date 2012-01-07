#include "GPX.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(GPX);

	void GPX::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::stringstream filename;
		filename << configuration["directory"] << "/"; 
		filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_"; 
		filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".gpx";
		std::cout << "Creating " << filename.str() << std::endl;
		std::ofstream mystream(filename.str().c_str());
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "<?xml version=\"1.0\"?>" << std::endl;
		mystream << "<gpx version=\"1.1\"" << std::endl;
		mystream << "     creator=\"Kalenji 300 Reader\"" << std::endl;
		mystream << "     xmlns=\"http://www.topografix.com/GPX/1/1\"" << std::endl;
		mystream << "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << std::endl;
		mystream << "     xmlns:gpxdata=\"http://www.cluetrust.com/XML/GPXDATA/1/0\"" << std::endl;
		mystream << "     xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1" << std::endl;
		mystream << "                          http://www.topografix.com/GPX/1/1/gpx.xsd\">" << std::endl;

		// TODO: Improve metadata (add link among others)
		mystream << "  <metadata>" << std::endl;
		mystream << "    <name>" << session->getName() << "</name>" << std::endl;
		mystream << "    <time>" << session->getBeginTime() << "</time>" << std::endl;
		mystream << "  </metadata>" << std::endl;

		mystream << "  <trk>" << std::endl;
		mystream << "    <trkseg>" << std::endl;
		std::list<Point*> points = session->getPoints();
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			// TODO: More info on point ? (cadence, hr when available)
			mystream << "      <trkpt lat=\"" << (*it)->getLatitude() << "\" lon=\"" << (*it)->getLongitude() << "\">" << std::endl;
			mystream << "        <ele>" << (*it)->getAltitude() << "</ele>" << std::endl;
			mystream << "        <time>" << (*it)->getTimeAsString() << "</time>" << std::endl;
			mystream << "        <extensions>" << std::endl;
			mystream << "          <gpxdata:hr>" << (*it)->getHeartRate() << "</gpxdata:hr>" << std::endl;
			mystream << "        </extensions>" << std::endl;
			mystream << "      </trkpt>" << std::endl;
		}
		mystream << "    </trkseg>" << std::endl;
		mystream << "  </trk>" << std::endl;
		mystream << "  <extensions>" << std::endl;
		int lap = 0;
		std::list<Lap*> laps = session->getLaps();
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			// TODO: Remove this check (could be moved to the getters) ?
			if((*it)->getStartPoint() == NULL || (*it)->getEndPoint() == NULL)
			{
				std::cerr << "Oups ! I've got a lap without ";
				if((*it)->getStartPoint() == NULL)
					std::cerr << "start";
				else
					std::cerr << "end";
				std::cerr << " point: (" << (*it)->getFirstPointId() << " - " << (*it)->getLastPointId() << "). This shouldn't happen ! Report a bug ..." << std::endl;
			}
			else
			{
				lap++;
				mystream << "    <gpxdata:lap>" << std::endl;
				mystream << "      <gpxdata:index>" << lap << "</gpxdata:index>" << std::endl;
				mystream << "      <gpxdata:startPoint lat=\"" << (*it)->getStartPoint()->getLatitude() << "\" lon=\"" << (*it)->getStartPoint()->getLongitude() << "\"/>" << std::endl;
				mystream << "      <gpxdata:endPoint lat=\"" << (*it)->getEndPoint()->getLatitude() << "\" lon=\"" << (*it)->getEndPoint()->getLongitude() << "\" />" << std::endl;
				mystream << "      <gpxdata:startTime>" << (*it)->getStartPoint()->getTimeAsString() << "</gpxdata:startTime>" << std::endl;
				mystream << "      <gpxdata:elapsedTime>" << (*it)->getDuration() << "</gpxdata:elapsedTime>" << std::endl;
				mystream << "      <gpxdata:calories>" << (*it)->getCalories() << "</gpxdata:calories>" << std::endl;
				mystream << "      <gpxdata:distance>" << (*it)->getLength() << "</gpxdata:distance>" << std::endl;
				// Heart rate is not available on my device but would be nice to add for CW 700:
				mystream << "      <gpxdata:summary name=\"AverageHeartRateBpm\" kind=\"avg\">0</gpxdata:summary>" << std::endl;
				// I didn't find a way to differentiate manual lap taking versus automatic (triggered by time or distance)
				// This is the correct syntax, but pytrainer doesn't support it
				//mystream << "      <gpxdata:trigger kind=\"" << configuration["trigger"] << "\" />" << std::endl;
				mystream << "      <gpxdata:trigger>" << configuration["trigger"] << "</gpxdata:trigger>" << std::endl;
				// What can I tell about this ?! Mandatory when using gpxdata (as the two previous one) so I put it with a default value ...
				mystream << "      <gpxdata:intensity>active</gpxdata:intensity>" << std::endl;
				mystream << "    </gpxdata:lap>" << std::endl;
			}
		}
		mystream << "  </extensions>" << std::endl;
		mystream << "</gpx>" << std::endl;
		mystream.close();
	}
}
