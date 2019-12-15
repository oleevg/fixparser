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
    ProtocolParserTest():
    protocolParser(createFixMessageProcessor())
    {

    }

    static fixparser::core::FixMessageProcessor::Ptr createFixMessageProcessor()
    {
      auto orderBook = std::make_shared<fixparser::core::OrderBook>();

      return std::make_shared<fixparser::core::FixMessageProcessor>(orderBook);
    }

    void SetUp() override
    {
      log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("fixparser");
      log4cxx::BasicConfigurator::configure();
    }

  protected:
    fixparser::core::ProtocolParser protocolParser;
};

TEST_F(ProtocolParserTest, Should_handle_unsupported_message)
{
  std::stringstream inputStream("8=FIX.4.4^9=129^35=V^49=CTEST^56=DERIBITSERVER^34=3^52=20191129-18:21:30.484^55=BTC-PERPETUAL^262=203^263=1^264=20^265=1^267=3^269=0^269=1^269=2^10=244^");

  ASSERT_NO_THROW(protocolParser.run(inputStream));
}

TEST_F(ProtocolParserTest, Should_handle_supported_message)
{
  std::stringstream inputStream("8=FIX.4.4^9=209^35=X^49=DERIBITSERVER^56=CTEST^34=11^52=20191129-18:21:30.543^55=BTC-PERPETUAL^100087=26516750^100090=7761.3400^746=8316391.1000^262=203^268=1^279=1^269=1^270=7806.0000^271=7439.0000^272=20191129-18:21:30.526^10=127^");

  ASSERT_NO_THROW(protocolParser.run(inputStream));
}

