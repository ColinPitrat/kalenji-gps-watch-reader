#include "GoogleMap.h"
#include "../Utils.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

// TODOs:
// Add session information
// Check whether highlightedPoint could be the global popup without glitches, by moving it and changing its content instead of closing it and opening a new one. Or maybe the glitches are due to a strange input to the callback ... Check the point exists ?
// Allow to choose X axis (distance or time)
namespace output
{
	REGISTER_OUTPUT(GoogleMap);

	void GoogleMap::dumpContent(std::ostream& out, const Session *session, std::map<std::string, std::string> &configuration)
	{
		if(configuration.find("google_api_key") == configuration.end()) {
			std::cerr << "Using GoogleMap output requires a Google API Key. You can get one from https://developers.google.com/maps/documentation/javascript/get-api-key" << std::endl;
			return;
		}
		const_cast<Session*>(session)->ensurePointDistanceAreOk();

		// Latitude and longitude retrieved from the GPS has 6 decimals and can have 2 digits before decimal point
		out.precision(8);
		out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
		out << "<html xmlns=\"http://www.w3.org/1999/xhtml\"  xmlns:v=\"urn:schemas-microsoft-com:vml\">" << std::endl;
		out << "<head>" << std::endl;
		out << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" << std::endl;
		out << "<title>Session from " << session->getBeginTime() << "</title>" << std::endl;

		out << "<style media='screen' type='text/css'>" << std::endl;
		out << ".dygraph-legend {" << std::endl;
		out << "    width: 100px;" << std::endl;
		out << "    background-color: transparent !important;" << std::endl;
		out << "    left: 75px !important;" << std::endl;
		out << "    top: 5px !important;" << std::endl;
		out << "    width: 400px !important;" << std::endl;
		out << "    " << std::endl;
		out << "}" << std::endl;
		out << "</style>" << std::endl;

		out << "<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?key=" << configuration["google_api_key"] << "&sensor=false\"></script>" << std::endl;
		out << "<script type=\"text/javascript\">" << std::endl;
		out << "popupGlobal = null;" << std::endl;
		out << "highlightedPoint = null;" << std::endl;

		out << "function lap_popup_callback(event, dataLapPoint)" << std::endl;
		out << "{" << std::endl;
		out << "    var popup = new google.maps.InfoWindow({position: event.latLng, content: dataLapPoint.infos});" << std::endl;
		out << "    popup.open(map);" << std::endl;
		out << "    return popup;" << std::endl;
		out << "}" << std::endl;

		out << "function point_popup_callback(event,dataPoint)" << std::endl;
		out << "{" << std::endl;
		out << "    var popup = new google.maps.InfoWindow({position: event.latLng, " << std::endl;
		out << "                                            content: \"<b>Time:</b> \" + dataPoint.time + \"<br /><b>Elapsed:</b> \" + dataPoint.duration +\"<br /><b>Speed:</b> \" + dataPoint.speed + \" km/h<br /><b>Heartrate:</b> \" + dataPoint.heartrate +\" bpm<br/><b>Elevation:</b> \"+ dataPoint.altitude + \" m\"});" << std::endl;
		out << "    popup.open(map);" << std::endl;
		out << "    return popup;" << std::endl;
		out << "}" << std::endl;

		out << "pointsList = Array(" << std::endl;
		std::list<Point*> points = session->getPoints();
		uint32_t point = 0;
		// Average speed is green
		// speed above replace green by red
		// speed below replace green by blue
		double avg_speed = session->getAvgSpeed();
		double min_speed;
		double max_speed;
		// let's cap the 10% extreme values to remove samples with
		// potential artifacts
		// sort the list of speed for that
		if (points.size() > 1)
		{
		  std::vector<double> speeds;
		  std::transform(points.begin(), points.end(), std::back_inserter(speeds),
		      [](const Point* p) -> double { return p->getSpeed(); });

		  std::sort(speeds.begin(), speeds.end());
		  min_speed = speeds[speeds.size() * 0.1];
		  max_speed = speeds[speeds.size() * 0.9];
		}
		else
		{
		  min_speed = max_speed = avg_speed;
		}
		double max_speed_factor = (double)0xFF / (max_speed - avg_speed);
		double min_speed_factor = (double)0xFF / (avg_speed - min_speed);
		for(auto it = points.begin(); it != points.end(); ++it)
		{
			// Point is latitude, longitude, color
			if(it != points.begin())
			{
			  out << ",";
			}
			out <<  "{";
			out << "lat:" << (*it)->getLatitude() << ", long:" << (*it)->getLongitude() << ", ";
			out << "distance:" << (*it)->getDistance() << ", ";
			out << "color: \"#";
			double speed = (*it)->getSpeed();
			if(std::isnan(speed) || !(*it)->getSpeed().isDefined()) speed = 0;
			double sp;
			if (speed > avg_speed)
			  sp = (speed - avg_speed) * max_speed_factor;
			else
			  sp = (avg_speed - speed) * min_speed_factor;
			if(sp < 0) sp = 0;
			if(sp > 0xff) sp = 0xFF;

			uint32_t color; // red green blue
			color = (256-(int)sp) << 8; /* green part */;
			color += (int)sp << (speed > avg_speed ? 16  /* red part if above avg */
			                                       :  0); /* blue part if below avg*/
			out << std::hex << std::setw(6) << std::setfill('0') << color;
			out << "\"" << std::dec << std::setw(0) << std::setfill(' ');
			// TODO: Use max hr and min hr to determine the width range

			uint32_t elapsed = ((*it)->getTime() - session->getTime()) * 1000; // in ms
			out << ", elapsed: " << elapsed;
			out << ", time: \"" << (*it)->getTimeAsString(true, true) << "\""; //TODO
			out << ", duration: \"" << durationAsString((*it)->getTime() - session->getTime()) << "\"";
			out << ", speed: " << speed;
			out << ", heartrate: ";
			if((*it)->getHeartRate().isDefined())
				out << (*it)->getHeartRate();
			else
				out << 0;
			out << ", altitude: ";
			if((*it)->getAltitude().isDefined())
				out << (*it)->getAltitude();
			else
				out << 0;
			out << "}" << std::endl;

			++point;
		}
		out << ");" << std::endl;

		std::list<Lap*> laps = session->getLaps();
		uint32_t nbLap = 0;
		bool addComa = false;
		out << "waypointsList = Array (";
		for(const auto& lap : laps)
		{
			if(lap->getEndPoint() != nullptr)
			{
			       out << std::endl;

			       if(addComa)
			       {
			          out << ",";
			       }
			       addComa = true;

			        out << "{";
				out << "lat:" << lap->getEndPoint()->getLatitude() << ", long:" << lap->getEndPoint()->getLongitude() << ", lap:" << lap->getLapNum() + 1;
				out << ", infos: \"";
				out << "<h3 style=\\\"padding:0; margin:0\\\">Lap " << lap->getLapNum() + 1 << "</h3>";
				out << "<b>Distance:</b> " << lap->getDistance()/1000.0 << " km<br/>";
				out << "<b>Time:</b> " << durationAsString(lap->getDuration()) << "<br/>";
				out << lap->getAvgSpeed().toStream("<b>Average speed:</b> ", " km/h<br/>");
				out << lap->getMaxSpeed().toStream("<b>Maximum speed:</b> ", " km/h<br/>");
				out << lap->getAvgHeartrate().toStream("<b>Average heartrate:</b> ", " bpm<br/>");
				out << lap->getMaxHeartrate().toStream("<b>Maximum heartrate:</b> ", " bpm<br/>");
				out << "\"";
				out << "}";
			}
			++nbLap;
		}
		out << ");" << std::endl << std::endl;

		out << "var graph;" << std::endl;
		out << "var XValueToPointId = {};" << std::endl;
		out << "var PointIdToXValue = {};" << std::endl;
		out << "var xAxisAttribute = \"elapsed\";" << std::endl;

		out << "function loadMap() " << std::endl;
		out << "{" << std::endl;
		out << "	var centerLatLng = new google.maps.LatLng(pointsList[0].lat, pointsList[0].long);" << std::endl;
		out << "	var myOptions = {" << std::endl;
		out << "	      zoom: 14," << std::endl;
		out << "	      center: centerLatLng," << std::endl;
		out << "	      scaleControl: true," << std::endl;
		out << "	      mapTypeId: google.maps.MapTypeId.HYBRID" << std::endl;
		out << "	};" << std::endl << std::endl;
		out << "	map = new google.maps.Map(document.getElementById(\"map\"), myOptions);" << std::endl;
		out << "   highlightedPoint = new google.maps.Marker({position: centerLatLng, map: map, zIndex: 1});" << std::endl;
		out << "	var image_size = new google.maps.Size(32, 32);" << std::endl;
		out << "	var image_origin = new google.maps.Point(0, 0);" << std::endl;
		out << "	var image_anchor = new google.maps.Point(3, 25);" << std::endl;
		out << "	for (i=0; i<waypointsList.length; i++)" << std::endl;
		out << "	{" << std::endl;
		out << "	        var dataLapPoint = waypointsList[i];" << std::endl;
		out << "		var point = new google.maps.LatLng(dataLapPoint.lat, dataLapPoint.long);" << std::endl;
		// TODO: Use something else for the icon of lap ending
		out << "		var markerImage = new google.maps.MarkerImage(\"http://www.icone-gif.com/icone/isometrique/32x32/green-flag.png\", image_size, image_origin, image_anchor);" << std::endl;
		out << "		var markerOptions = {" << std::endl;
		out << "			icon: markerImage," << std::endl;
		out << "			position: point}" << std::endl;
		out << "		var markerD = new google.maps.Marker(markerOptions); " << std::endl;
		out << "		markerD.setMap(map);" << std::endl;
		out << "		attachLapPopupHandler(markerD, dataLapPoint);" << std::endl;
		out << "	}" << std::endl << std::endl;
		out << "	for (i=0; i<pointsList.length; i++)" << std::endl;
		out << "	{" << std::endl;
		out << "		if(i > 0)" << std::endl;
		out << "		{" << std::endl;
		out << "			var previousDataPoint = pointsList[i-1];" << std::endl;
		out << "			var currentDataPoint = pointsList[i];" << std::endl;
		out << "			var startPoint = new google.maps.LatLng(previousDataPoint.lat, previousDataPoint.long);" << std::endl;
		out << "			var endPoint = new google.maps.LatLng(currentDataPoint.lat, currentDataPoint.long);" << std::endl;
		out << "			var pathArray = Array(startPoint, endPoint);" << std::endl;
		out << "			var polyline = new google.maps.Polyline({path: pathArray," << std::endl;
		out << "					strokeColor: currentDataPoint.color," << std::endl;
		out << "					strokeOpacity: 0.9," << std::endl;
		out << "					strokeWeight: 5," << std::endl;
		out << "					});" << std::endl;
		out << "			polyline.setMap(map);" << std::endl;
		out << "			attachPopupHandler(polyline, currentDataPoint);" << std::endl;
		out << "		        attachMouseOverHandler(polyline, i);" << std::endl;
		out << "		}" << std::endl;
		out << "	}" << std::endl;
		out << "}" << std::endl;


		out << "function attachLapPopupHandler(mapElement, dataLapPoint) {" << std::endl;
		out << "     google.maps.event.addListener(mapElement, 'click', function(evt) {lap_popup_callback(evt,dataLapPoint);});" << std::endl;
		out << "}" << std::endl;

		out << "function attachPopupHandler(mapElement, dataPoint) {" << std::endl;
		out << "     google.maps.event.addListener(mapElement, 'click', function(evt) {point_popup_callback(evt,dataPoint);});" << std::endl;
		out << "}" << std::endl;

		out << "function attachMouseOverHandler(mapElement, point) {" << std::endl;
		out << "     google.maps.event.addListener(mapElement, 'mouseover', function() {graph.setSelection(point);});" << std::endl;
		out << "}" << std::endl;
		out << "//]]>" << std::endl;
		out << "</script>" << std::endl;
		out << "<script type=\"text/javascript\" src=\"http://dygraphs.com/1.0.1/dygraph-combined.js\"></script>" << std::endl;
		out << "<script type=\"text/javascript\">" << std::endl;
		out << "// point ID, elapsed time (ms), speed (km/h), heartrate (bpm), elevation (m)" << std::endl;
		
		int i = 0;
		laps = session->getLaps();
		auto itLaps = laps.begin();
		std::list<uint32_t> lapsList;
		for(const auto& point : points)
		{
			if(itLaps == laps.end())
				break;
			while((*itLaps)->getEndPoint() == point)
			{
				lapsList.push_back(i);
				itLaps++;
				if(itLaps == laps.end())
					break;
			}
			++i;
		}
		
		out << "var laps = [";
		for(auto it = lapsList.begin(); it != lapsList.end(); ++it)
		{
      if(it !=  lapsList.begin()) {
			   out << ",";
			}
			out << *it;
		}
		out << "];" << std::endl;
		out << "var displayData = [true, true, true];" << std::endl;
		out << "var labelsData = [\"Speed\", \"Heart Rate\", \"Altitude\"];" << std::endl;

		
		out << "var XAxisValueFormater = {" << std::endl;
		out << "    \"elapsed\": function(ms,multiline) {" << std::endl;
		out << "	     var h = Math.floor(ms / (61 * 60 * 1000));" << std::endl;
		out << "	     ms = ms - h * (60 * 60 * 1000);" << std::endl;
		out << "	     var m = Math.floor(ms / (60 * 1000));" << std::endl;
		out << "	     ms = ms - m * (60 * 1000);" << std::endl;
		out << "	     var s = Math.floor(ms / 1000);" << std::endl;
		out << "	     ms = ms - (s * 1000);" << std::endl;
		out << "	     ths = Math.floor(ms / 10);" << std::endl;
		out << "	     var r = \"\";" << std::endl;
		out << "	     if(h!==0) {r = r + h +\"h\"; if(multiline) {r = r +\"<br/>\"}}" << std::endl;
		out << "	     r = r + m + \"mn\"; if(multiline) {r = r +\"<br/>\"}" << std::endl;
		out << "	     r = r + s + \".\" + ths +\"s\";" << std::endl;
		out << "	     return r;" << std::endl;
		out << "    }," << std::endl;
		out << "    \"distance\": function(dInMeter,multiline) {" << std::endl;
		out << "	     var tm = Math.floor(dInMeter / 100);" << std::endl;
		out << "	     var r = \"\" + (tm / 10.);" << std::endl;
		out << "	     if(multiline) {r = r +\"<br/>\"};" << std::endl;
		out << "	     r = r + \"Km\";" << std::endl;
		out << "	     return r;" << std::endl;
		out << "    }" << std::endl;
		out << "}" << std::endl;

		out << "function loadGraph() " << std::endl;
		out << "{" << std::endl;
		out << "        if(document.getElementById(\"xAxisAttributeDistance\").checked) {" << std::endl;
		out << "           xAxisAttribute = \"distance\";" << std::endl;
		out << "        }" << std::endl;
		out << "        else {" << std::endl;
		out << "           xAxisAttribute = \"elapsed\";" << std::endl;
		out << "        }" << std::endl;

		out << "   var lapsXValues = [];" << std::endl;
		out << "	var graphDatas=[];" << std::endl;
		out << "	var labels=[];" << std::endl;
		out << "	var iLaps = 0;" << std::endl;
		out << "	for(var i = 0; i < pointsList.length; i++)" << std::endl;
		out << "	{" << std::endl;
		out << "		var col = 0;" << std::endl;
		out << "		graphDatas[i] = [];" << std::endl;
		out << "		labels[col] = \"Point ID\";" << std::endl;
		out << "		var xValue = pointsList[i][xAxisAttribute];" << std::endl;		
		out << "		graphDatas[i][col++] = xValue;" << std::endl;
		out << "		XValueToPointId[xValue] = i;" << std::endl;
		out << "		PointIdToXValue[i] = xValue;" << std::endl;
		out << "		labels[col] = \"Laps\";" << std::endl;
		out << "		graphDatas[i][col++] = null;" << std::endl;
		out << "		if(displayData[0]) { //speed" << std::endl;
		out << "		    labels[col] = labelsData[0];" << std::endl;
		out << "		    graphDatas[i][col++] = pointsList[i].speed;" << std::endl;
		out << "	        }" << std::endl;
		out << "		if(displayData[1]) { //heartrate" << std::endl;
		out << "		    labels[col] = labelsData[1];" << std::endl;
		out << "		    graphDatas[i][col++] = pointsList[i].heartrate;" << std::endl;
		out << "	        }" << std::endl;
		out << "		if(displayData[2]) { //altitude" << std::endl;
		out << "		    labels[col] = labelsData[2];" << std::endl;
		out << "		    graphDatas[i][col++] = pointsList[i].altitude;" << std::endl;
		out << "	        }" << std::endl;
		out << "	        " << std::endl;
		out << "	        if(i === laps[iLaps]) {" << std::endl;
		out << "	            lapsXValues.push(xValue);" << std::endl;
		out << "	            iLaps = iLaps + 1;" << std::endl;
		out << "	        }" << std::endl;
		out << "	}" << std::endl;
		out << "	graph = new Dygraph(" << std::endl;
		out << "	document.getElementById(\"graph\")" << std::endl;
		out << "	,graphDatas" << std::endl;
		out << "	,{" << std::endl;
		out << "	labels: labels," << std::endl;
		out << "	'Speed': { axis: {includeZero:true}}," << std::endl;
		out << "   colors: [\"#000000\", \"#0000FF\", \"#00AA00\", \"#FF0000\"]," << std::endl;
		out << "	axes: { " << std::endl;
		out << "	x: {" << std::endl;
		out << "	 valueFormatter: function(xValue) {return XAxisValueFormater[xAxisAttribute](xValue,false);}" << std::endl;
		out << "	 ,axisLabelFormatter: function(xValue) {return XAxisValueFormater[xAxisAttribute](xValue,true);}" << std::endl;
		out << "	}" << std::endl;
		out << "	}" << std::endl;
		out << "	,ylabel: 'Altitude (m) / Heart rate (bpm)'" << std::endl;
		out << "	,y2label: 'Speed (km/h)'" << std::endl;
		out << "	}" << std::endl;
		out << "	);" << std::endl;
		out << "	graph.updateOptions({clickCallback : function(e, x, points) { if(popupGlobal) popupGlobal.close(); e.latLng = new google.maps.LatLng(pointsList[XValueToPointId[x]].lat, pointsList[XValueToPointId[x]].long); popupGlobal = point_popup_callback(e,pointsList[XValueToPointId[x]]); } });" << std::endl;
		out << "	graph.updateOptions({highlightCallback : function(e, x, points) { center = new google.maps.LatLng(pointsList[XValueToPointId[x]].lat, pointsList[XValueToPointId[x]].long); map.setCenter(center); highlightedPoint.setPosition(center); } });" << std::endl;
		out << "	graph.updateOptions({annotationClickHandler : function(ann, pt, dg, e) { if(popupGlobal) popupGlobal.close(); e.latLng = new google.maps.LatLng(pointsList[XValueToPointId[ann.xval]].lat, pointsList[XValueToPointId[ann.xval]].long); popupGlobal = lap_popup_callback(e,waypointsList[ann.shortText-1]); } });" << std::endl;
		out << "	graph.updateOptions({underlayCallback: function(canvas, area, g) {" << std::endl;
		out << "			for(var i = 0; i+1 < lapsXValues.length; i+=2)" << std::endl;
		out << "			{" << std::endl;
		out << "              var left = graph.toDomCoords(lapsXValues[i], 0)[0];" << std::endl;
		out << "              var right = graph.toDomCoords(lapsXValues[i+1], 0)[0];" << std::endl;
		out << "              canvas.fillStyle = \"rgba(220, 220, 220, 1.0)\";" << std::endl;
		out << "              canvas.fillRect(left, area.y, right - left, area.h);" << std::endl;
		out << "			}" << std::endl;
		out << "		}});" << std::endl;
		out << "	annotations = [];" << std::endl;
		out << "	for(var i = 0; i < lapsXValues.length; ++i)" << std::endl;
		out << "	{" << std::endl;
		out << "		annotations.push({" << std::endl;
		out << "			series: 'Laps'," << std::endl;
		out << "			xval: lapsXValues[i]," << std::endl;
		out << "			attachAtBottom: true," << std::endl;
		out << "			shortText: (i+1)," << std::endl;
		out << "			text: 'Lap ' + (i+1)" << std::endl;
		out << "		});" << std::endl;
		out << "	}" << std::endl;
		out << "	graph.setAnnotations(annotations);" << std::endl;
		out << "}" << std::endl;
		out << "function toggleDisplay(i)" << std::endl;
		out << "{" << std::endl;
		out << "	displayData[i] = !displayData[i];" << std::endl;
		out << "	loadGraph();" << std::endl;
		out << "}" << std::endl;
		out << "function load()" << std::endl;
		out << "{" << std::endl;
		out << "	loadGraph();" << std::endl;
		out << "	loadMap();" << std::endl;
		out << "}" << std::endl;
		out << "</script>" << std::endl;
		out << "</head>" << std::endl;
		out << "<body onload=\"load()\" style=\"cursor:crosshair\" border=\"0\">" << std::endl;
		out << "<div id=\"map\" style=\"width: 100%; height: " << configuration["google_map_height"] << "px; top: 0px; left: 0px\"></div>" << std::endl;
		out << "<div id=\"graph\" style=\"width: 100%; height: 300px; top: 0px; left: 0px\"></div>" << std::endl;
		out << "<div id=\"spacer\" style=\"height: 25px\"></div>" << std::endl;
		out << "<div id=\"controls\" style=\"width: 100%; text-align:center\"><input type=\"checkbox\" name=\"Speed\" onchange=\"toggleDisplay(0)\" checked=\"checked\">Speed</input><input type=\"checkbox\" name=\"Heartrate\" onchange=\"toggleDisplay(1)\" checked=\"checked\">Heartrate</input><input type=\"checkbox\" name=\"Elevation\" onchange=\"toggleDisplay(2)\" checked=\"checked\">Elevation</input>" << std::endl;

		out << "  <div id=\"xAxisOptions\">" << std::endl;
		out << "  <span>X Axis:</span>&nbsp;" << std::endl;
		out << "    <input id=\"xAxisAttributeTime\" type=\"radio\" name=\"group1\" value=\"elapsed\" checked onChange=\"loadGraph();\">Time</input>" << std::endl;
		out << "    <input id=\"xAxisAttributeDistance\" type=\"radio\" name=\"group1\" value=\"distance\" onChange=\"loadGraph();\">Distance</input>" << std::endl;
		out << "  </div>" << std::endl;

		dumpSessionSummary(out, session);

		out << "</div>" << std::endl;
		out << "</body>" << std::endl;
		out << "</html>" << std::endl;
	}

	void GoogleMap::dumpSessionSummary(std::ostream &out, const Session* session)
	{
		out << "<div id=\"summary\" style=\"width: 100% ; text-align:left\">" << std::endl;
		out << "<b>Session summary:</b><br>" << std::endl;
		out << "Time: " << durationAsString(session->getDuration()) << ", ";
		out << "Distance: " << session->getDistance()/1000.0 << " km";
		out << session->getMaxSpeed().toStream(", MaxSpeed: ", " km/h,");
		out << session->getAvgSpeed().toStream(", AvgSpeed: ", " km/h");
		out << ".</div>" << std::endl;
		auto laps = session->getLaps();
		if (laps.size() > 0)
		{
			out << "<div id=\"lap_info\" style=\"width: 100% ; text-align:left\">" << std::endl;
			out << "<b>Lap details:</b><br>" << std::endl;
			out << "<table border='1'><tr><th>lap</th><th>time</th><th>distance</th><th>average speed</th><th>max speed</th><th>average heartrate</th><th>max heartrate</th></tr>" << std::endl;
			for(const auto& lap : laps)
			{
				out << "<tr>";
				out << "<td>" << std::setw(3)  << lap->getLapNum() + 1                 << "</td>";
				out << "<td>" << std::setw(10) << durationAsString(lap->getDuration()) << "</td>";
				out << "<td>" << std::setw(4)  << lap->getDistance()/1000.0            << " km</td>";
				out << "<td>" << std::setw(6)  << lap->getAvgSpeed()                   << " km/h</td>";
				out << "<td>" << std::setw(6)  << lap->getMaxSpeed()                   << " km/h</td>";
				out << "<td>" << std::setw(4)  << lap->getAvgHeartrate()               << " bpm</td>";
				out << "<td>" << std::setw(4)  << lap->getMaxHeartrate()               << " bpm</td>";
				out << "</tr>" << std::endl;
			}
			out << "</table></div>";
		}
	}
}
