#define BOOST_TEST_MODULE stack_queue_tests
#include <boost/test/included/unit_test.hpp>

#include "stack.hpp"
#include "queue.hpp"

#include <stdexcept>
#include <string>

BOOST_AUTO_TEST_CASE(stack_push_top_drop_int)
{
  matveev::Stack< int > s;

  BOOST_TEST(s.empty());
  BOOST_TEST(s.size() == 0);

  s.push(10);
  s.push(20);
  s.push(30);

  BOOST_TEST(!s.empty());
  BOOST_TEST(s.size() == 3);

  BOOST_CHECK_EQUAL(s.top(), 30);
  s.drop();

  BOOST_CHECK_EQUAL(s.top(), 20);
  s.drop();

  BOOST_CHECK_EQUAL(s.top(), 10);
  s.drop();

  BOOST_TEST(s.empty());
  BOOST_TEST(s.size() == 0);
}

BOOST_AUTO_TEST_CASE(stack_push_top_drop_string)
{
  matveev::Stack< std::string > s;

  s.push("a");
  s.push("b");

  BOOST_CHECK_EQUAL(s.top(), "b");
  s.drop();

  BOOST_CHECK_EQUAL(s.top(), "a");
  s.drop();

  BOOST_TEST(s.empty());
}

BOOST_AUTO_TEST_CASE(stack_empty_throws)
{
  matveev::Stack< int > s;

  BOOST_CHECK_THROW(s.top(), std::runtime_error);
  BOOST_CHECK_THROW(s.drop(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(queue_push_front_drop_int)
{
  matveev::Queue< int > q;

  BOOST_TEST(q.empty());
  BOOST_TEST(q.size() == 0);

  q.push(1);
  q.push(2);
  q.push(3);

  BOOST_TEST(!q.empty());
  BOOST_TEST(q.size() == 3);

  BOOST_CHECK_EQUAL(q.front(), 1);
  q.drop();

  BOOST_CHECK_EQUAL(q.front(), 2);
  q.drop();

  BOOST_CHECK_EQUAL(q.front(), 3);
  q.drop();

  BOOST_TEST(q.empty());
  BOOST_TEST(q.size() == 0);
}

BOOST_AUTO_TEST_CASE(queue_push_front_drop_string)
{
  matveev::Queue< std::string > q;

  q.push("one");
  q.push("two");

  BOOST_CHECK_EQUAL(q.front(), "one");
  q.drop();

  BOOST_CHECK_EQUAL(q.front(), "two");
  q.drop();

  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(queue_empty_throws)
{
  matveev::Queue< int > q;

  BOOST_CHECK_THROW(q.front(), std::runtime_error);
  BOOST_CHECK_THROW(q.drop(), std::runtime_error);
}
