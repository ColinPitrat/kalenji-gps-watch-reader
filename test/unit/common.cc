#include <gtest/gtest.h>
#include <test/unit/common.h>
#include <bom/Session.h>

namespace test
{
	void addLapToSession(Session* session, double lat, double lon, double duration, uint32_t length, uint32_t nbPoints)
	{
		ASSERT_NE(0u, nbPoints);

		Point *startPoint(nullptr), *endPoint(nullptr);
		for(uint32_t j = 0; j < nbPoints; ++j)
		{
			auto point = new Point(Field<double>(lat), Field<double>(lon+j*0.001), FieldUndef, FieldUndef, 0, 0, Field<uint16_t>(100), 3);
			session->addPoint(point);
			if(j == 0) startPoint = point;
			endPoint = point;
		}
		auto lap = new Lap(0, 0, duration, length, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef, FieldUndef);
		lap->setStartPoint(startPoint);
		lap->setEndPoint(endPoint);
		session->addLap(lap);
	}
}
