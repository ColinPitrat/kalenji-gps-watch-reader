#include <gtest/gtest.h>
#include <Utils.h>
#include <bom/Field.h>
#include <bom/Point.h>

class UtilsTest : public testing::Test {};

TEST_F(UtilsTest, FormatterStreamAndStr)
{
    Formatter f;
    std::string expected = "Message for test";

    f << "Message" << " for " << "test";

    ASSERT_EQ(expected, f.str());
}

TEST_F(UtilsTest, FormatterStreamAndImplicitStr)
{
    Formatter f;
    std::string expected = "Message for test";

    f << "Message" << " for " << "test";

    ASSERT_EQ(expected, static_cast<std::string>(f));
}

TEST_F(UtilsTest, ThrowStreamThrows)
{
    ASSERT_THROW(THROW_STREAM("Exception " << "test " << "message"), std::runtime_error);
}

TEST_F(UtilsTest, ThrowStreamExceptionContainsMessage)
{
    std::string expected = "Exception test message";
    try
    {
        THROW_STREAM("Exception " << "test " << "message")
    }
    catch(std::runtime_error &e)
    {
        ASSERT_EQ(expected, e.what());
    }
}

TEST_F(UtilsTest, trimStringInitialTabsAndSpaces)
{
    std::string testString = "\t \t Test string";

    trimString(testString);

    ASSERT_STREQ("Test string", testString.c_str());
}

TEST_F(UtilsTest, trimStringTerminalTabsAndSpaces)
{
    std::string testString = "Test string\t \t ";

    trimString(testString);

    ASSERT_STREQ("Test string", testString.c_str());
}

TEST_F(UtilsTest, durationAsStringConvertsDurationToString)
{
    auto testString = durationAsString(891740, false);

    ASSERT_STREQ("10d 7h42m20s", testString.c_str());
}

TEST_F(UtilsTest, durationAsStringConvertsDurationToStringWithMs)
{
    auto testString = durationAsString(628495.327, true);

    ASSERT_STREQ("7d 6h34m55s327", testString.c_str());
}

TEST_F(UtilsTest, durationAsStringConvertsDurationToStringWithMsLowerThan100)
{
    auto testString = durationAsString(791342.027, true);

    ASSERT_STREQ("9d 3h49m2s027", testString.c_str());
}

TEST_F(UtilsTest, splitStringOneToken)
{
    auto tokens = splitString("toto", ",");

    ASSERT_EQ(1u, tokens.size());
    ASSERT_STREQ("toto", tokens.front().c_str());
}

TEST_F(UtilsTest, splitStringFourTokens)
{
    auto tokens = splitString("toto:titi:tata:tutu", ":");

    ASSERT_EQ(4u, tokens.size());
    ASSERT_STREQ("toto", tokens.front().c_str());
    tokens.pop_front();
    ASSERT_STREQ("titi", tokens.front().c_str());
    tokens.pop_front();
    ASSERT_STREQ("tata", tokens.front().c_str());
    tokens.pop_front();
    ASSERT_STREQ("tutu", tokens.front().c_str());
    tokens.pop_front();
}

TEST_F(UtilsTest, str_to_int_0)
{
    ASSERT_EQ(0u, str_to_int("0"));
}

TEST_F(UtilsTest, str_to_int_42)
{
    ASSERT_EQ(42u, str_to_int("42"));
}

TEST_F(UtilsTest, str_to_int_trailing_chars)
{
    ASSERT_EQ(42u, str_to_int("42azerty"));
}

TEST_F(UtilsTest, str_to_int_only_chars)
{
    ASSERT_EQ(0u, str_to_int("azerty"));
}

TEST_F(UtilsTest, distanceEarthParisLondon)
{
    Point Paris(48.8534100, 2.3488000, FieldUndef, FieldUndef, 0, 0, FieldUndef, 3);
    Point London(51.5085300, -0.1257400, FieldUndef, FieldUndef, 0, 0, FieldUndef, 3);

    ASSERT_NEAR(343867, distanceEarth(Paris, London), 1);
}
