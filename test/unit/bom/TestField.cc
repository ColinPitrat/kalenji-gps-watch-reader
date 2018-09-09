#include <gtest/gtest.h>
#include <bom/Field.h>

TEST(FieldTest, CopyingUndefField)
{
	Field<double> f = FieldUndef;
	Field<double> copy = f;
	EXPECT_FALSE(copy.isDefined());
}
