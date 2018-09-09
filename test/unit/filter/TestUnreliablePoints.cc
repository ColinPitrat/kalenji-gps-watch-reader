#include <gtest/gtest.h>
#include <filter/UnreliablePoints.h>

class UnreliablePointsTest : public testing::Test
{
	protected:
		std::map<std::string, std::string> _emptyConfiguration;
		filter::UnreliablePoints _unreliablePointsFilter;
		Session _session;
};

TEST_F(UnreliablePointsTest, DoNothingIfNoUnreliablePoint)
{
  Point *p;
  for(double lat = 12.345, lon = 67.89; lat < 12.4; lat += 0.001, lon += 0.001) {
    p = new Point(12.345, 67.89, FieldUndef, FieldUndef, 0, 0, FieldUndef, 3);
    _session.addPoint(p);
  }
  EXPECT_EQ(56u, _session.getPoints().size());
  _unreliablePointsFilter.filter(&_session, _emptyConfiguration);
  EXPECT_EQ(56u, _session.getPoints().size());
}

TEST_F(UnreliablePointsTest, RemoveUnreliablePoint)
{
  Point *p;
  int i = 0;
  for(double lat = 12.345, lon = 67.89; lat < 12.4; lat += 0.001, lon += 0.001, ++i) {
    p = new Point(12.345, 67.89, FieldUndef, FieldUndef, 0, 0, FieldUndef, (i % 10) == 0 ? 0 : 3);
    _session.addPoint(p);
  }
  EXPECT_EQ(56u, _session.getPoints().size());
  _unreliablePointsFilter.filter(&_session, _emptyConfiguration);
  EXPECT_EQ(50u, _session.getPoints().size());
}

TEST_F(UnreliablePointsTest, RemoveUnreliablePointWithLaps)
{
  Point *p, *p29;
  int i = 0;
  for(double lat = 12.345, lon = 67.89; lat < 12.4; lat += 0.001, lon += 0.001, ++i) {
    p = new Point(12.345, 67.89, FieldUndef, FieldUndef, 0, 0, FieldUndef, (i % 3) == 0 ? 0 : 3);
    std::cout << "Point " << i << ": " << (void*) p << std::endl;
    if (i == 29) p29 = p;
    _session.addPoint(p);
  }
  Lap *l, *l2, *l3;
  for(int j = 0; j < 6; ++j) {
    int start = j*10, end = (j+1)*10;
    if (j == 5) end = 55;
    l = new Lap(start, end, 30, 100, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef);
    if (j == 2) l2 = l;
    if (j == 3) l3 = l;
    l->setStartPoint(_session.getPoints()[start]);
    l->setEndPoint(_session.getPoints()[end]);
    _session.addLap(l);
  }

  EXPECT_EQ(56u, _session.getPoints().size());
  _unreliablePointsFilter.filter(&_session, _emptyConfiguration);
  EXPECT_EQ(37u, _session.getPoints().size());

  // Point 30 is ending lap 2 and starting lap 3. It has been removed.
  EXPECT_EQ(p29, l2->getEndPoint());
  EXPECT_EQ(p29, l3->getStartPoint());
}
