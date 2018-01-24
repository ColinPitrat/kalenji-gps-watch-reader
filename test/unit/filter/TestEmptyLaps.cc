#include <gtest/gtest.h>
#include <filter/EmptyLaps.h>
#include <test/unit/common.h>

class EmptyLapsTest : public testing::Test
{
    protected:
        void addNonEmptyLap(double lat, double lon)
        {
		test::addLapToSession(&aSession, lat, lon, 50, 20, 10);
        }

        void addNullDistanceLap(double lat, double lon)
        {
		test::addLapToSession(&aSession, lat, lon, 50, 0, 10);
        }

        void addNullDurationLap(double lat, double lon)
        {
		test::addLapToSession(&aSession, lat, lon, 0, 20, 10);
        }

        void addOnePointLap(double lat, double lon)
        {
		test::addLapToSession(&aSession, lat, lon, 50, 20, 1);
        }

	std::map<std::string, std::string> emptyConfiguration;
	filter::EmptyLaps aEmptyLapsFilter;
	Session aSession;
};


TEST_F(EmptyLapsTest, NoLapRemovedIfNoEmptyLap)
{
    double lat(48.8583701), lon(2.2944813);
    for(int j = 0; j < 5; ++j)
    {
        addNonEmptyLap(lat+0.01*j, lon);
    }

    aEmptyLapsFilter.filter(&aSession, emptyConfiguration);

    ASSERT_EQ(5u, aSession.getLaps().size());
}

TEST_F(EmptyLapsTest, NullDistanceLapsRemoved)
{
    double lat(48.8583701), lon(2.2944813);
    addNonEmptyLap(lat, lon);
    addNullDistanceLap(lat+0.01, lon);
    addNullDistanceLap(lat+0.02, lon);
    addNonEmptyLap(lat+0.03, lon);
    addNonEmptyLap(lat+0.04, lon);

    aEmptyLapsFilter.filter(&aSession, emptyConfiguration);

    ASSERT_EQ(3u, aSession.getLaps().size());
}

TEST_F(EmptyLapsTest, NullDurationLapsRemoved)
{
    double lat(48.8583701), lon(2.2944813);
    addNullDurationLap(lat, lon);
    addNonEmptyLap(lat+0.01, lon);
    addNullDurationLap(lat+0.02, lon);
    addNonEmptyLap(lat+0.03, lon);
    addNullDurationLap(lat+0.04, lon);

    aEmptyLapsFilter.filter(&aSession, emptyConfiguration);

    ASSERT_EQ(2u, aSession.getLaps().size());
}

TEST_F(EmptyLapsTest, SameStartAndEndPointsLapsRemoved)
{
    double lat(48.8583701), lon(2.2944813);
    addNonEmptyLap(lat, lon);
    addOnePointLap(lat+0.01, lon);
    addNonEmptyLap(lat+0.02, lon);
    addNonEmptyLap(lat+0.03, lon);
    addNonEmptyLap(lat+0.04, lon);

    aEmptyLapsFilter.filter(&aSession, emptyConfiguration);

    ASSERT_EQ(4u, aSession.getLaps().size());
}
