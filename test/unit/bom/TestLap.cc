#include <gtest/gtest.h>
#include <bom/Lap.h>

TEST(LapTest, CreatingLapWithUndefs)
{
	Lap l(0, 0, 10, 10, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef);
	EXPECT_FALSE(l.getMaxSpeed().isDefined());
	EXPECT_FALSE(l.getAvgSpeed().isDefined());
	EXPECT_FALSE(l.getMaxHeartrate().isDefined());
	EXPECT_FALSE(l.getAvgHeartrate().isDefined());
	EXPECT_FALSE(l.getCalories().isDefined());
	EXPECT_FALSE(l.getGrams().isDefined());
	EXPECT_FALSE(l.getDescent().isDefined());
	EXPECT_FALSE(l.getAscent().isDefined());
}
