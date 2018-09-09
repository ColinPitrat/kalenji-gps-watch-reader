#include <gtest/gtest.h>
#include <bom/Session.h>

#include <sstream>
#include <ctime>

TEST(SessionTest, SessionToStream)
{
  std::ostringstream oss;
  tm begin;
  strptime("2000-12-31 00:00:00", "%Y-%m-%d %H:%M:%S", &begin);
  SessionId id = {'4', '2'};
  Session session(id, 17, begin, 120, 13*60+14, 1000, 1);
  // Session constructor assumes local time.
  session.setTimeT(mktime_utc(&begin));

  oss << session;

  EXPECT_EQ("   17 - 2000-12-31 00:00:00     1 laps          1 km          13m14s",oss.str());
}
