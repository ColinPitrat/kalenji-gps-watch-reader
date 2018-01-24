#ifndef _TEST_UNIT_COMMON_H_
#define _TEST_UNIT_COMMON_H_

class Session;

namespace test
{
	void addLapToSession(Session* session, double lat, double lon, double duration, uint32_t length, uint32_t nbPoints);
}

#endif
