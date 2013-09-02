#include "KML.h"
#include "../Utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(KML);

	void KML::dumpContent(std::ostream& mystream, Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);

		// TODO: Whole KML support !
		mystream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		mystream << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" << std::endl;
		mystream << "<Document>" << std::endl;
		mystream << "<name>" << session->getName() << "</name>" << std::endl;
		mystream << "<open>1</open>" << std::endl;

		mystream << "<Style id=\"kalenji_lap\">" << std::endl;
		mystream << "<IconStyle>" << std::endl;
		mystream << "<color>ff00ffff</color>" << std::endl;
		mystream << "<scale>0.7</scale>" << std::endl;
		mystream << "<Icon>" << std::endl;
		mystream << "<href>http://maps.google.com/mapfiles/kml/pal4/icon28.png</href>" << std::endl;
		mystream << "</Icon>" << std::endl;
		mystream << "</IconStyle>" << std::endl;
		mystream << "</Style>" << std::endl;

		mystream << "<Style id=\"kalenji_runner\">" << std::endl;
		mystream << "<IconStyle>" << std::endl;
		mystream << "<color>ff0000ff</color>" << std::endl;
		mystream << "<scale>1.0</scale>" << std::endl;
		mystream << "<Icon>" << std::endl;
		mystream << "<href>http://maps.google.com/mapfiles/kml/pal2/icon57.png</href>" << std::endl;
		mystream << "</Icon>" << std::endl;
		mystream << "</IconStyle>" << std::endl;
		mystream << "</Style>" << std::endl;

		mystream << "<Style id=\"kalenji_trajet\">" << std::endl;
		mystream << "<IconStyle>" << std::endl;
		mystream << "<scale>1.1</scale>" << std::endl;
		mystream << "<Icon>" << std::endl;
		mystream << "<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>" << std::endl;
		mystream << "</Icon>" << std::endl;
		mystream << "<hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
		mystream << "</IconStyle>" << std::endl;
		mystream << "<LineStyle>" << std::endl;
		mystream << "<color>ff00ffff</color>" << std::endl;
		mystream << "<width>3</width>" << std::endl;
		mystream << "</LineStyle>" << std::endl;
		mystream << "</Style>" << std::endl;

		std::list<Lap*> laps = session->getLaps();
		uint32_t i = 0;
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			++i;
			mystream << "<Placemark>" << std::endl;
			mystream << "<name>Lap " << i << "</name>" << std::endl;
			mystream << "<styleUrl>kalenji_lap</styleUrl>" << std::endl;
			mystream << "<description>" << std::endl;
			mystream << "<b>Distance:</b> " << (*it)->getDistance()/1000.0 << " km<br/>";
			mystream << "<b>Time:</b> " << durationAsString((*it)->getDuration()) << "<br/>";
			mystream << (*it)->getAvgSpeed().toStream("<b>Average speed:</b> ", " km/h<br/>");
			mystream << (*it)->getMaxSpeed().toStream("<b>Maximum speed:</b> ", " km/h<br/>");
			mystream << (*it)->getAvgHeartrate().toStream("<b>Average heartrate:</b> ", " bpm<br/>");
			mystream << (*it)->getMaxHeartrate().toStream("<b>Maximum heartrate:</b> ", " bpm<br/>");
			mystream << "</description>" << std::endl;
			mystream << "<Point>" << std::endl;
			mystream << "<coordinates>" << (*it)->getEndPoint()->getLongitude() << "," << (*it)->getEndPoint()->getLatitude() << "," << (*it)->getEndPoint()->getAltitude() << "</coordinates>" << std::endl;
			mystream << "</Point>" << std::endl;
			mystream << "</Placemark>" << std::endl;
		}

		mystream << "<Placemark>" << std::endl;
		mystream << "<name>Trajet</name>" << std::endl;
		mystream << "<styleUrl>#kalenji_trajet</styleUrl>" << std::endl;
		mystream << "<LineString>" << std::endl;
		mystream << "<tessellate>1</tessellate>" << std::endl;
		mystream << "<coordinates>" << std::endl;
		std::list<Point*> points = session->getPoints();
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			mystream << (*it)->getLongitude() << "," << (*it)->getLatitude() << "," << (*it)->getAltitude() << " ";
		}
		mystream << "</coordinates>" << std::endl;
		mystream << "</LineString>" << std::endl;
		mystream << "</Placemark>" << std::endl;

		mystream << "<Placemark>" << std::endl;
		mystream << "<name>Runner</name>" << std::endl;
		mystream << "<styleUrl>kalenji_runner</styleUrl>" << std::endl;
		mystream << "<Point id=\"runner\">" << std::endl;
		std::list<Point*>::iterator it = points.begin();
		mystream << "<coordinates>" << (*it)->getLongitude() << "," << (*it)->getLatitude() << "," << (*it)->getAltitude() << "</coordinates>" << std::endl;
		mystream << "</Point>" << std::endl;
		mystream << "</Placemark>" << std::endl << std::endl;

		mystream << "<gx:Tour>" << std::endl;
		mystream << "<name>Animation</name>" << std::endl;
		mystream << "<gx:Playlist>" << std::endl;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			double duration = 0.1;
			mystream << "<gx:AnimatedUpdate>" << std::endl;
			mystream << "<gx:duration>" << duration << "</gx:duration>" << std::endl;
			mystream << "<Update>" << std::endl;
			mystream << "<targetHref></targetHref>" << std::endl;
			mystream << "<Change>" << std::endl;
			mystream << "<Point targetId=\"runner\"> " << std::endl;
			mystream << "<coordinates>" << (*it)->getLongitude() << "," << (*it)->getLatitude() << "," << (*it)->getAltitude() << "</coordinates> " << std::endl;
			mystream << "</Point>" << std::endl;
			mystream << "</Change> " << std::endl;
			mystream << "</Update>" << std::endl;
			mystream << "<gx:delayedStart>0</gx:delayedStart>" << std::endl;
			mystream << "</gx:AnimatedUpdate>" << std::endl << std::endl;

			mystream << "<gx:Wait>" << std::endl;
			mystream << "<gx:duration>" << duration << "</gx:duration>" << std::endl;
			mystream << "</gx:Wait>" << std::endl;
		}
		mystream << "</gx:Playlist>" << std::endl;
		mystream << "</gx:Tour>" << std::endl;

		mystream << "</Document>" << std::endl;
		mystream << "</kml>" << std::endl;
	}
}
