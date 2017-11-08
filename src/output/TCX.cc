#include "TCX.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(TCX);

	void TCX::dumpContent(std::ostream& mystream, const Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(12);
		mystream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		mystream << "<TrainingCenterDatabase xmlns=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2\""
				" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation="
				"\"http://www.garmin.com/xmlschemas/ActivityExtension/v2 "
				"http://www.garmin.com/xmlschemas/ActivityExtensionv2.xsd "
				"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 "
				"http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd\">" << std::endl;
		mystream << " <Activities>" << std::endl;
		mystream << "  <Activity Sport=\"" << configuration["tcx_sport"] << "\">" << std::endl;
		mystream << "   <Id>" << session->getBeginTime() << "</Id>" << std::endl;
		std::list<Lap*> laps = session->getLaps();
		std::list<Point*> points = session->getPoints();
		auto pit = points.begin();
		uint32_t point_id = 0;
		for(const auto& lap : laps)
		{
			if(lap->getStartPoint() != nullptr)
			{
				mystream << "   <Lap StartTime=\"" << lap->getStartPoint()->getTimeAsString() << "\">" << std::endl;
			}
			else
			{
				mystream << "   <Lap>" << std::endl;
			}
			mystream << "    <TotalTimeSeconds>" << lap->getDuration() << "</TotalTimeSeconds>" << std::endl;
			mystream << "    <DistanceMeters>" << lap->getDistance() << "</DistanceMeters>" << std::endl;
			mystream << "    <MaximumSpeed>" << lap->getMaxSpeed() << "</MaximumSpeed>" << std::endl;
			mystream << "    <Calories>" << lap->getCalories() << "</Calories>" << std::endl;
			mystream << "    <AverageHeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>"
				<< lap->getAvgHeartrate() << "</Value></AverageHeartRateBpm>" << std::endl;
			mystream << "    <MaximumHeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>"
				<< lap->getMaxHeartrate() << "</Value></MaximumHeartRateBpm>" << std::endl;
			mystream << "    <Intensity>Active</Intensity>" << std::endl;
			mystream << "    <TriggerMethod>Distance</TriggerMethod>" << std::endl;

			// goto first point of this lap
			while (point_id < lap->getFirstPointId()) {
				if (pit == points.end())
					break;
				++point_id;
				++pit;
			}

			mystream << "    <Track>" << std::endl;
			double total_dist = 0;
			time_t prev_time = (*points.begin())->getTime();
			for(; pit != points.end(); ++pit, ++point_id)
			{
				double delta_tm = difftime((*pit)->getTime(), prev_time);
				total_dist += delta_tm * (*pit)->getSpeed() / 3.6;
				mystream << "     <Trackpoint>" << std::endl;
				mystream << "      <Time>" << (*pit)->getTimeAsString() << "</Time>" << std::endl;
				if ((*pit)->getLatitude().isDefined() && (*pit)->getLongitude().isDefined())
				{
					mystream << "      <Position>"
						<< (*pit)->getLatitude().toStream("<LatitudeDegrees>", "</LatitudeDegrees>")
						<< (*pit)->getLongitude().toStream("<LongitudeDegrees>", "</LongitudeDegrees>")
						<< "</Position>" << std::endl;
				}
				mystream << "      <AltitudeMeters>" << (*pit)->getAltitude().toStream() << "</AltitudeMeters>" << std::endl;
				mystream << "      <DistanceMeters>" << total_dist << "</DistanceMeters>" << std::endl;
				mystream << (*pit)->getHeartRate().toStream(
					"      <HeartRateBpm xsi:type=\"HeartRateInBeatsPerMinute_t\"><Value>",
				    "</Value></HeartRateBpm>") << std::endl;
				mystream << "      <Extensions>" << std::endl;
				mystream << "       <TPX xmlns=\"http://www.garmin.com/xmlschemas/ActivityExtension/v2\" CadenceSensor=\"Footpod\">" << std::endl;
				if((*pit)->getSpeed().isDefined())
				{
					mystream << "         <Speed>" << (*pit)->getSpeed() / 3.6 << "</Speed>" << std::endl;
				}
				mystream << "       </TPX>" << std::endl;
				mystream << "      </Extensions>" << std::endl;
				mystream << "     </Trackpoint>" << std::endl;

				prev_time = (*pit)->getTime();
				if (point_id == lap->getLastPointId())
					break;
			}
			mystream << "    </Track>" << std::endl;
			mystream << "   </Lap>" << std::endl;
		}

		// TODO: creator
		mystream << "  </Activity>" << std::endl;
		mystream << " </Activities>" << std::endl;

		// TODO: author
		mystream << "</TrainingCenterDatabase>" << std::endl;
	}
}
