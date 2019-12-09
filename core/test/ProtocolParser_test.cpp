/*
 * ProtocolParser_test.cpp
 *
 *  Created on: 12/4/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <log4cxx/basicconfigurator.h>

#include <gtest/gtest.h>

#include <core/ProtocolParser.hpp>

class ProtocolParserTest : public testing::Test
{
  public:
    void SetUp() override
    {
      log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("fixparser");
      log4cxx::BasicConfigurator::configure();
    }
};

TEST_F(ProtocolParserTest, Should_run)
{
  std::stringstream inputStream("8=FIX.4.4^9=129^35=V^49=CTEST^56=DERIBITSERVER^34=3^52=20191129-18:21:30.484^55=BTC-PERPETUAL^262=203^263=1^264=20^265=1^267=3^269=0^269=1^269=2^10=244^");

  fixparser::core::ProtocolParser protocolParser;
  protocolParser.run(inputStream, '=', '^');

  ASSERT_TRUE( 1 == 1);
}

