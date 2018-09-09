#include "GPX.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(GPX);

	void GPX::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		bool gpxdata_ext = configuration["gpx_extensions"].find("gpxdata") != std::string::npos;
		bool gpxtpx_ext = configuration["gpx_extensions"].find("gpxtpx") != std::string::npos;
		bool has_extension =  gpxdata_ext || gpxtpx_ext;
		// Latitude and longitude retrieved from the GPS has 6 decimals and can have 2 digits before decimal point
		out.precision(8);
		out << "<?xml version=\"1.0\"?>" << std::endl;
		out << "<gpx version=\"1.1\"" << std::endl;
		out << "     creator=\"Kalenji Reader\"" << std::endl;
		out << "     xmlns=\"http://www.topografix.com/GPX/1/1\"" << std::endl;
		out << "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << std::endl;
		if(gpxdata_ext)
		{
			out << "     xmlns:gpxdata=\"http://www.cluetrust.com/XML/GPXDATA/1/0\"" << std::endl;
		}
		if(gpxtpx_ext)
		{
			out << "     xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\"" << std::endl;
		}
		out << "     xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1" << std::endl;
		out << "                          http://www.topografix.com/GPX/1/1/gpx.xsd\">" << std::endl;

		// TODO: Improve metadata (add link among others)
		out << "  <metadata>" << std::endl;
		out << "    <name>" << session->getName() << "</name>" << std::endl;
		out << "    <time>" << session->getBeginTime() << "</time>" << std::endl;
		out << "  </metadata>" << std::endl;

		out << "  <trk>" << std::endl;
		out << "    <trkseg>" << std::endl;
		std::vector<Point*> points = session->getPoints();
		for(const auto& point : points)
		{
			out << "      <trkpt ";
			out << point->getLatitude().toStream("lat=\"", "\" ");
			out << point->getLongitude().toStream("lon=\"", "\" ");
			out << ">" << std::endl;
			out << point->getAltitude().toStream("        <ele>", "</ele>") << std::endl;
			out << "        <time>" << point->getTimeAsString() << "</time>" << std::endl;
			if(has_extension)
			{
				out << "        <extensions>" << std::endl;
			}
			if(gpxdata_ext)
			{
				out << point->getHeartRate().toStream("          <gpxdata:hr>", "</gpxdata:hr>\n");
			}
			if(gpxtpx_ext)
			{
				out << point->getHeartRate().toStream("          <gpxtpx:TrackPointExtension><gpxtpx:hr>", "</gpxtpx:hr></gpxtpx:TrackPointExtension>\n");
			}
			if(has_extension)
			{
				out << "        </extensions>" << std::endl;
			}
			out << "      </trkpt>" << std::endl;
		}
		out << "    </trkseg>" << std::endl;
		out << "  </trk>" << std::endl;
		if(gpxdata_ext)
		{
			out << "  <extensions>" << std::endl;
			int nbLap = 0;
			std::vector<Lap*> laps = session->getLaps();
			for(const auto& lap : laps)
			{
				// TODO: Remove this check (could be moved to the getters) ?
				if(lap->getStartPoint() == nullptr || lap->getEndPoint() == nullptr)
				{
					std::cerr << "Oups ! I've got a lap without ";
					if(lap->getStartPoint() == nullptr)
						std::cerr << "start";
					else
						std::cerr << "end";
					std::cerr << " point: (" << lap->getFirstPointId() << " - " << lap->getLastPointId() << "). This shouldn't happen ! Report a bug ..." << std::endl;
				}
				else
				{
					nbLap++;
					out << "    <gpxdata:lap>" << std::endl;
					out << "      <gpxdata:index>" << nbLap << "</gpxdata:index>" << std::endl;
					if(lap->getStartPoint()->getLatitude().isDefined() && lap->getStartPoint()->getLongitude().isDefined())
					{
						out << "      <gpxdata:startPoint lat=\"" << lap->getStartPoint()->getLatitude() << "\" lon=\"" << lap->getStartPoint()->getLongitude() << "\"/>" << std::endl;
					}
					if(lap->getEndPoint()->getLatitude().isDefined() && lap->getEndPoint()->getLongitude().isDefined())
					{
						out << "      <gpxdata:endPoint lat=\"" << lap->getEndPoint()->getLatitude() << "\" lon=\"" << lap->getEndPoint()->getLongitude() << "\" />" << std::endl;
					}
					out << "      <gpxdata:startTime>" << lap->getStartPoint()->getTimeAsString() << "</gpxdata:startTime>" << std::endl;
					out << "      <gpxdata:elapsedTime>" << lap->getDuration() << "</gpxdata:elapsedTime>" << std::endl;
					out << lap->getCalories().toStream("      <gpxdata:calories>", "</gpxdata:calories>\n");
					out << "      <gpxdata:distance>" << lap->getDistance() << "</gpxdata:distance>" << std::endl;
					out << lap->getAvgSpeed().toStream("      <gpxdata:summary name=\"AverageSpeed\" kind=\"avg\">", "</gpxdata:summary>\n");
					out << lap->getMaxSpeed().toStream("      <gpxdata:summary name=\"MaximumSpeed\" kind=\"max\">", "</gpxdata:summary>\n");
					out << lap->getAvgHeartrate().toStream("      <gpxdata:summary name=\"AverageHeartRateBpm\" kind=\"avg\">", "</gpxdata:summary>\n");
					out << lap->getMaxHeartrate().toStream("      <gpxdata:summary name=\"MaximumHeartRateBpm\" kind=\"max\">", "</gpxdata:summary>\n");
					// I didn't find a way to differentiate manual lap taking versus automatic (triggered by time or distance)
					// This is the correct syntax, but pytrainer doesn't support it
					//out << "      <gpxdata:trigger kind=\"" << configuration["trigger"] << "\" />" << std::endl;
					out << "      <gpxdata:trigger>" << configuration["trigger"] << "</gpxdata:trigger>" << std::endl;
					// What can I tell about this ?! Mandatory when using gpxdata (as the two previous one) so I put it with a default value ...
					out << "      <gpxdata:intensity>active</gpxdata:intensity>" << std::endl;
					out << "    </gpxdata:lap>" << std::endl;
				}
			}
			out << "  </extensions>" << std::endl;
		}
		out << "</gpx>" << std::endl;
	}
}
