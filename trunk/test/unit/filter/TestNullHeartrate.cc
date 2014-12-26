#include <gtest/gtest.h>
#include <filter/NullHeartrate.h>
#include <bom/Session.h>
#include <bom/Point.h>

class NullHeartrateTest : public testing::Test 
{
	protected:
		std::map<std::string, std::string> emptyConfiguration;
		filter::NullHeartrate aNullHeartrateFilter;
		Session aSession;
};

TEST_F(NullHeartrateTest, BetweenTwoPointsWithSameValue)
{
	Point *firstPoint, *secondPoint, *thirdPoint;
	firstPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 0, 0, 100, 3);
	aSession.addPoint(firstPoint);
	secondPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 30, 0, 0, 3); 
	aSession.addPoint(secondPoint);
	thirdPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 60, 0, 100, 3); 
	aSession.addPoint(thirdPoint);

	aNullHeartrateFilter.filter(&aSession, emptyConfiguration);

	ASSERT_EQ(secondPoint->getHeartRate(), 100);
}

TEST_F(NullHeartrateTest, BetweenTwoPointsWithDifferentValues)
{
	Point *firstPoint, *secondPoint, *thirdPoint;
	firstPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 0, 0, 95, 3);
	aSession.addPoint(firstPoint);
	secondPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 30, 0, 0, 3); 
	aSession.addPoint(secondPoint);
	thirdPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 60, 0, 105, 3); 
	aSession.addPoint(thirdPoint);

	aNullHeartrateFilter.filter(&aSession, emptyConfiguration);

	// Current implementation is that the last heartrate is copied until a non-null heartrate is found
	// Interpolating could be a better idea
	ASSERT_EQ(secondPoint->getHeartRate(), 95);
}

TEST_F(NullHeartrateTest, FirstPointNotFixed)
{
	Point *firstPoint, *secondPoint, *thirdPoint;
	firstPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 0, 0, 0, 3);
	aSession.addPoint(firstPoint);
	secondPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 30, 0, 100, 3); 
	aSession.addPoint(secondPoint);
	thirdPoint = new Point(FieldUndef, FieldUndef, FieldUndef, FieldUndef, 60, 0, 100, 3); 
	aSession.addPoint(thirdPoint);

	aNullHeartrateFilter.filter(&aSession, emptyConfiguration);

	ASSERT_EQ(firstPoint->getHeartRate(), 0);
}
