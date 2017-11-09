#include "KML.h"
#include "../Utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(KML);

	void KML::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		out.precision(8);

		// TODO: Whole KML support !
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		out << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" << std::endl;
		out << "<Document>" << std::endl;
		out << "<name>" << session->getName() << "</name>" << std::endl;
		out << "<open>1</open>" << std::endl;

		out << "<Style id=\"kalenji_lap\">" << std::endl;
		out << "<IconStyle>" << std::endl;
		out << "<color>ff00ffff</color>" << std::endl;
		out << "<scale>0.7</scale>" << std::endl;
		out << "<Icon>" << std::endl;
		out << "<href>http://maps.google.com/mapfiles/kml/pal4/icon28.png</href>" << std::endl;
		out << "</Icon>" << std::endl;
		out << "</IconStyle>" << std::endl;
		out << "</Style>" << std::endl;

		out << "<Style id=\"kalenji_runner\">" << std::endl;
		out << "<IconStyle>" << std::endl;
		out << "<color>ff0000ff</color>" << std::endl;
		out << "<scale>1.0</scale>" << std::endl;
		out << "<Icon>" << std::endl;
		out << "<href>http://maps.google.com/mapfiles/kml/pal2/icon57.png</href>" << std::endl;
		out << "</Icon>" << std::endl;
		out << "</IconStyle>" << std::endl;
		out << "</Style>" << std::endl;

		out << "<Style id=\"kalenji_trajet\">" << std::endl;
		out << "<IconStyle>" << std::endl;
		out << "<scale>1.1</scale>" << std::endl;
		out << "<Icon>" << std::endl;
		out << "<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>" << std::endl;
		out << "</Icon>" << std::endl;
		out << "<hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
		out << "</IconStyle>" << std::endl;
		out << "<LineStyle>" << std::endl;
		out << "<color>ff00ffff</color>" << std::endl;
		out << "<width>3</width>" << std::endl;
		out << "</LineStyle>" << std::endl;
		out << "</Style>" << std::endl;

		std::list<Lap*> laps = session->getLaps();
		uint32_t i = 0;
		for(const auto& lap : laps)
		{
			++i;
			if(lap->getEndPoint() != nullptr)
			{
				out << "<Placemark>" << std::endl;
				out << "<name>Lap " << i << "</name>" << std::endl;
				out << "<styleUrl>kalenji_lap</styleUrl>" << std::endl;
				out << "<description>" << std::endl;
				out << "<b>Distance:</b> " << lap->getDistance()/1000.0 << " km<br/>";
				out << "<b>Time:</b> " << durationAsString(lap->getDuration()) << "<br/>";
				out << lap->getAvgSpeed().toStream("<b>Average speed:</b> ", " km/h<br/>");
				out << lap->getMaxSpeed().toStream("<b>Maximum speed:</b> ", " km/h<br/>");
				out << lap->getAvgHeartrate().toStream("<b>Average heartrate:</b> ", " bpm<br/>");
				out << lap->getMaxHeartrate().toStream("<b>Maximum heartrate:</b> ", " bpm<br/>");
				out << "</description>" << std::endl;
				out << "<Point>" << std::endl;
				out << "<coordinates>" << lap->getEndPoint()->getLongitude() << "," << lap->getEndPoint()->getLatitude() << "," << lap->getEndPoint()->getAltitude() << "</coordinates>" << std::endl;
				out << "</Point>" << std::endl;
				out << "</Placemark>" << std::endl;
			}
		}

		out << "<Placemark>" << std::endl;
		out << "<name>Trajet</name>" << std::endl;
		out << "<styleUrl>#kalenji_trajet</styleUrl>" << std::endl;
		out << "<LineString>" << std::endl;
		out << "<tessellate>1</tessellate>" << std::endl;
		out << "<coordinates>" << std::endl;
		std::list<Point*> points = session->getPoints();
		for(const auto& point : points)
		{
			out << point->getLongitude() << "," << point->getLatitude() << "," << point->getAltitude() << " ";
		}
		out << "</coordinates>" << std::endl;
		out << "</LineString>" << std::endl;
		out << "</Placemark>" << std::endl;

		out << "<Placemark>" << std::endl;
		out << "<name>Runner</name>" << std::endl;
		out << "<styleUrl>kalenji_runner</styleUrl>" << std::endl;
		out << "<Point id=\"runner\">" << std::endl;
		auto it = points.begin();
		out << "<coordinates>" << (*it)->getLongitude() << "," << (*it)->getLatitude() << "," << (*it)->getAltitude() << "</coordinates>" << std::endl;
		out << "</Point>" << std::endl;
		out << "</Placemark>" << std::endl << std::endl;

		out << "<gx:Tour>" << std::endl;
		out << "<name>Animation</name>" << std::endl;
		out << "<gx:Playlist>" << std::endl;
		for(const auto& point : points)
		{
			double duration = 0.1;
			out << "<gx:AnimatedUpdate>" << std::endl;
			out << "<gx:duration>" << duration << "</gx:duration>" << std::endl;
			out << "<Update>" << std::endl;
			out << "<targetHref></targetHref>" << std::endl;
			out << "<Change>" << std::endl;
			out << "<Point targetId=\"runner\"> " << std::endl;
			out << "<coordinates>" << point->getLongitude() << "," << point->getLatitude() << "," << point->getAltitude() << "</coordinates> " << std::endl;
			out << "</Point>" << std::endl;
			out << "</Change> " << std::endl;
			out << "</Update>" << std::endl;
			out << "<gx:delayedStart>0</gx:delayedStart>" << std::endl;
			out << "</gx:AnimatedUpdate>" << std::endl << std::endl;

			out << "<gx:Wait>" << std::endl;
			out << "<gx:duration>" << duration << "</gx:duration>" << std::endl;
			out << "</gx:Wait>" << std::endl;
		}
		out << "</gx:Playlist>" << std::endl;
		out << "</gx:Tour>" << std::endl;

		out << "</Document>" << std::endl;
		out << "</kml>" << std::endl;
	}
}
