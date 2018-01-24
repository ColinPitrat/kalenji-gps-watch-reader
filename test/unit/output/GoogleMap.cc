#include <gtest/gtest.h>
#include <output/GoogleMap.h>
#include <sstream>
#include <test/unit/common.h>

class GoogleMapTest : public testing::Test
{
	protected:
		Session aSession;
		output::GoogleMap aGoogleMapOutput;
		std::ostringstream aOss;
};

TEST_F(GoogleMapTest, DumpEmptySessionSummary)
{
	aGoogleMapOutput.dumpSessionSummary(aOss, &aSession);

	std::string expected = "<div id=\"summary\" style=\"width: 100% ; text-align:left\">\n"
			"<b>Session summary:</b><br>\n"
			"Time: 0s, Distance: 0 km.</div>\n";
	EXPECT_EQ(expected, aOss.str());
}

TEST_F(GoogleMapTest, DumpOneLapSessionSummary)
{
	double lat(12.345678), lon(9.876543), duration(30);
	uint32_t length(500), nbPoints(30);
	test::addLapToSession(&aSession, lat, lon, duration, length, nbPoints);
	aGoogleMapOutput.dumpSessionSummary(aOss, &aSession);

	std::string expected = "<div id=\"summary\" style=\"width: 100% ; text-align:left\">\n"
			"<b>Session summary:</b><br>\n"
			"Time: 0s, Distance: 0 km.</div>\n"
			"<div id=\"lap_info\" style=\"width: 100% ; text-align:left\">\n"
			"<b>Lap details:</b><br>\n"
			"<table border='1'><tr><th>lap</th><th>time</th><th>distance</th><th>average speed</th><th>max speed</th><th>average heartrate</th><th>max heartrate</th></tr>\n"
			"<tr><td>  1</td><td>       30s</td><td> 0.5 km</td><td>   N/A</td><td>   N/A</td><td> N/A</td><td> N/A</td></tr>\n"
			"</table></div>";
	EXPECT_EQ(expected, aOss.str());
}
