#include <gtest/gtest.h>
#include <filter/ComputeSessionStats.h>
#include <bom/Session.h>
#include <bom/Point.h>

class ComputeSessionStatsTest : public testing::Test 
{
	protected:
		std::map<std::string, std::string> emptyConfiguration;
		filter::ComputeSessionStats aComputeSessionsStatsFilter;
		Session aSession;
};

TEST_F(ComputeSessionStatsTest, DistanceComputation)
{
	Point *firstPoint, *secondPoint;
	// San Francisco
	firstPoint = new Point(37.779, -122.422, FieldUndef, FieldUndef, 0, 0, FieldUndef, 3);
	aSession.addPoint(firstPoint);
	// New York - 1 mn later
	secondPoint = new Point(40.7111, -74.006, FieldUndef, FieldUndef, 60, 0, FieldUndef, 3); 
	aSession.addPoint(secondPoint);

	aComputeSessionsStatsFilter.filter(&aSession, emptyConfiguration);

	// Distance SF - NY: ~ 4130 km (4130.347 km with current version)
	ASSERT_LT(abs(4130000 - aSession.getDistance()), 1000);
}

TEST_F(ComputeSessionStatsTest, AscentDescentComputationWhenClimbing)
{
	Point *firstPoint, *secondPoint;
	// Sea level
	firstPoint = new Point(0, 0, 0, FieldUndef, 0, 0, FieldUndef, 3);
	aSession.addPoint(firstPoint);
	// 100 m high, 1 min later
	secondPoint = new Point(0, 0, 100, FieldUndef, 60, 0, FieldUndef, 3); 
	aSession.addPoint(secondPoint);

	aComputeSessionsStatsFilter.filter(&aSession, emptyConfiguration);

	ASSERT_EQ(aSession.getAscent(), 100u);
	ASSERT_EQ(aSession.getDescent(), 0u);
}

TEST_F(ComputeSessionStatsTest, AscentDescentComputationWhenGoingDown)
{
	Point *firstPoint, *secondPoint;
	// Sea level
	firstPoint = new Point(0, 0, 100, FieldUndef, 0, 0, FieldUndef, 3);
	aSession.addPoint(firstPoint);
	// 100 m high, 1 min later
	secondPoint = new Point(0, 0, 0, FieldUndef, 60, 0, FieldUndef, 3); 
	aSession.addPoint(secondPoint);

	aComputeSessionsStatsFilter.filter(&aSession, emptyConfiguration);

	ASSERT_EQ(aSession.getAscent(), 0);
	ASSERT_EQ(aSession.getDescent(), 100);
}

TEST_F(ComputeSessionStatsTest, MaxAndAvgSpeedComputation)
{
	Point *firstPoint, *secondPoint, *thirdPoint;
	// San Francisco
	firstPoint = new Point(37.779, -122.422, FieldUndef, FieldUndef, 0, 0, FieldUndef, 3);
	aSession.addPoint(firstPoint);
	// New York - 1 h later
	secondPoint = new Point(40.7111, -74.006, FieldUndef, FieldUndef, 3600, 0, FieldUndef, 3); 
	aSession.addPoint(secondPoint);
	// Back to SF - 2 h later
	thirdPoint = new Point(37.779, -122.422, FieldUndef, FieldUndef, 10800, 0, FieldUndef, 3); 
	aSession.addPoint(thirdPoint);

	aComputeSessionsStatsFilter.filter(&aSession, emptyConfiguration);

	// SF - NY in 1 hour = 4130 km/h
	ASSERT_LT(abs(4130 - aSession.getMaxSpeed()), 1);
	// SF - NY - SF in 3 hour = 8260 km / 3h = 2753 km/h
	ASSERT_LT(abs(2753 - aSession.getAvgSpeed()), 1);
}

TEST_F(ComputeSessionStatsTest, DurationComputation)
{
	Point *firstPoint, *secondPoint, *thirdPoint;
	// Beginning
	firstPoint = new Point(0, 0, 100, FieldUndef, 0, 0, FieldUndef, 3);
	aSession.addPoint(firstPoint);
	// Intermediary point at 1 mn
	secondPoint = new Point(0, 0, 0, FieldUndef, 60, 0, FieldUndef, 3); 
	aSession.addPoint(secondPoint);
	// Final point at 2 mn
	thirdPoint = new Point(0, 0, 0, FieldUndef, 120, 0, FieldUndef, 3); 
	aSession.addPoint(thirdPoint);

	aComputeSessionsStatsFilter.filter(&aSession, emptyConfiguration);

	ASSERT_EQ(aSession.getDuration(), 120);
}
