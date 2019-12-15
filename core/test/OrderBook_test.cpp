/*
 * OrderBook_test.cpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <gtest/gtest.h>

#include <core/OrderBook.hpp>

class OrderBookTest : public::testing::Test
{
  public:
    void SetUp() override
    {

    }

    void fillSellingItems()
    {
      fillItems(fixparser::core::OrderBookItemType::Sell);
    }

    void fillBuyingItems()
    {
      fillItems(fixparser::core::OrderBookItemType::Buy);
    }

  private:
    void fillItems(fixparser::core::OrderBookItemType type)
    {
      orderBook.addItem(type, {10.1, 10});
      orderBook.addItem(type, {9.1, 10});
      orderBook.addItem(type, {8.1, 10});
      orderBook.addItem(type, {3.1, 10});
      orderBook.addItem(type, {7.1, 10});
      orderBook.addItem(type, {6.1, 10});
      orderBook.addItem(type, {4.1, 10});
      orderBook.addItem(type, {5.1, 10});
      orderBook.addItem(type, {2.1, 10});
      orderBook.addItem(type, {1.1, 10});
    }

  protected:
    fixparser::core::OrderBook orderBook;
};

TEST_F(OrderBookTest, Should_return_lowest_selling_items)
{
  fillSellingItems();

  auto items = orderBook.getNthLowestSelling(3);

  // Assert
  ASSERT_EQ(items.size(), 3);
  ASSERT_FLOAT_EQ(items[0].price, 1.1);
  ASSERT_FLOAT_EQ(items[1].price, 2.1);
  ASSERT_FLOAT_EQ(items[2].price, 3.1);
}

TEST_F(OrderBookTest, Should_return_highest_buying_items)
{
  fillBuyingItems();

  auto items = orderBook.getNthHighestBuying(3);

  // Assert
  ASSERT_EQ(items.size(), 3);
  ASSERT_FLOAT_EQ(items[0].price, 10.1);
  ASSERT_FLOAT_EQ(items[1].price, 9.1);
  ASSERT_FLOAT_EQ(items[2].price, 8.1);
}

TEST_F(OrderBookTest, Should_delete_buying_items)
{
  fillBuyingItems();

  orderBook.deleteItem(fixparser::core::OrderBookItemType::Buy, {5.1, 10});
  auto items = orderBook.getNthHighestBuying(6);

  // Assert
  ASSERT_EQ(items.size(), 6);
  ASSERT_FLOAT_EQ(items[5].price, 4.1);
}

TEST_F(OrderBookTest, Should_change_buying_items)
{
  fillBuyingItems();

  orderBook.changeItem(fixparser::core::OrderBookItemType::Buy, {5.1, 100});
  auto items = orderBook.getNthHighestBuying(6);

  // Assert
  ASSERT_EQ(items.size(), 6);
  ASSERT_FLOAT_EQ(items[5].price, 5.1);
  ASSERT_EQ(items[5].quantity, 100);
}

