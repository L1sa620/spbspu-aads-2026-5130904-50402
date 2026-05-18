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
  BOOST_TEST(static_cast< bool >(tree.begin() == tree.end()));
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

BOOST_AUTO_TEST_CASE(iteration_is_sorted)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(3, "three");
  tree.push(1, "one");
  tree.push(2, "two");

  matveev::BSTree< int, std::string >::Iterator it = tree.begin();

  BOOST_TEST(it->first == 1);
  ++it;
  BOOST_TEST(it->first == 2);
  ++it;
  BOOST_TEST(it->first == 3);
  ++it;
  BOOST_TEST(static_cast< bool >(it == tree.end()));
}

BOOST_AUTO_TEST_CASE(reverse_iteration)
{
  matveev::BSTree< int, int > tree;

  tree.push(2, 20);
  tree.push(1, 10);
  tree.push(3, 30);

  matveev::BSTree< int, int >::Iterator it = tree.end();

  --it;
  BOOST_TEST(it->first == 3);
  --it;
  BOOST_TEST(it->first == 2);
  --it;
  BOOST_TEST(it->first == 1);
}

BOOST_AUTO_TEST_CASE(find_existing_and_missing)
{
  matveev::BSTree< int, int > tree;

  tree.push(2, 20);
  tree.push(1, 10);
  tree.push(3, 30);

  BOOST_TEST(tree.find(2)->second == 20);
  BOOST_TEST(static_cast< bool >(tree.find(4) == tree.end()));
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
  BOOST_TEST(static_cast< bool >(tree.begin() == tree.end()));
}
