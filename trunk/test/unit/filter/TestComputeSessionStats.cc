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

TEST_F(ComputeSessionStatsTest, AscentComputation)
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

// TODO: test ascent, descent, max speed, avg speed, duration
