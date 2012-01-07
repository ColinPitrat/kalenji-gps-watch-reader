#include "FixElevation.h"

namespace filter
{
	REGISTER_FILTER(FixElevation);

	void FixElevation::filter(Session *session)
	{
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
