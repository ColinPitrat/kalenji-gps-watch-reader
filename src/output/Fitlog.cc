#include "Fitlog.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(Fitlog);

	void Fitlog::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		out.precision(8);
		out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		out << "<FitnessWorkbook xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://www.zonefivesoftware.com/xmlschemas/FitnessLogbook/v2\">" << std::endl;
		out << " <AthleteLog>" << std::endl;
		out << "  <Athlete />" << std::endl;
		out << "  <Activity StartTime=\"" << session->getBeginTime() << "\">" << std::endl;
		out << "   <Duration TotalSeconds=\"" << session->getDuration() << "\" />" << std::endl;
		out << "   <Distance TotalMeters=\"" << session->getDistance() << "\" />" << std::endl;
		if(session->getAvgHeartrate().isDefined() || session->getMaxHeartrate().isDefined())
		{
			out << "   <HeartRate ";
			out << session->getAvgHeartrate().toStream("AverageBPM=\"", "\" ");
			out << session->getMaxHeartrate().toStream("MaximumBPM=\"", "\" "); 
			out << "/>" << std::endl;
		}
		out << "   <Laps>" << std::endl;
		std::list<Lap*> laps = session->getLaps();
		for(const auto& lap : laps)
		{
			out << "    <Lap StartTime=\"" << lap->getStartPoint()->getTimeAsString() << "\" DurationSeconds=\"" << lap->getDuration() << "\" >" << std::endl;
			out << "     <Distance TotalMeters=\"" << lap->getDistance() << "\" />" << std::endl;
			out << lap->getAvgHeartrate().toStream("     <HeartRate AverageBPM=\"", "\" />\n");
			out << lap->getCalories().toStream("     <Calories TotalCal=\"", "\" />\n");
			out << "    </Lap>" << std::endl;
		}
		out << "   </Laps>" << std::endl;

		// TODO: Verify the format !
		out << "   <Track StartTime=\"" << session->getBeginTime() << "\">" << std::endl;
		std::list<Point*> points = session->getPoints();
		time_t prev_time = (*points.begin())->getTime();
		time_t first_time = (*points.begin())->getTime();
		double total_dist = 0;
		for(const auto& point : points)
		{
			double tm = difftime(point->getTime(), first_time);
			double delta_tm = difftime(point->getTime(), prev_time);
			total_dist += delta_tm * point->getSpeed() / 3.6;
			out << "    <pt tm=\"" << (int) tm << "\" dist=\"" << total_dist << "\" ";
			out << point->getHeartRate().toStream("hr=\"", "\" ");
			out << point->getLatitude().toStream("lat=\"", "\" ");
			out << point->getLongitude().toStream("lon=\"", "\" ");
			out << point->getAltitude().toStream("ele=\"", "\" ");
			out << "/>" << std::endl;
			prev_time = point->getTime();
		}
		out << "   </Track>" << std::endl;
		out << "  </Activity>" << std::endl;
		out << " </AthleteLog>" << std::endl;
		out << "</FitnessWorkbook>" << std::endl;
		// TODO: footer
	}
}
