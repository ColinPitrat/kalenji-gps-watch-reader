#include "GPX.h"
#include <cstring>

namespace device
{
	REGISTER_DEVICE(GPX);

	bool GPX::openDoc()
	{
		_document = xmlParseDoc(_docAsString);

		if (_document == NULL ) 
		{
			std::cerr << "Document not parsed successfully. \n";
			return false;
		}

		_rootNode = xmlDocGetRootElement(_document);

		if (_rootNode == NULL) 
		{
			std::cerr << "Empty document\n";
			xmlFreeDoc(_document);
			return false;
		}

		if (xmlStrcmp(_rootNode->name, (const xmlChar *) "gpx") != 0) 
		{
			std::cerr << "Document of the wrong type, root node != gpx (" << _rootNode->name << ")" << std::endl;
			xmlFreeDoc(_document);
			return false;
		}
		return true;
	}

	void GPX::parseDoc(Session *oSession)
	{
		// Ignored data: attributes version and creator
		xmlNodePtr cur = _rootNode->xmlChildrenNode;
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "metadata") == 0)
			{
				parseMetadata(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "trk") == 0) 
			{
				parseTrk(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "rte") == 0) 
			{
				parseTrkSegOrRoute(oSession, cur);
			}
			// TODO: The idea of a waypoint is to store a point of interest so it shouldn't be handled as rtePt or trkPt
			else if (xmlStrcmp(cur->name, (const xmlChar *) "wpt") == 0)
			{
				parseWayPoint(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "extensions") == 0) 
			{
				parseExtensions(oSession, cur);
			}
			// Ignore text content of gpx node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	void GPX::parseMetadata(Session *oSession, xmlNodePtr rootNode)
	{
		// Ignored data: desc, author, copyright, link, keywords, bounds, extensions
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "name") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oSession->setName((const char*)data);
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "time") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				if(xmlStrlen(data) >= 20)
				{
					tm time;
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time.tm_sec  = atoi((char*) data + 17);
					time.tm_isdst = -1;
					oSession->setTime(time);
				}
				else
				{
					std::cout << "Date is too short: '" << data << "'" << std::endl;
				}
				xmlFree(data);
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in metadata not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	void GPX::parseTrk(Session *oSession, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "trkseg") == 0) 
			{
				parseTrkSegOrRoute(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "name") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oSession->setName((const char*)data);
				xmlFree(data);
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in trk not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	void GPX::parseTrkSegOrRoute(Session *oSession, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "trkpt") == 0 ||
			    xmlStrcmp(cur->name, (const xmlChar *) "rtept") == 0)
			{
				parseWayPoint(oSession, cur);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "name") == 0)
			{
				oSession->setName((char*)xmlNodeListGetString(_document, cur->xmlChildrenNode, 1));
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in trkSeg not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	// Handle all waypoints (same format for trkPt, rtePt, wayPt ...)
	void GPX::parseWayPoint(Session *oSession, xmlNodePtr rootNode)
	{
		xmlChar *data;
		Point *aPoint = new Point();

		data = xmlGetProp(rootNode, (xmlChar*)"lat");
		aPoint->setLatitude(atof((char*)data));
		xmlFree(data);

		data = xmlGetProp(rootNode, (xmlChar*)"lon");
		aPoint->setLongitude(atof((char*)data));
		xmlFree(data);

		xmlNodePtr cur = rootNode->xmlChildrenNode;

		// TODO: missing speed
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "ele") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				aPoint->setAltitude(atoi((char*) data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "time") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				if(xmlStrlen(data) >= 20)
				{
					tm time_tm;
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time_tm.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time_tm.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time_tm.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time_tm.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time_tm.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time_tm.tm_sec  = atoi((char*) data + 17);
					time_tm.tm_isdst = -1;
					aPoint->setTime(atoi((char*) data));
				}
				else
				{
					std::cout << "Date is too short in point: '" << data << "'" << std::endl;
				}
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "extensions") == 0) 
			{
				parseWayPointExtensions(aPoint, cur);
			}
			// Ignore comment but if a point have one, we consider it's an important point
			else if (xmlStrcmp(cur->name, (const xmlChar *) "cmt") == 0) 
			{
				aPoint->setImportant(true);
			}
			// Ignore text content of metadata node
			else 
			if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in point not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
		oSession->addPoint(aPoint);
		//std::cout << "Got a point for " << aPoint->getLatitude() << "," << aPoint->getLongitude() << " (" << aPoint->getAltitude() << ") " << aPoint->getHeartRate() << " bpm" << std::endl;
	}

	void GPX::parseWayPointExtensions(Point *oPoint, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		xmlChar *data;

		// TODO: missing summary elements for which attribute must be checked to now the info contained
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "hr") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				oPoint->setHeartRate(atoi((char*)data));
				xmlFree(data);
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in trackPoint extensions not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	void GPX::parseExtensions(Session *oSession, xmlNodePtr rootNode)
	{
		xmlNodePtr cur = rootNode->xmlChildrenNode;
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "lap") == 0) 
			{
				parseLap(oSession, cur);
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in extensions not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
	}

	// TODO: Store begin time. Find points from lap begin time and duration. Handle max and avg for hr and speed
	void GPX::parseLap(Session *oSession, xmlNodePtr rootNode)
	{
		xmlChar *data;
		Lap *aLap = new Lap();

		xmlNodePtr cur = rootNode->xmlChildrenNode;

		// TODO: Handle start point / end point 
		while(cur != NULL)
		{
			if (xmlStrcmp(cur->name, (const xmlChar *) "index") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				aLap->setLapNum(atoi((char*) data));
				xmlFree(data);
			}
			if (xmlStrcmp(cur->name, (const xmlChar *) "calories") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				aLap->setCalories(atoi((char*) data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "distance") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				aLap->setDistance(atoi((char*) data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "elapsedTime") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				aLap->setDuration(atoi((char*) data));
				xmlFree(data);
			}
			else if (xmlStrcmp(cur->name, (const xmlChar *) "startTime") == 0) 
			{
				data = xmlNodeListGetString(_document, cur->xmlChildrenNode, 1);
				if(xmlStrlen(data) >= 20)
				{
					tm time_tm;
					// In tm, year is year since 1900. GPS returns year since 2000
					data[4]  = 0; time_tm.tm_year = atoi((char*) data) - 1900;
					// In tm, month is between 0 and 11.
					data[7]  = 0; time_tm.tm_mon  = atoi((char*) data + 5) - 1;
					data[10] = 0; time_tm.tm_mday = atoi((char*) data + 8);
					data[13] = 0; time_tm.tm_hour = atoi((char*) data + 11);
					data[16] = 0; time_tm.tm_min  = atoi((char*) data + 14);
					data[19] = 0; time_tm.tm_sec  = atoi((char*) data + 17);
					time_tm.tm_isdst = -1;
					// TODO: We need to use startTime to find start point ! Should be added to Point ?
				}
				else
				{
					std::cout << "Date is too short in point: '" << data << "'" << std::endl;
				}
				xmlFree(data);
			}
			// Ignore text content of metadata node
			else if (xmlStrcmp(cur->name, (const xmlChar *) "text") != 0) 
			{
				std::cerr << "Warning: parsing of element '" << cur->name << "' in lap not implemented in device::GPX" << std::endl;
			}
			cur = cur->next;
		}
		oSession->addLap(aLap);
	}

	void GPX::closeDoc()
	{
		xmlFreeDoc(_document);
	}

	void GPX::init()
	{
		_dataSource->init(0x0483, 0x5740);
		std::string fileContent;
		unsigned char* line;
		size_t length;
		while(_dataSource->read_data(0x81, &line, &length))
		{
			fileContent.append((char*)line, length);
		}
		_docAsString = new xmlChar[fileContent.size()];
		strcpy((char*)_docAsString, fileContent.c_str());
	}

	void GPX::getSessionsList(SessionsMap *oSessions)
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

	void GPX::getSessionsDetails(SessionsMap *oSessions)
	{
		// All work is done in getSessionsList
	}
}
