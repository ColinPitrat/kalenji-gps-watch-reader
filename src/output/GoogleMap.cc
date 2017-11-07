#include "GoogleMap.h"
#include "../Utils.h"
#include <sstream>
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

	void GoogleMap::dumpContent(std::ostream& mystream, Session *session, std::map<std::string, std::string> &configuration)
	{
		if(configuration.find("google_api_key") == configuration.end()) {
			std::cerr << "Using GoogleMap output requires a Google API Key. You can get one from https://developers.google.com/maps/documentation/javascript/get-api-key" << std::endl;
			return;
		}
		session->ensurePointDistanceAreOk();

		// Latitude and longitude retrieved from the GPS has 6 decimals and can habe 2 digits before decimal point
		mystream.precision(8);
		mystream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
		mystream << "<html xmlns=\"http://www.w3.org/1999/xhtml\"  xmlns:v=\"urn:schemas-microsoft-com:vml\">" << std::endl;
		mystream << "<head>" << std::endl;
		mystream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" << std::endl;
		mystream << "<title>Session from " << session->getBeginTime() << "</title>" << std::endl;

		mystream << "<style media='screen' type='text/css'>" << std::endl;
		mystream << ".dygraph-legend {" << std::endl;
		mystream << "    width: 100px;" << std::endl;
		mystream << "    background-color: transparent !important;" << std::endl;
		mystream << "    left: 75px !important;" << std::endl;
		mystream << "    top: 5px !important;" << std::endl;
		mystream << "    width: 400px !important;" << std::endl;
		mystream << "    " << std::endl;
		mystream << "}" << std::endl;
		mystream << "</style>" << std::endl;

		mystream << "<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?key=" << configuration["google_api_key"] << "&sensor=false\"></script>" << std::endl;
		mystream << "<script type=\"text/javascript\">" << std::endl;
		mystream << "popupGlobal = null;" << std::endl;
		mystream << "highlightedPoint = null;" << std::endl;

		mystream << "function lap_popup_callback(event, dataLapPoint)" << std::endl;
		mystream << "{" << std::endl;
		mystream << "    var popup = new google.maps.InfoWindow({position: event.latLng, content: dataLapPoint.infos});" << std::endl;
		mystream << "    popup.open(map);" << std::endl;
		mystream << "    return popup;" << std::endl;
		mystream << "}" << std::endl;

		mystream << "function point_popup_callback(event,dataPoint)" << std::endl;
		mystream << "{" << std::endl;
		mystream << "    var popup = new google.maps.InfoWindow({position: event.latLng, " << std::endl;
		mystream << "                                            content: \"<b>Time:</b> \" + dataPoint.time + \"<br /><b>Elapsed:</b> \" + dataPoint.duration +\"<br /><b>Speed:</b> \" + dataPoint.speed + \" km/h<br /><b>Heartrate:</b> \" + dataPoint.heartrate +\" bpm<br/><b>Elevation:</b> \"+ dataPoint.altitude + \" m\"});" << std::endl;
		mystream << "    popup.open(map);" << std::endl;
		mystream << "    return popup;" << std::endl;
		mystream << "}" << std::endl;

		mystream << "pointsList = Array(" << std::endl;
		std::list<Point*> points = session->getPoints();
		uint32_t point = 0;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			// Point is latitude, longitude, color
			if(it != points.begin())
			{
			  mystream << ",";
			}
			mystream <<  "{";
			mystream << "lat:" << (*it)->getLatitude() << ", long:" << (*it)->getLongitude() << ", ";
			mystream << "distance:" << (*it)->getDistance() << ", ";
			mystream << "color: \"#";
			// Max speed is bright red, 5 km/h or less is black
			// TODO: Dynamic way to find lower bound ?
			double min_speed = 5;
			double speed_factor = (double)0xFF / (session->getMaxSpeed() - min_speed);
			int16_t sp = ((*it)->getSpeed() - min_speed) * speed_factor;
			uint32_t elapsed = ((*it)->getTime() - session->getTime()) * 1000; // in ms
			if(sp < 0) sp = 0;
			if(sp > 0xFF) sp = 0xFF;
			mystream << std::hex << std::setw(2) << std::setfill('0') << sp;
			mystream << "0000\"" << std::dec << std::setw(0) << std::setfill(' ');
			// TODO: Use max hr and min hr to determine the width range

			mystream << ", elapsed: " << elapsed;
			mystream << ", time: \"" << (*it)->getTimeAsString(true, true) << "\""; //TODO
			mystream << ", duration: \"" << durationAsString((*it)->getTime() - session->getTime()) << "\"";
			double speed = (*it)->getSpeed();
			if(std::isnan(speed)) speed = 0;
			mystream << ", speed: " << speed;
			mystream << ", heartrate: ";
			if((*it)->getHeartRate().isDefined())
				mystream << (*it)->getHeartRate();
			else
				mystream << 0;
			mystream << ", altitude: ";
			if((*it)->getAltitude().isDefined())
				mystream << (*it)->getAltitude();
			else
				mystream << 0;
			mystream << "}" << std::endl;

			++point;
		}
		mystream << ");" << std::endl;

		std::list<Lap*> laps = session->getLaps();
		uint32_t lap = 0;
		bool addComa = false;
		mystream << "waypointsList = Array (";
		for(std::list<Lap*>::iterator it = laps.begin(); it != laps.end(); ++it)
		{
			if((*it)->getEndPoint() != nullptr)
			{
			       mystream << std::endl;

			       if(addComa)
			       {
			          mystream << ",";
			       }
			       addComa = true;

			        mystream << "{";
				mystream << "lat:" << (*it)->getEndPoint()->getLatitude() << ", long:" << (*it)->getEndPoint()->getLongitude() << ", lap:" << lap;
				mystream << ", infos: \"";
				mystream << "<h3 style=\\\"padding:0; margin:0\\\">Lap " << lap+1 << "</h3>";
				mystream << "<b>Distance:</b> " << (*it)->getDistance()/1000.0 << " km<br/>";
				mystream << "<b>Time:</b> " << durationAsString((*it)->getDuration()) << "<br/>";
				mystream << (*it)->getAvgSpeed().toStream("<b>Average speed:</b> ", " km/h<br/>");
				mystream << (*it)->getMaxSpeed().toStream("<b>Maximum speed:</b> ", " km/h<br/>");
				mystream << (*it)->getAvgHeartrate().toStream("<b>Average heartrate:</b> ", " bpm<br/>");
				mystream << (*it)->getMaxHeartrate().toStream("<b>Maximum heartrate:</b> ", " bpm<br/>");
				mystream << "\"";
				mystream << "}";
			}
			++lap;
		}
		mystream << ");" << std::endl << std::endl;

		mystream << "var graph;" << std::endl;
		mystream << "var XValueToPointId = {};" << std::endl;
		mystream << "var PointIdToXValue = {};" << std::endl;
		mystream << "var xAxisAttribute = \"elapsed\";" << std::endl;

		mystream << "function loadMap() " << std::endl;
		mystream << "{" << std::endl;
		mystream << "	var centerLatLng = new google.maps.LatLng(pointsList[0].lat, pointsList[0].long);" << std::endl;
		mystream << "	var myOptions = {" << std::endl;
		mystream << "	      zoom: 14," << std::endl;
		mystream << "	      center: centerLatLng," << std::endl;
		mystream << "	      scaleControl: true," << std::endl;
		mystream << "	      mapTypeId: google.maps.MapTypeId.HYBRID" << std::endl;
		mystream << "	};" << std::endl << std::endl;
		mystream << "	map = new google.maps.Map(document.getElementById(\"map\"), myOptions);" << std::endl;
		mystream << "   highlightedPoint = new google.maps.Marker({position: centerLatLng, map: map, zIndex: 1});" << std::endl;
		mystream << "	var image_size = new google.maps.Size(32, 32);" << std::endl;
		mystream << "	var image_origin = new google.maps.Point(0, 0);" << std::endl;
		mystream << "	var image_anchor = new google.maps.Point(3, 25);" << std::endl;
		mystream << "	for (i=0; i<waypointsList.length; i++)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "	        var dataLapPoint = waypointsList[i];" << std::endl;
		mystream << "		var point = new google.maps.LatLng(dataLapPoint.lat, dataLapPoint.long);" << std::endl;
		// TODO: Use something else for the icon of lap ending
		mystream << "		var markerImage = new google.maps.MarkerImage(\"http://www.icone-gif.com/icone/isometrique/32x32/green-flag.png\", image_size, image_origin, image_anchor);" << std::endl;
		mystream << "		var markerOptions = {" << std::endl;
		mystream << "			icon: markerImage," << std::endl;
		mystream << "			position: point}" << std::endl;
		mystream << "		var markerD = new google.maps.Marker(markerOptions); " << std::endl;
		mystream << "		markerD.setMap(map);" << std::endl;
		mystream << "		attachLapPopupHandler(markerD, dataLapPoint);" << std::endl;
		mystream << "	}" << std::endl << std::endl;
		mystream << "	for (i=0; i<pointsList.length; i++)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "		if(i > 0)" << std::endl;
		mystream << "		{" << std::endl;
		mystream << "			var previousDataPoint = pointsList[i-1];" << std::endl;
		mystream << "			var currentDataPoint = pointsList[i];" << std::endl;
		mystream << "			var startPoint = new google.maps.LatLng(previousDataPoint.lat, previousDataPoint.long);" << std::endl;
		mystream << "			var endPoint = new google.maps.LatLng(currentDataPoint.lat, currentDataPoint.long);" << std::endl;
		mystream << "			var pathArray = Array(startPoint, endPoint);" << std::endl;
		mystream << "			var polyline = new google.maps.Polyline({path: pathArray," << std::endl;
		mystream << "					strokeColor: currentDataPoint.color," << std::endl;
		mystream << "					strokeOpacity: 0.9," << std::endl;
		mystream << "					strokeWeight: 5," << std::endl;
		mystream << "					});" << std::endl;
		mystream << "			polyline.setMap(map);" << std::endl;
		mystream << "			attachPopupHandler(polyline, currentDataPoint);" << std::endl;
		mystream << "		        attachMouseOverHandler(polyline, i);" << std::endl;
		mystream << "		}" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "}" << std::endl;


		mystream << "function attachLapPopupHandler(mapElement, dataLapPoint) {" << std::endl;
		mystream << "     google.maps.event.addListener(mapElement, 'click', function(evt) {lap_popup_callback(evt,dataLapPoint);});" << std::endl;
		mystream << "}" << std::endl;

		mystream << "function attachPopupHandler(mapElement, dataPoint) {" << std::endl;
		mystream << "     google.maps.event.addListener(mapElement, 'click', function(evt) {point_popup_callback(evt,dataPoint);});" << std::endl;
		mystream << "}" << std::endl;

		mystream << "function attachMouseOverHandler(mapElement, point) {" << std::endl;
		mystream << "     google.maps.event.addListener(mapElement, 'mouseover', function() {graph.setSelection(point);});" << std::endl;
		mystream << "}" << std::endl;
		mystream << "//]]>" << std::endl;
		mystream << "</script>" << std::endl;
		mystream << "<script type=\"text/javascript\" src=\"http://dygraphs.com/1.0.1/dygraph-combined.js\"></script>" << std::endl;
		mystream << "<script type=\"text/javascript\">" << std::endl;
		mystream << "// point ID, elapsed time (ms), speed (km/h), heartrate (bpm), elevation (m)" << std::endl;
		
		int i = 0;
		laps = session->getLaps();
		std::list<Lap*>::iterator itLaps = laps.begin();
		std::list<uint32_t> lapsList;
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if(itLaps == laps.end())
				break;
			while((*itLaps)->getEndPoint() == *it)
			{
				lapsList.push_back(i);
				itLaps++;
				if(itLaps == laps.end())
					break;
			}
			++i;
		}
		
		mystream << "var laps = [";
		for(std::list<uint32_t>::iterator it = lapsList.begin(); it != lapsList.end(); ++it)
		{
		        if(it !=  lapsList.begin()) {
			   mystream << ",";
			}
			mystream << *it;
		}
		mystream << "];" << std::endl;
		mystream << "var displayData = [true, true, true];" << std::endl;
		mystream << "var labelsData = [\"Speed\", \"Heart Rate\", \"Altitude\"];" << std::endl;

		
		mystream << "var XAxisValueFormater = {" << std::endl;
		mystream << "    \"elapsed\": function(ms,multiline) {" << std::endl;
		mystream << "	     var h = Math.floor(ms / (61 * 60 * 1000));" << std::endl;
		mystream << "	     ms = ms - h * (60 * 60 * 1000);" << std::endl;
		mystream << "	     var m = Math.floor(ms / (60 * 1000));" << std::endl;
		mystream << "	     ms = ms - m * (60 * 1000);" << std::endl;
		mystream << "	     var s = Math.floor(ms / 1000);" << std::endl;
		mystream << "	     ms = ms - (s * 1000);" << std::endl;
		mystream << "	     ths = Math.floor(ms / 10);" << std::endl;
		mystream << "	     var r = \"\";" << std::endl;
		mystream << "	     if(h!==0) {r = r + h +\"h\"; if(multiline) {r = r +\"<br/>\"}}" << std::endl;
		mystream << "	     r = r + m + \"mn\"; if(multiline) {r = r +\"<br/>\"}" << std::endl;
		mystream << "	     r = r + s + \".\" + ths +\"s\";" << std::endl;
		mystream << "	     return r;" << std::endl;
		mystream << "    }," << std::endl;
		mystream << "    \"distance\": function(dInMeter,multiline) {" << std::endl;
		mystream << "	     var tm = Math.floor(dInMeter / 100);" << std::endl;
		mystream << "	     var r = \"\" + (tm / 10.);" << std::endl;
		mystream << "	     if(multiline) {r = r +\"<br/>\"};" << std::endl;
		mystream << "	     r = r + \"Km\";" << std::endl;
		mystream << "	     return r;" << std::endl;
		mystream << "    }" << std::endl;
		mystream << "}" << std::endl;

		mystream << "function loadGraph() " << std::endl;
		mystream << "{" << std::endl;
		mystream << "        if(document.getElementById(\"xAxisAttributeDistance\").checked) {" << std::endl;
		mystream << "           xAxisAttribute = \"distance\";" << std::endl;
		mystream << "        }" << std::endl;
		mystream << "        else {" << std::endl;
		mystream << "           xAxisAttribute = \"elapsed\";" << std::endl;
		mystream << "        }" << std::endl;

		mystream << "   var lapsXValues = [];" << std::endl;
		mystream << "	var graphDatas=[];" << std::endl;
		mystream << "	var labels=[];" << std::endl;
		mystream << "	var iLaps = 0;" << std::endl;
		mystream << "	for(var i = 0; i < pointsList.length; i++)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "		var col = 0;" << std::endl;
		mystream << "		graphDatas[i] = [];" << std::endl;
		mystream << "		labels[col] = \"Point ID\";" << std::endl;
		mystream << "		var xValue = pointsList[i][xAxisAttribute];" << std::endl;		
		mystream << "		graphDatas[i][col++] = xValue;" << std::endl;
		mystream << "		XValueToPointId[xValue] = i;" << std::endl;
		mystream << "		PointIdToXValue[i] = xValue;" << std::endl;
		mystream << "		labels[col] = \"Laps\";" << std::endl;
		mystream << "		graphDatas[i][col++] = null;" << std::endl;
		mystream << "		if(displayData[0]) { //speed" << std::endl;
		mystream << "		    labels[col] = labelsData[0];" << std::endl;
		mystream << "		    graphDatas[i][col++] = pointsList[i].speed;" << std::endl;
		mystream << "	        }" << std::endl;
		mystream << "		if(displayData[1]) { //heartrate" << std::endl;
		mystream << "		    labels[col] = labelsData[1];" << std::endl;
		mystream << "		    graphDatas[i][col++] = pointsList[i].heartrate;" << std::endl;
		mystream << "	        }" << std::endl;
		mystream << "		if(displayData[2]) { //altitude" << std::endl;
		mystream << "		    labels[col] = labelsData[2];" << std::endl;
		mystream << "		    graphDatas[i][col++] = pointsList[i].altitude;" << std::endl;
		mystream << "	        }" << std::endl;
		mystream << "	        " << std::endl;
		mystream << "	        if(i === laps[iLaps]) {" << std::endl;
		mystream << "	            lapsXValues.push(xValue);" << std::endl;
		mystream << "	            iLaps = iLaps + 1;" << std::endl;
		mystream << "	        }" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "	graph = new Dygraph(" << std::endl;
		mystream << "	document.getElementById(\"graph\")" << std::endl;
		mystream << "	,graphDatas" << std::endl;
		mystream << "	,{" << std::endl;
		mystream << "	labels: labels," << std::endl;
		mystream << "	'Speed': { axis: {includeZero:true}}," << std::endl;
		mystream << "   colors: [\"#000000\", \"#0000FF\", \"#00AA00\", \"#FF0000\"]," << std::endl;
		mystream << "	axes: { " << std::endl;
		mystream << "	x: {" << std::endl;
		mystream << "	 valueFormatter: function(xValue) {return XAxisValueFormater[xAxisAttribute](xValue,false);}" << std::endl;
		mystream << "	 ,axisLabelFormatter: function(xValue) {return XAxisValueFormater[xAxisAttribute](xValue,true);}" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "	,ylabel: 'Altitude (m) / Heart rate (bpm)'" << std::endl;
		mystream << "	,y2label: 'Speed (km/h)'" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "	);" << std::endl;
		mystream << "	graph.updateOptions({clickCallback : function(e, x, points) { if(popupGlobal) popupGlobal.close(); e.latLng = new google.maps.LatLng(pointsList[XValueToPointId[x]].lat, pointsList[XValueToPointId[x]].long); popupGlobal = point_popup_callback(e,pointsList[XValueToPointId[x]]); } });" << std::endl;
		mystream << "	graph.updateOptions({highlightCallback : function(e, x, points) { center = new google.maps.LatLng(pointsList[XValueToPointId[x]].lat, pointsList[XValueToPointId[x]].long); map.setCenter(center); highlightedPoint.setPosition(center); } });" << std::endl;
		mystream << "	graph.updateOptions({annotationClickHandler : function(ann, pt, dg, e) { if(popupGlobal) popupGlobal.close(); e.latLng = new google.maps.LatLng(pointsList[XValueToPointId[ann.xval]].lat, pointsList[XValueToPointId[ann.xval]].long); popupGlobal = lap_popup_callback(e,waypointsList[ann.shortText-1]); } });" << std::endl;
		mystream << "	graph.updateOptions({underlayCallback: function(canvas, area, g) {" << std::endl;
		mystream << "			for(var i = 0; i+1 < lapsXValues.length; i+=2)" << std::endl;
		mystream << "			{" << std::endl;
		mystream << "              var left = graph.toDomCoords(lapsXValues[i], 0)[0];" << std::endl;
		mystream << "              var right = graph.toDomCoords(lapsXValues[i+1], 0)[0];" << std::endl;
		mystream << "              canvas.fillStyle = \"rgba(220, 220, 220, 1.0)\";" << std::endl;
		mystream << "              canvas.fillRect(left, area.y, right - left, area.h);" << std::endl;
		mystream << "			}" << std::endl;
		mystream << "		}});" << std::endl;
		mystream << "	annotations = [];" << std::endl;
		mystream << "	for(var i = 0; i < lapsXValues.length; ++i)" << std::endl;
		mystream << "	{" << std::endl;
		mystream << "		annotations.push({" << std::endl;
		mystream << "			series: 'Laps'," << std::endl;
		mystream << "			xval: lapsXValues[i]," << std::endl;
		mystream << "			attachAtBottom: true," << std::endl;
		mystream << "			shortText: (i+1)," << std::endl;
		mystream << "			text: 'Lap ' + (i+1)" << std::endl;
		mystream << "		});" << std::endl;
		mystream << "	}" << std::endl;
		mystream << "	graph.setAnnotations(annotations);" << std::endl;
		mystream << "}" << std::endl;
		mystream << "function toggleDisplay(i)" << std::endl;
		mystream << "{" << std::endl;
		mystream << "	displayData[i] = !displayData[i];" << std::endl;
		mystream << "	loadGraph();" << std::endl;
		mystream << "}" << std::endl;
		mystream << "function load()" << std::endl;
		mystream << "{" << std::endl;
		mystream << "	loadGraph();" << std::endl;
		mystream << "	loadMap();" << std::endl;
		mystream << "}" << std::endl;
		mystream << "</script>" << std::endl;
		mystream << "</head>" << std::endl;
		mystream << "<body onload=\"load()\" style=\"cursor:crosshair\" border=\"0\">" << std::endl;
		mystream << "<div id=\"map\" style=\"width: 100%; height: " << configuration["google_map_height"] << "px; top: 0px; left: 0px\"></div>" << std::endl;
		mystream << "<div id=\"graph\" style=\"width: 100%; height: 300px; top: 0px; left: 0px\"></div>" << std::endl;
		mystream << "<div id=\"spacer\" style=\"height: 25px\"></div>" << std::endl;
		mystream << "<div id=\"controls\" style=\"width: 100%; text-align:center\"><input type=\"checkbox\" name=\"Speed\" onchange=\"toggleDisplay(0)\" checked=\"checked\">Speed</input><input type=\"checkbox\" name=\"Heartrate\" onchange=\"toggleDisplay(1)\" checked=\"checked\">Heartrate</input><input type=\"checkbox\" name=\"Elevation\" onchange=\"toggleDisplay(2)\" checked=\"checked\">Elevation</input>" << std::endl;

		mystream << "  <div id=\"xAxisOptions\">" << std::endl;
		mystream << "  <span>X Axis:</span>&nbsp;" << std::endl;
		mystream << "    <input id=\"xAxisAttributeTime\" type=\"radio\" name=\"group1\" value=\"elapsed\" checked onChange=\"loadGraph();\">Time</input>" << std::endl;
		mystream << "    <input id=\"xAxisAttributeDistance\" type=\"radio\" name=\"group1\" value=\"distance\" onChange=\"loadGraph();\">Distance</input>" << std::endl;
		mystream << "  </div>" << std::endl;

		mystream << "</div>" << std::endl;
		mystream << "</body>" << std::endl;
		mystream << "</html>" << std::endl;
	}
}
