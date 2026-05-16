#include <boost/test/unit_test.hpp>

#include "binary-search-tree.hpp"

BOOST_AUTO_TEST_CASE(empty_tree)
{
  matveev::BSTree< int, int > tree;

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(!tree.has(1));
}
