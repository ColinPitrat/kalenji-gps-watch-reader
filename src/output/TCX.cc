#include "TCX.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(TCX);

	void TCX::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		out.precision(12);
		out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		out << "<TrainingCenterDatabase xmlns=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2\""
				" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation="
				"\"http://www.garmin.com/xmlschemas/ActivityExtension/v2 "
				"http://www.garmin.com/xmlschemas/ActivityExtensionv2.xsd "
				"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 "
				"http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd\">" << std::endl;
		out << " <Activities>" << std::endl;
		out << "  <Activity Sport=\"" << configuration["tcx_sport"] << "\">" << std::endl;
		out << "   <Id>" << session->getBeginTime() << "</Id>" << std::endl;
		std::list<Lap*> laps = session->getLaps();
		std::list<Point*> points = session->getPoints();
		auto pit = points.begin();
		uint32_t point_id = 0;
		for(const auto& lap : laps)
		{
			if(lap->getStartPoint() != nullptr)
			{
				out << "   <Lap StartTime=\"" << lap->getStartPoint()->getTimeAsString() << "\">" << std::endl;
			}
			else
			{
				out << "   <Lap>" << std::endl;
			}
			out << "    <TotalTimeSeconds>" << lap->getDuration() << "</TotalTimeSeconds>" << std::endl;
			out << "    <DistanceMeters>" << lap->getDistance() << "</DistanceMeters>" << std::endl;
			out << "    <MaximumSpeed>" << lap->getMaxSpeed() << "</MaximumSpeed>" << std::endl;
			out << "    <Calories>" << lap->getCalories() << "</Calories>" << std::endl;
			out << "    <AverageHeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>"
				<< lap->getAvgHeartrate() << "</Value></AverageHeartRateBpm>" << std::endl;
			out << "    <MaximumHeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>"
				<< lap->getMaxHeartrate() << "</Value></MaximumHeartRateBpm>" << std::endl;
			out << "    <Intensity>Active</Intensity>" << std::endl;
			out << "    <TriggerMethod>Distance</TriggerMethod>" << std::endl;

			// goto first point of this lap
			while (point_id < lap->getFirstPointId()) {
				if (pit == points.end())
					break;
				++point_id;
				++pit;
			}

			out << "    <Track>" << std::endl;
			double total_dist = 0;
			time_t prev_time = (*points.begin())->getTime();
			for(; pit != points.end(); ++pit, ++point_id)
			{
				double delta_tm = difftime((*pit)->getTime(), prev_time);
				total_dist += delta_tm * (*pit)->getSpeed() / 3.6;
				out << "     <Trackpoint>" << std::endl;
				out << "      <Time>" << (*pit)->getTimeAsString() << "</Time>" << std::endl;
				if ((*pit)->getLatitude().isDefined() && (*pit)->getLongitude().isDefined())
				{
					out << "      <Position>"
						<< (*pit)->getLatitude().toStream("<LatitudeDegrees>", "</LatitudeDegrees>")
						<< (*pit)->getLongitude().toStream("<LongitudeDegrees>", "</LongitudeDegrees>")
						<< "</Position>" << std::endl;
				}
				out << "      <AltitudeMeters>" << (*pit)->getAltitude().toStream() << "</AltitudeMeters>" << std::endl;
				out << "      <DistanceMeters>" << total_dist << "</DistanceMeters>" << std::endl;
				out << (*pit)->getHeartRate().toStream(
					"      <HeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>",
				    "</Value></HeartRateBpm>") << std::endl;
				out << "      <Extensions>" << std::endl;
				out << "       <TPX xmlns=\"http://www.garmin.com/xmlschemas/ActivityExtension/v2\" CadenceSensor=\"Footpod\">" << std::endl;
				if((*pit)->getSpeed().isDefined())
				{
					out << "         <Speed>" << (*pit)->getSpeed() / 3.6 << "</Speed>" << std::endl;
				}
				out << "       </TPX>" << std::endl;
				out << "      </Extensions>" << std::endl;
				out << "     </Trackpoint>" << std::endl;

				prev_time = (*pit)->getTime();
				if (point_id == lap->getLastPointId())
					break;
			}
			out << "    </Track>" << std::endl;
			out << "   </Lap>" << std::endl;
		}

		// TODO: creator
		out << "  </Activity>" << std::endl;
		out << " </Activities>" << std::endl;

		// TODO: author
		out << "</TrainingCenterDatabase>" << std::endl;
	}
}
