#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <source/Logger.h>

#include <stdexcept>
#include <source/Source.h>

class MockSource : public source::Source
{
 public:
	MOCK_METHOD2(init, void(uint32_t vendorId, uint32_t productId));
	MOCK_METHOD0(release, void());
	MOCK_METHOD3(read_data, bool(unsigned char iEndPoint, unsigned char **oData, size_t *oLength));
	MOCK_METHOD3(write_data, void(unsigned char iEndPoint, unsigned char *iData, size_t iLength));
	MOCK_METHOD6(control_transfer, void(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength));

	std::string getName() override { return "MockSource"; };
};

class LoggerTest : public testing::Test
{
 public:
  LoggerTest() : _logger(&_src, "/tmp/logfile.txt") {}

 protected:
  void TestForwardReadData(bool rc, unsigned char* expected_data, size_t expected_length);
  void TestForwardWriteData(unsigned char* data, size_t length);

  MockSource _src;
  source::Logger _logger;
};

TEST_F(LoggerTest, ConstructorThrowsIfNullSourceGiven)
{
	ASSERT_THROW(source::Logger(nullptr, "logfile.txt"), std::invalid_argument);
}

TEST_F(LoggerTest, ForwardsInit)
{
	uint32_t vendor(42), product(17);
	EXPECT_CALL(_src, init(vendor, product));
	_logger.init(vendor, product);
}

TEST_F(LoggerTest, ForwardsRelease)
{
	EXPECT_CALL(_src, release());
	_logger.release();
}

void LoggerTest::TestForwardReadData(bool rc, unsigned char* expected_data, size_t expected_length) {
	unsigned char ep = 0x42;
	unsigned char *data = nullptr;
	size_t len = 42;

	EXPECT_CALL(_src, read_data(ep, &data, &len))
			.WillOnce(DoAll(
					testing::SetArgPointee<1>(expected_data),
					testing::SetArgPointee<2>(expected_length),
					testing::Return(rc)));

	EXPECT_EQ(rc, _logger.read_data(ep, &data, &len));
}

TEST_F(LoggerTest, ForwardsReadDataReturningTrueNoData)
{
	TestForwardReadData(true, nullptr, 0);
}

TEST_F(LoggerTest, ForwardsReadDataReturningFalseNoData)
{
	TestForwardReadData(false, nullptr, 0);
}

TEST_F(LoggerTest, ForwardsReadDataReturningFalseWithData)
{
	std::vector<unsigned char> data = { 12, 24, 48, 96, 192 };
	TestForwardReadData(false, data.data(), data.size());
}

void LoggerTest::TestForwardWriteData(unsigned char* data, size_t len) {
	unsigned char ep = 0x42;

	EXPECT_CALL(_src, write_data(ep, data, len));

	_logger.write_data(ep, data, len);
}

TEST_F(LoggerTest, ForwardsWriteData)
{
	std::vector<unsigned char> data = { 0xDE, 0xAD, 0xBE, 0xEF };
	TestForwardWriteData(data.data(), data.size());
}

TEST_F(LoggerTest, ForwardsWriteDataNoData)
{
	TestForwardWriteData(nullptr, 0);
}

TEST_F(LoggerTest, ForwardsControlTransfer)
{
	unsigned char req_type(12), req(42);
	unsigned short val(17), idx(39);
	std::vector<unsigned char> data = { 3, 6, 9, 12, 15, 18, 21, 24 };

	EXPECT_CALL(_src, control_transfer(req_type, req, val, idx, data.data(), data.size()));

	_logger.control_transfer(req_type, req, val, idx, data.data(), data.size());
}
