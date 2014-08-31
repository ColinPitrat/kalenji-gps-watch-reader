#include "FixElevation.h"
#include "../Utils.h"
#include <curl/curl.h>
#include <cstring>
#include <list>
#include <algorithm>

#include <cstdlib>
#include <unistd.h>

namespace filter
{
	REGISTER_FILTER(FixElevation);
	std::string FixElevation::HTTPdata;

	size_t FixElevation::copyHTTPData(void *ptr, size_t size, size_t nmemb, FILE *stream)
	{
		HTTPdata.append((char*) ptr, size*nmemb);
		return size*nmemb;
	}

	bool FixElevation::parseHTTPData(Session *session, std::list<Point*>::iterator first, std::list<Point*>::iterator last)
	{
		int elevation;
		std::list<std::string> lines = splitString(HTTPdata, "\n");
		int i = 0;
		int n = 0;
		for(std::list<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
		{
			// std::cout << "Found token " << it->c_str() << std::endl;
			it->erase(remove_if(it->begin(), it->end(), isspace), it->end());
			std::list<std::string> tokens = splitString(*it, ":");
			if(tokens.size() == 2)
			{
				// std::cout << "  Cleaned token " << it->c_str() << std::endl;
				std::list<std::string>::iterator it2 = tokens.begin(); 
				if((*it2) == "\"elevation\"")
				{
					while(!((*first)->getLatitude().isDefined() && (*first)->getLongitude().isDefined()))
						++first;
					++it2;
					// std::cout << "    Found elevation " << it2->c_str() << std::endl;
					elevation = atol(it2->c_str());
					if(!(*first)->getAltitude().isDefined() || elevation != (*first)->getAltitude())
					{
						++fixed_points;
						//std::cout << "Moving " << (*first)->getLatitude() << "," << (*first)->getLongitude() << " from " << (*first)->getAltitude();
						(*first)->setAltitude(elevation);
						//std::cout << " to " << (*first)->getAltitude() << std::endl;
					}
					++first;
					++i;
				}
				if((*it2) == "\"status\"")
				{
					it2++;
					if((*it2) != "\"OK\"")
					{
						std::cerr << "Error: received an error from Google API: " << (*it2) << "." << std::endl;
						return false;
					}
				}
			}
		}
		if(last != first)
		{
			while(last != first && i <= n)
			{
				++first;
				++n;
				// Just for safety
				if(n > 100) break;
			}
			std::cerr << "Error: didn't found the number of elevation expected in answer from Google API: " << i << " found, " << n << " expected." << std::endl;
			//std::cerr << HTTPdata << std:endl;
			return false;
		}
		return true;
	}

	void FixElevation::filter(Session *session)
	{
		CURL *curl;
		CURLcode res;
		fixed_points = 0;

		curl = curl_easy_init();
		if(!curl) 
		{
			std::cerr << "Error: Didn't manage to initialize curl" << std::endl;
			return;
		}

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &FixElevation::copyHTTPData);

		std::list<Point*> &points = session->getPoints();
		//std::cout << "Retrieving elevation for " << points.size() << " points" << std::endl;

		// Retrieve points elevation 89 by 89 (max url length = 2048 => (2048 - 75) / 22 = 89)
		int i = 0;
		std::list<Point*>::iterator request_first = points.begin();
		std::list<Point*>::iterator request_last;
		std::stringstream urlparams; 
		for(std::list<Point*>::iterator it = points.begin(); it != points.end(); )
		{
			if((*it)->getLatitude().isDefined() && (*it)->getLongitude().isDefined())
			{
				if(i != 0)
					urlparams << "|";
				urlparams << (*it)->getLatitude() << "," << (*it)->getLongitude();
				++i;
			}

			if((++it == points.end() && i != 0) || i == 89)
			{
				std::stringstream url; 
				url << "http://maps.googleapis.com/maps/api/elevation/json?sensor=true&locations=";
				url << urlparams.str();
				//std::cout << "Doing a GET on " << url.str() << std::endl;

				curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
				res = curl_easy_perform(curl);
				if(res != CURLE_OK)
				{
					std::cout << "Error " << res << " when trying to get " << url.str() << std::endl;
				}
				request_last = it;
				if(!parseHTTPData(session, request_first, request_last))
				{
					std::cout << "Error when parsing result of " << url.str() << std::endl;
				}

				HTTPdata = "";
				request_first = it;
				i = 0;
				urlparams.str("");
				// Limit to 10 queries per second:
				// https://developers.google.com/maps/documentation/elevation/#Limits
				usleep(100000);
			}
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
		std::cout << "    Fixed elevation of " << fixed_points << " points." << std::endl;
	}
}
/* To reproduce:
	Basically do a GET on an URL like (2048 char max, 512 locations max, 25000 loc per day max, 25000 query per day max):
	http://maps.googleapis.com/maps/api/elevation/json?sensor=true&locations=45.790628,4.77558|45.790642,4.77556|45.790707,4.775602
	Retrieves:
	{
	   "results" : [
	      {
		 "elevation" : 279.0686340332031,
		 "location" : {
		    "lat" : 45.7906280,
		    "lng" : 4.775580
		 },
		 "resolution" : 152.7032318115234
	      },
	      {
		 "elevation" : 279.2112426757812,
		 "location" : {
		    "lat" : 45.7906420,
		    "lng" : 4.775560
		 },
		 "resolution" : 152.7032318115234
	      },
	      {
		 "elevation" : 279.1915893554688,
		 "location" : {
		    "lat" : 45.7907070,
		    "lng" : 4.7756020
		 },
		 "resolution" : 152.7032318115234
	      }
	   ],
	   "status" : "OK"
	}

steps = len(trackpoints) / 300

path = ''
lat_prev, long_prev = 0, 0
t = 0
for t in xrange(0,len(trackpoints),steps):
    lat = float(trackpoints[t].attrib['lat'])
    lon = float(trackpoints[t].attrib['lon'])
    encoded_lat, lat_prev = encode_coord(lat, lat_prev)
    encoded_long, long_prev = encode_coord(lon, long_prev)
    path += encoded_lat + encoded_long
    t += 1
url = "http://maps.googleapis.com/maps/api/elevation/json?sensor=true&samples=%d&path=enc:" % int((len(trackpoints) / steps))
url += path

try:
    google_ele = cjson.decode(urllib2.urlopen(url).read())
    if google_ele['status'] == "OK":
	t_idx = 0
	ele_points = len(google_ele['results'])
	for ele_new in xrange(0,ele_points):
	    addExt(trackpoints[t_idx], google_ele['results'][ele_new]['elevation'])
	    for intermediate in xrange(ele_new+1, ele_new+steps):
		if intermediate<len(trackpoints):
		    if ele_new==ele_points-1:
			calculated = google_ele['results'][ele_new]['elevation']
		    else:
			ele1 = google_ele['results'][ele_new]['elevation']
			ele2 = google_ele['results'][ele_new+1]['elevation']
			calculated = (ele1 * (intermediate-ele_new)  + ele2 * (steps - (intermediate-ele_new))) / steps
		    t_idx += 1
		    addExt(trackpoints[t_idx], calculated)
	    t_idx += 1
	ele_fixed = True
except urllib2.HTTPError:
    pass


def encode_coord(x, prev):
    val = int(x * 1e5)
    return encode_signed(val - prev), val

def encode_signed(n):
    tmp = n << 1
    if n < 0:
        tmp = ~tmp
    return encode_unsigned(tmp)

def encode_unsigned(n):
    b = []
    while n >= 32:
        b.append(n & 31)
        n = n >> 5
    b = [(c | 0x20) for c in b]
    b.append(n)
    b = [(i + 63) for i in b]
    return ''.join([chr(i) for i in b])

*/
