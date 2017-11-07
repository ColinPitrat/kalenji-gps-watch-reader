#include "TCX.h"
#include <cstring>
#include "Utils.h"

namespace device
{
	REGISTER_DEVICE(TCX);

	bool TCX::openDoc()
	{
		_document = xmlParseDoc(_docAsString);

		if (_document == nullptr )
		{
			std::cerr << "Document not parsed successfully. \n" << _docAsString << std::endl;
			return false;
		}

		_rootNode = xmlDocGetRootElement(_document);

		if (_rootNode == nullptr)
		{
			std::cerr << "Empty document\n";
			xmlFreeDoc(_document);
			return false;
		}

		if (xmlStrcmp(_rootNode->name, (const xmlChar *) "TrainingCenterDatabase") != 0)
		{
			std::cerr << "Document of the wrong type, root node != TrainingCenterDatabase (" << _rootNode->name << ")" << std::endl;
			xmlFreeDoc(_document);
			return false;
		}
		return true;
	}

	void TCX::parseDoc(Session *oSession)
	{
		// Ignored data: attributes version and creator
		xmlNodePtr cur = _rootNode->xmlChildrenNode;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Activities") == 0)
			{
				parseActivities(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in TrainingCenterDatabase not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parseActivities(Session *oSession, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		int nbActivities = 0;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Activity") == 0)
			{
				if(nbActivities == 0)
				{
					parseActivity(oSession, cur);
				}
				else
				{
					std::cout << "Multiple activities in TCX. This is not supported !!! Decoding only the first one." << std::endl;
				}
				nbActivities++;
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Activities not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parseActivity(Session *oSession, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		int lapNum = 1;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Id") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oSession->setName((const char*)data);
				if(xmlStrlen(data) >= 20)
				{
					tm time;
					memset(&time, 0, sizeof(time));
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time.tm_sec  = atoi((char*) data + 17);
					time.tm_isdst = -1;
					oSession->setTimeT(mktime_utc(&time));
				}
				else
				{
					std::cout << "Date is too short: '" << data << "'" << std::endl;
				}
				xmlFree(data);
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "Lap") == 0)
			{
				Lap *aLap = new Lap();
				aLap->setLapNum(lapNum++);
				parseLap(oSession, aLap, cur);
				oSession->addLap(aLap);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Activity not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parseLap(Session *oSession, Lap *oLap, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		int nbTracks = 0;
		while(cur != nullptr)
		{
			// To handle: AverageHeartRateBpm, MaximumHeartRateBpm, Intensity, TriggerMethod
			if (xmlStrcmp(cur->name, (const xmlChar *) "StartTime") == 0)
			{
				/* How to set start time of lap ?
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				if(xmlStrlen(data) >= 20)
				{
					tm time;
					memset(&time, 0, sizeof(time));
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time.tm_sec  = atoi((char*) data + 17);
					time.tm_isdst = -1;
					oLap->setTimeT(mktime_utc(&time));
				}
				else
				{
					std::cout << "Date is too short: '" << data << "'" << std::endl;
				}
				 */
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "TotalTimeSeconds") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oLap->setDuration(atoi((char*) data));
				xmlFree(data);
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "DistanceMeters") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oLap->setDistance(atoi((char*) data));
				xmlFree(data);
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "MaximumSpeed") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oLap->setMaxSpeed(atof((char*) data));
				xmlFree(data);
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "Calories") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oLap->setCalories(atoi((char*) data));
				xmlFree(data);
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "AverageHeartRateBpm") == 0)
			{
				oLap->setAvgHeartrate(parseValue(cur));
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "MaximumHeartRateBpm") == 0)
			{
				oLap->setMaxHeartrate(parseValue(cur));
			}
			else if(xmlStrcmp(cur->name, (const xmlChar *) "Track") == 0)
			{
				if(nbTracks == 0)
				{
					parseTrack(oSession, oLap, cur);
				}
				else
				{
					std::cout << "Multiple tracks in lap in TCX. This is not supported !!! Decoding only the first one." << std::endl;
				}
				nbTracks++;
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Lap not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parseTrack(Session *oSession, Lap *oLap, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		int nbPoints = oSession->getPoints().size();
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Trackpoint") == 0)
			{
				Point *aPoint = new Point();
				parseTrackpoint(aPoint, cur);
				oSession->addPoint(aPoint);
				if(oLap->getStartPoint() == nullptr)
				{
					oLap->setStartPoint(aPoint);
					oLap->setFirstPointId(nbPoints);
				}
				nbPoints++;
				oLap->setEndPoint(aPoint);
				oLap->setLastPointId(nbPoints);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Track not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parseTrackpoint(Point *oPoint, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != nullptr)
		{
			// To handle: HeartRateBpm, Extensions
			if (xmlStrcmp(cur->name, (const xmlChar *) "Time") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				if(xmlStrlen(data) >= 20)
				{
					tm time;
					memset(&time, 0, sizeof(time));
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time.tm_sec  = atoi((char*) data + 17);
					time.tm_isdst = -1;
					oPoint->setTime(mktime_utc(&time));
				}
				else
				{
					std::cout << "Date is too short: '" << data << "'" << std::endl;
				}
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "Position") == 0)
			{
				parsePosition(oPoint, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "AltitudeMeters") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setAltitude(atoi((char*) data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "DistanceMeters") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setDistance(static_cast<uint32_t>(atof((char*) data)));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "Extensions") == 0)
			{
				parsePointExtensions(oPoint, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Trackpoint not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parsePosition(Point *oPoint, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "LatitudeDegrees") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setLatitude(atof((char*)data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "LongitudeDegrees") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setLongitude(atof((char*)data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in Trackpoint/Position not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parsePointExtensions(Point *oPoint, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "TPX") == 0)
			{
				parsePointTPX(oPoint, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in PointExtension not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	void TCX::parsePointTPX(Point *oPoint, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Speed") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setSpeed(atof((char*)data)*3.6);
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0)
			{
				LOG_VERBOSE("Warning: parsing of element '" << cur->name << "' in PointExtension not implemented in device::TCX");
			}
			cur = cur->next;
		}
	}

	double TCX::parseValue(xmlNodePtr rootNode)
	{
		double result = 0;
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != nullptr)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "Value") == 0)
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				result = atof((char*) data);
				xmlFree(data);
			}
			cur = cur->next;
		}
		return result;
	}

	void TCX::closeDoc()
	{
		xmlFreeDoc(_document);
	}

	void TCX::init(const DeviceId& deviceId)
	{
    _dataSource->init(deviceId.vendorId, deviceId.productId);
		std::string fileContent;
		unsigned char* line;
		size_t length;
		while(_dataSource->read_data(0x81, &line, &length))
		{
			fileContent.append((char*)line, length);
		}
		_docAsString = new xmlChar[fileContent.size()+2];
		strncpy((char*)_docAsString, fileContent.c_str(), fileContent.size()+1);
	}

	void TCX::getSessionsList(SessionsMap *oSessions)
	{
		Session aSession;
		if (openDoc())
		{
			oSessions->insert(SessionsMapElement(aSession.getId(), aSession));
			parseDoc(&(*oSessions)[aSession.getId()]);
			// TODO: Understand why some cores appears when trying to free the document
			//closeDoc();
		}
		// TODO: else
	}

	void TCX::getSessionsDetails(SessionsMap *oSessions)
	{
		// All work is done in getSessionsList
	}
}
