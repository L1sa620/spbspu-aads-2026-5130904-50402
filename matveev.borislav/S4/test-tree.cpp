#include <boost/test/unit_test.hpp>

#include "binary-search-tree.hpp"

#include <stdexcept>
#include <string>

BOOST_AUTO_TEST_CASE(empty_tree)
{
  matveev::BSTree< int, int > tree;

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(!tree.has(1));
}

BOOST_AUTO_TEST_CASE(push_has_and_at)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  BOOST_TEST(!tree.empty());
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(tree.has(1));
  BOOST_TEST(tree.has(2));
  BOOST_TEST(tree.has(3));
  BOOST_TEST(!tree.has(4));
  BOOST_TEST(tree.at(1) == "one");
  BOOST_TEST(tree.at(2) == "two");
  BOOST_TEST(tree.at(3) == "three");
}

BOOST_AUTO_TEST_CASE(push_duplicate_key)
{
  matveev::BSTree< int, int > tree;

  tree.push(1, 10);

  BOOST_CHECK_THROW(tree.push(1, 20), std::logic_error);
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.at(1) == 10);
}

BOOST_AUTO_TEST_CASE(at_missing_key)
{
  matveev::BSTree< int, int > tree;

  BOOST_CHECK_THROW(tree.at(1), std::logic_error);
}

BOOST_AUTO_TEST_CASE(clear_tree)
{
  matveev::BSTree< int, int > tree;

  tree.push(2, 20);
  tree.push(1, 10);
  tree.push(3, 30);

  tree.clear();

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(!tree.has(2));
}
