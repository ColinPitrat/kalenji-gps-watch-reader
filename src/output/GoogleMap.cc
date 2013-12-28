#include "GoogleMap.h"
#include "../Utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace output
{
	REGISTER_OUTPUT(GoogleMap);

	void GoogleMap::dumpContent(std::ostream& mystream, Session *session, std::map<std::string, std::string> &configuration)
	{
		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
		mystream << "<html xmlns=\"http://www.w3.org/1999/xhtml\"  xmlns:v=\"urn:schemas-microsoft-com:vml\">" << std::endl;
		mystream << "<head>" << std::endl;
		mystream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" << std::endl;
		mystream << "<title>Session from " << session->getBeginTime() << "</title>" << std::endl;
		mystream << "<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\"></script>" << std::endl;
		mystream << "<script type=\"text/javascript\">" << std::endl;

		std::list<Point*> points = session->getPoints();
		uint32_t point = 0;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			// Point is latitude, longitude, color
			mystream << "point" << point << " = Array(";
			mystream << (*it)->getLatitude() << "," << (*it)->getLongitude() << ",";
			mystream << "\"#"; 
			// Max speed is bright red, 5 km/h or less is black
			// TODO: Dynamic way to find lower bound ?
			double min_speed = 5;
			double speed_factor = 0xFF / (session->getMaxSpeed() - min_speed);
			int16_t sp = ((*it)->getSpeed() - min_speed) * speed_factor;
			if(sp < 0) sp = 0;
			if(sp > 0xFF) sp = 0xFF;
			mystream << std::hex << std::setw(2) << std::setfill('0') << sp;
			mystream << "0000\"" << std::dec << std::setw(0) << std::setfill(' ');
			// TODO: Use max hr and min hr to determine the width range
			uint16_t hr = (*it)->getHeartRate();
			if(hr <= 60) hr = 60;
			mystream << ");" << std::endl;

			mystream << "function point_popup_callback_" << point << "(event)" << std::endl;
			mystream << "{" << std::endl;
			mystream << "    var popup = new google.maps.InfoWindow({position: event.latLng, " << std::endl;
			mystream << "                                            content: \"";
			mystream << "<b>Time:</b> " << (*it)->getTimeAsString(true) << "<br />";
			mystream << "<b>Elapsed:</b> " << durationAsString((*it)->getTime() - session->getTime()) << "<br />";
			// TODO: Need to add a reference to the lap in each point ?
			//mystream << "<b>Elapsed lap:</b> " << durationAsString((*it)->getTimeAsString(true) << "<br />";
			mystream << "<b>Speed:</b> " << (*it)->getSpeed() << " km/h<br />";
			mystream << "<b>Heartrate:</b> " << (*it)->getHeartRate() << " bpm<br/>";
			mystream << "<b>Elevation:</b> " << (*it)->getAltitude() << " m";
			mystream << "\"});" << std::endl;
			mystream << "    popup.open(map);" << std::endl;
			mystream << "}" << std::endl;
			++point;
		}
		mystream << "pointsList = Array (";
		for(uint32_t i = 0; i < point; ++i)
		{
			if(i > 0) mystream << ",";
			mystream << "point" << i;
		}
		mystream << ");" << std::endl;
		mystream << "point_popup_callbacks = Array (";
		for(uint32_t i = 0; i < point; ++i)
		{
			if(i > 0) mystream << ",";
			mystream << "point_popup_callback_" << i;
		}
		mystream << ");" << std::endl;

		std::list<Lap*> laps = session->getLaps();
		uint32_t lap = 0;
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			if((*it)->getEndPoint() != NULL)
			{
				mystream << "waypoint" << lap << " = Array (";
				mystream << (*it)->getEndPoint()->getLatitude() << ", " << (*it)->getEndPoint()->getLongitude() << ", " << lap;
				mystream << ");" << std::endl;

				mystream << "function lap_popup_callback_" << lap << "(event)" << std::endl;
				mystream << "{" << std::endl;
				mystream << "    var popup = new google.maps.InfoWindow({position: event.latLng, " << std::endl;
				mystream << "                                            content: \"";
				mystream << "<b>Distance:</b> " << (*it)->getDistance()/1000.0 << " km<br/>";
				mystream << "<b>Time:</b> " << durationAsString((*it)->getDuration()) << "<br/>";
				mystream << (*it)->getAvgSpeed().toStream("<b>Average speed:</b> ", " km/h<br/>");
				mystream << (*it)->getMaxSpeed().toStream("<b>Maximum speed:</b> ", " km/h<br/>");
				mystream << (*it)->getAvgHeartrate().toStream("<b>Average heartrate:</b> ", " bpm<br/>");
				mystream << (*it)->getMaxHeartrate().toStream("<b>Maximum heartrate:</b> ", " bpm<br/>");
				mystream << "\"});" << std::endl;
				mystream << "    popup.open(map);" << std::endl;
				mystream << "}" << std::endl;
			}
			++lap;
		}
		mystream << "waypointsList = Array (";
		for(uint32_t i = 0; i < lap; ++i)
		{
			if(i > 0) mystream << ",";
			mystream << "waypoint" << i;
		}
		mystream << ");" << std::endl << std::endl;;
		mystream << "lap_popup_callbacks = Array (";
		for(uint32_t i = 0; i < lap; ++i)
		{
			if(i > 0) mystream << ",";
			mystream << "lap_popup_callback_" << i;
		}
		mystream << ");" << std::endl;

		mystream << "function load() " << std::endl;
		mystream << "{" << std::endl;
		mystream << "	var centerLatLng = new google.maps.LatLng(pointsList[0][0], pointsList[0][1]);" << std::endl;
		mystream << "	var myOptions = {" << std::endl;
		mystream << "	      zoom: 14," << std::endl;
		mystream << "	      center: centerLatLng," << std::endl;
		mystream << "	      scaleControl: true," << std::endl;
		mystream << "	      mapTypeId: google.maps.MapTypeId.HYBRID" << std::endl;
		mystream << "	};" << std::endl << std::endl;
		mystream << "	map = new google.maps.Map(document.getElementById(\"map\"), myOptions);" << std::endl;
		mystream << "	var image_size = new google.maps.Size(32, 32);" << std::endl;
		mystream << "	var image_origin = new google.maps.Point(0, 0);" << std::endl;
		mystream << "	var image_anchor = new google.maps.Point(3, 25);" << std::endl;
		mystream << "	for (i=0; i<waypointsList.length; i++)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "		var point = new google.maps.LatLng(waypointsList[i][0], waypointsList[i][1]);" << std::endl;
		// TODO: Use something else for the icon of lap ending
		mystream << "		var markerImage = new google.maps.MarkerImage(\"http://www.icone-gif.com/icone/isometrique/32x32/green-flag.png\", image_size, image_origin, image_anchor);" << std::endl;
		mystream << "		var markerOptions = {" << std::endl;
		mystream << "			icon: markerImage," << std::endl;
		mystream << "			position: point}" << std::endl;
		mystream << "		var markerD = new google.maps.Marker(markerOptions); " << std::endl;
		mystream << "		markerD.setMap(map);" << std::endl;
		mystream << "		google.maps.event.addListener(markerD, 'click', lap_popup_callbacks[i]);" << std::endl;
		mystream << "	}" << std::endl << std::endl;
		mystream << "	for (i=0; i<pointsList.length; i++)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "		if(i > 0)" << std::endl;
		mystream << "		{" << std::endl;
		mystream << "			var startPoint = new google.maps.LatLng(pointsList[i-1][0], pointsList[i-1][1]);" << std::endl;
		mystream << "			var endPoint = new google.maps.LatLng(pointsList[i][0], pointsList[i][1]);" << std::endl;
		mystream << "			var pathArray = Array(startPoint, endPoint);" << std::endl;
		mystream << "			var polyline = new google.maps.Polyline({path: pathArray," << std::endl;
		mystream << "					strokeColor: pointsList[i][2]," << std::endl;
		mystream << "					strokeOpacity: 0.9," << std::endl;
		mystream << "					strokeWeight: 5," << std::endl;
		mystream << "					});" << std::endl;
		mystream << "			polyline.setMap(map);" << std::endl;
		mystream << "			google.maps.event.addListener(polyline, 'click', point_popup_callbacks[i]);" << std::endl;
		mystream << "		}" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "}" << std::endl;
		mystream << "//]]>" << std::endl;
		mystream << "</script>" << std::endl;
		mystream << "</head>" << std::endl;
		mystream << "<body onload=\"load()\" style=\"cursor:crosshair\" border=\"0\">" << std::endl;
		mystream << "<div id=\"map\" style=\"width: 100%; height: 600px; top: 0px; left: 0px\"></div>" << std::endl;
		mystream << "</body>" << std::endl;
		mystream << "</html>" << std::endl;
	}
}
