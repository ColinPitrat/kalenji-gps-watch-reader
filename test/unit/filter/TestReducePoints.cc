#include <gtest/gtest.h>
#include <filter/ReducePoints.h>
#include <bom/Session.h>
#include <bom/Point.h>

class ReducePointsTest : public testing::Test 
{
	protected:
		void addPointsToSession(uint32_t nbPoints, double startLat, double startLon, double deltaLat, double deltaLon);

		std::map<std::string, std::string> aConfiguration;
		filter::ReducePoints aReducePointsFilter;
		Session aSession;
};

void ReducePointsTest::addPointsToSession(uint32_t nbPoints, double startLat, double startLon, double deltaLat, double deltaLon)
{
	for(uint32_t i = 0; i < nbPoints; ++i)
	{
		Point *point = new Point(startLat+i*deltaLat, startLon+i*deltaLon, FieldUndef, FieldUndef, 0, 0, 100, 3);
		aSession.addPoint(point);
	}
}

TEST_F(ReducePointsTest, LessPointsThanMax)
{
	aConfiguration["reduce_points_max"] = "50";
	addPointsToSession(10, 45.5781883, 2.9440619, 0.0001, 0.0001);

	aReducePointsFilter.filter(&aSession, aConfiguration);

	ASSERT_EQ(10u, aSession.getPoints().size());
}

TEST_F(ReducePointsTest, MorePointsThanMax)
{
	aConfiguration["reduce_points_max"] = "50";
	aConfiguration["reduce_points_min_dist"] = "10";
	addPointsToSession(60, 45.5781883, 2.9440619, 0.00001, 0.00001);

	aReducePointsFilter.filter(&aSession, aConfiguration);

	ASSERT_GE(50u, aSession.getPoints().size());
}

TEST_F(ReducePointsTest, MorePointsThanMaxRemovesStraightLines)
{
	aConfiguration["reduce_points_max"] = "50";
	aConfiguration["reduce_points_max_dist"] = "500";
	addPointsToSession(50, 45.5781883,        2.9440619,        0.00001,  0.00001 );
	addPointsToSession(50, 45.5781883+0.0005, 2.9440619+0.0005, 0.00001,  -0.00001);
	addPointsToSession(50, 45.5781883+0.001,  2.9440619,        -0.00001, 0.00001 );
	addPointsToSession(50, 45.5781883+0.0005, 2.9440619+0.0005, -0.00001, 0.00001 );
	addPointsToSession(50, 45.5781883,        2.9440619,        -0.00001, -0.00001);

	aReducePointsFilter.filter(&aSession, aConfiguration);

	ASSERT_EQ(6u, aSession.getPoints().size());
}
