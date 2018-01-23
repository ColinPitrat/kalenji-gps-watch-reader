#include <gtest/gtest.h>
#include <output/GoogleMap.h>
#include <sstream>

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
