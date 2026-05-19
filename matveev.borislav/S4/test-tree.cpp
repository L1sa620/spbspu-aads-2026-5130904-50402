#include <boost/test/unit_test.hpp>

#include "binary-search-tree.hpp"

#include <stdexcept>
#include <string>
#include <utility>

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

  auto it = tree.begin();

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

  auto it = tree.end();

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

BOOST_AUTO_TEST_CASE(height_of_tree)
{
  matveev::BSTree< int, int > tree;

  BOOST_TEST(tree.height() == 0);

  tree.push(4, 40);
  tree.push(2, 20);
  tree.push(1, 10);
  tree.push(3, 30);
  tree.push(6, 60);
  tree.push(5, 50);

  BOOST_TEST(tree.height() == 3);
  BOOST_TEST(tree.height(tree.find(2)) == 2);
  BOOST_TEST(tree.height(tree.find(1)) == 1);
  BOOST_TEST(tree.height(tree.end()) == 0);
}

BOOST_AUTO_TEST_CASE(drop_leaf)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  BOOST_TEST(tree.drop(1) == "one");
  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(!tree.has(1));
  BOOST_TEST(tree.has(2));
  BOOST_TEST(tree.has(3));
}

BOOST_AUTO_TEST_CASE(drop_node_with_one_child)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(4, "four");

  BOOST_TEST(tree.drop(3) == "three");
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(!tree.has(3));
  BOOST_TEST(tree.has(4));
  BOOST_TEST(tree.find(4)->second == "four");
}

BOOST_AUTO_TEST_CASE(drop_node_with_two_children)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(4, "four");
  tree.push(2, "two");
  tree.push(6, "six");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(5, "five");
  tree.push(7, "seven");

  BOOST_TEST(tree.drop(4) == "four");
  BOOST_TEST(tree.size() == 6);
  BOOST_TEST(!tree.has(4));

  int expected = 1;

  for (auto it = tree.begin(); it != tree.end(); ++it)
  {
    if (expected == 4)
    {
      ++expected;
    }

    BOOST_TEST(it->first == expected);
    ++expected;
  }
}

BOOST_AUTO_TEST_CASE(copy_tree)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  matveev::BSTree< int, std::string > copy(tree);

  BOOST_TEST(copy.size() == 3);
  BOOST_TEST(copy.at(1) == "one");
  BOOST_TEST(copy.at(2) == "two");
  BOOST_TEST(copy.at(3) == "three");

  copy.drop(2);

  BOOST_TEST(tree.has(2));
  BOOST_TEST(!copy.has(2));
}

BOOST_AUTO_TEST_CASE(move_tree)
{
  matveev::BSTree< int, std::string > tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  matveev::BSTree< int, std::string > moved(std::move(tree));

  BOOST_TEST(moved.size() == 3);
  BOOST_TEST(moved.at(1) == "one");
  BOOST_TEST(moved.at(2) == "two");
  BOOST_TEST(moved.at(3) == "three");
  BOOST_TEST(tree.empty());
}

BOOST_AUTO_TEST_CASE(swap_trees)
{
  matveev::BSTree< int, int > lhs;
  matveev::BSTree< int, int > rhs;

  lhs.push(1, 10);
  rhs.push(2, 20);
  rhs.push(3, 30);

  lhs.swap(rhs);

  BOOST_TEST(lhs.size() == 2);
  BOOST_TEST(rhs.size() == 1);
  BOOST_TEST(lhs.has(2));
  BOOST_TEST(lhs.has(3));
  BOOST_TEST(rhs.has(1));
}

BOOST_AUTO_TEST_CASE(left_rotation)
{
  matveev::BSTree< int, int > tree;

  tree.push(10, 10);
  tree.push(5, 5);
  tree.push(15, 15);
  tree.push(12, 12);

  auto lowered = tree.rotateLeft(tree.find(15));

  BOOST_TEST(lowered->first == 10);
  BOOST_TEST(tree.begin()->first == 5);
  BOOST_TEST(static_cast< bool >(tree.find(15) != tree.end()));
  BOOST_TEST(tree.height() == 3);
}

BOOST_AUTO_TEST_CASE(right_rotation)
{
  matveev::BSTree< int, int > tree;

  tree.push(10, 10);
  tree.push(5, 5);
  tree.push(15, 15);
  tree.push(7, 7);

  auto lowered = tree.rotateRight(tree.find(5));

  BOOST_TEST(lowered->first == 10);
  BOOST_TEST(tree.begin()->first == 5);
  BOOST_TEST(static_cast< bool >(tree.find(5) != tree.end()));
  BOOST_TEST(tree.height() == 3);
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
