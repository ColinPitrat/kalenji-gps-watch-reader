#include "Fitlog.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(Fitlog);

	void Fitlog::dumpContent(std::ostream& mystream, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		mystream << "<FitnessWorkbook xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://www.zonefivesoftware.com/xmlschemas/FitnessLogbook/v2\">" << std::endl;
		mystream << " <AthleteLog>" << std::endl;
		mystream << "  <Athlete />" << std::endl;
		mystream << "  <Activity StartTime=\"" << session->getBeginTime() << "\">" << std::endl;
		mystream << "   <Duration TotalSeconds=\"" << session->getDuration() << "\" />" << std::endl;
		mystream << "   <Distance TotalMeters=\"" << session->getDistance() << "\" />" << std::endl;
		if(session->getAvgHeartrate().isDefined() || session->getMaxHeartrate().isDefined())
		{
			mystream << "   <HeartRate ";
			mystream << session->getAvgHeartrate().toStream("AverageBPM=\"", "\" ");
			mystream << session->getMaxHeartrate().toStream("MaximumBPM=\"", "\" "); 
			mystream << "/>" << std::endl;
		}
		mystream << "   <Laps>" << std::endl;
		std::list<Lap*> laps = session->getLaps();
		for(const auto& lap : laps)
		{
			mystream << "    <Lap StartTime=\"" << lap->getStartPoint()->getTimeAsString() << "\" DurationSeconds=\"" << lap->getDuration() << "\" >" << std::endl;
			mystream << "     <Distance TotalMeters=\"" << lap->getDistance() << "\" />" << std::endl;
			mystream << lap->getAvgHeartrate().toStream("     <HeartRate AverageBPM=\"", "\" />\n");
			mystream << lap->getCalories().toStream("     <Calories TotalCal=\"", "\" />\n");
			mystream << "    </Lap>" << std::endl;
		}
		mystream << "   </Laps>" << std::endl;

		// TODO: Verify the format !
		mystream << "   <Track StartTime=\"" << session->getBeginTime() << "\">" << std::endl;
		std::list<Point*> points = session->getPoints();
		time_t prev_time = (*points.begin())->getTime();
		time_t first_time = (*points.begin())->getTime();
		double total_dist = 0;
		for(const auto& point : points)
		{
			double tm = difftime(point->getTime(), first_time);
			double delta_tm = difftime(point->getTime(), prev_time);
			total_dist += delta_tm * point->getSpeed() / 3.6;
			mystream << "    <pt tm=\"" << (int) tm << "\" dist=\"" << total_dist << "\" ";
			mystream << point->getHeartRate().toStream("hr=\"", "\" ");
			mystream << point->getLatitude().toStream("lat=\"", "\" ");
			mystream << point->getLongitude().toStream("lon=\"", "\" ");
			mystream << point->getAltitude().toStream("ele=\"", "\" ");
			mystream << "/>" << std::endl;
			prev_time = point->getTime();
		}
		mystream << "   </Track>" << std::endl;
		mystream << "  </Activity>" << std::endl;
		mystream << " </AthleteLog>" << std::endl;
		mystream << "</FitnessWorkbook>" << std::endl;
		// TODO: footer
	}
}
