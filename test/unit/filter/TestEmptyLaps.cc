#include <gtest/gtest.h>
#include <filter/EmptyLaps.h>

class EmptyLapsTest : public testing::Test
{
    protected:
        void addNonEmptyLap(double lat, double lon)
        {
            addLapToSession(lat, lon, 50, 20, 10);
        }

        void addNullDistanceLap(double lat, double lon)
        {
            addLapToSession(lat, lon, 50, 0, 10);
        }

        void addNullDurationLap(double lat, double lon)
        {
            addLapToSession(lat, lon, 0, 20, 10);
        }

        void addOnePointLap(double lat, double lon)
        {
            addLapToSession(lat, lon, 50, 20, 1);
        }

		std::map<std::string, std::string> emptyConfiguration;
		filter::EmptyLaps aEmptyLapsFilter;
		Session aSession;

    private:
        void addLapToSession(double lat, double lon, double duration, uint32_t length, uint32_t nbPoints)
        {
            ASSERT_NE(0, nbPoints);

            Point *startPoint(NULL), *endPoint(NULL);
            for(uint32_t j = 0; j < nbPoints; ++j)
            {
                Point *point = new Point(lat, lon+j*0.001, FieldUndef, FieldUndef, 0, 0, 100, 3);
                aSession.addPoint(point);
                if(j == 0) startPoint = point;
                endPoint = point;
            }
            Lap *lap = new Lap(0, 0, duration, length, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef);
            lap->setStartPoint(startPoint);
            lap->setEndPoint(endPoint);
            aSession.addLap(lap);
        }
};


TEST_F(EmptyLapsTest, NoLapRemovedIfNoEmptyLap)
{
    double lat(48.8583701), lon(2.2944813);
    for(int j = 0; j < 5; ++j)
    {
        addNonEmptyLap(lat+0.01*j, lon);
    }

    aEmptyLapsFilter.filter(&aSession, emptyConfiguration);

    ASSERT_EQ(5, aSession.getLaps().size());
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

    ASSERT_EQ(3, aSession.getLaps().size());
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

    ASSERT_EQ(2, aSession.getLaps().size());
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

    ASSERT_EQ(4, aSession.getLaps().size());
}
