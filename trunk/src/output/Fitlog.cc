#include "Fitlog.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(Fitlog);

	void Fitlog::dump(Session *session, std::map<std::string, std::string> &configuration)
	{
		std::stringstream filename;
		filename << configuration["directory"] << "/"; 
		filename << session->getYear() << std::setw(2) << std::setfill('0') << session->getMonth() << std::setw(2) << std::setfill('0') << session->getDay() << "_"; 
		filename << std::setw(2) << std::setfill('0') << session->getHour() << std::setw(2) << std::setfill('0') << session->getMinutes() << std::setw(2) << std::setfill('0') << session->getSeconds() << ".fitlog";
		std::cout << "Creating " << filename.str() << std::endl;
		std::ofstream mystream(filename.str().c_str());
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
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			mystream << "    <Lap StartTime=\"" << (*it)->getStartPoint()->getTimeAsString() << "\" DurationSeconds=\"" << (*it)->getDuration() << "\" >" << std::endl;
			mystream << "     <Distance TotalMeters=\"" << (*it)->getDistance() << "\" />" << std::endl;
			mystream << (*it)->getAvgHeartrate().toStream("     <HeartRate AverageBPM=\"", "\" />");
			mystream << (*it)->getCalories().toStream("     <Calories TotalCal=\"", "\" />");
			mystream << "    </Lap>" << std::endl;
		}
		mystream << "   </Laps>" << std::endl;

		// TODO: Verify the format !
		mystream << "   <Track StartTime=\"" << session->getBeginTime() << "\">" << std::endl;
		std::list<Point*> points = session->getPoints();
		time_t prev_time = (*points.begin())->getTime();
		time_t first_time = (*points.begin())->getTime();
		double total_dist = 0;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			double tm = difftime((*it)->getTime(), first_time);
			double delta_tm = difftime((*it)->getTime(), prev_time);
			total_dist += delta_tm * (*it)->getSpeed() / 3.6;
			mystream << "    <pt tm=\"" << (int) tm << "\" dist=\"" << total_dist << "\" ";
			mystream << (*it)->getHeartRate().toStream("hr=\"", "\" ");
			mystream << (*it)->getLatitude().toStream("lat=\"", "\" ");
			mystream << (*it)->getLongitude().toStream("lon=\"", "\" ");
			mystream << (*it)->getAltitude().toStream("ele=\"", "\" ");
			mystream << "/>" << std::endl;
			prev_time = (*it)->getTime();
		}
		mystream << "   </Track>" << std::endl;
		mystream << "  </Activity>" << std::endl;
		mystream << " </AthleteLog>" << std::endl;
		mystream << "</FitnessWorkbook>" << std::endl;
		// TODO: footer
		mystream.close();
	}
}
