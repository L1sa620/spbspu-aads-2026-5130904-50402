#include "world.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
  int passed = 0;

  void check(bool condition, const char* name)
  {
    if (!condition)
    {
      std::cout << "FAIL: " << name << '\n';
      std::exit(1);
    }

    ++passed;
    std::cout << "ok: " << name << '\n';
  }

  std::vector< std::string > orderVec(const matveev::World& world)
  {
    std::vector< std::string > result;
    const matveev::List< std::string >& order = world.order();
    for (matveev::LCIter< std::string > it = order.begin(); it != order.end(); ++it)
    {
      result.push_back(*it);
    }
    return result;
  }

  template< class F >
  bool throws(F f)
  {
    try
    {
      f();
    }
    catch (const std::exception&)
    {
      return true;
    }
    return false;
  }
}

int main()
{
  {
    matveev::World w;
    w.createLocation("forest");
    w.createLocation("dungeon");
    check(w.hasLocation("forest") && w.hasLocation("dungeon"), "create: both exist");
    check(!w.hasLocation("cave"), "create: absent reported absent");
    check(orderVec(w) == std::vector< std::string >({ "forest", "dungeon" }), "create: creation order preserved");
    check(throws([&]{ w.createLocation("forest"); }), "create: duplicate throws");
  }

  {
    matveev::World w;
    w.createLocation("a");
    w.createLocation("b");
    w.createLocation("c");
    w.removeLocation("b");
    check(!w.hasLocation("b") && w.hasLocation("a") && w.hasLocation("c"), "remove: target gone, others kept");
    check(orderVec(w) == std::vector< std::string >({ "a", "c" }), "remove: order updated");
    check(throws([&]{ w.removeLocation("b"); }), "remove: missing throws");
  }

  {
    matveev::World w;
    w.createLocation("a");
    w.createLocation("b");
    w.renameLocation("a", "x");
    check(w.hasLocation("x") && !w.hasLocation("a"), "rename: renamed");
    check(orderVec(w) == std::vector< std::string >({ "x", "b" }), "rename: order updated in place");
    check(throws([&]{ w.renameLocation("missing", "y"); }), "rename: missing source throws");
    check(throws([&]{ w.renameLocation("b", "x"); }), "rename: existing target throws");
  }

  {
    matveev::World w;
    w.createLocation("solo");
    check(w.location("solo").name == "solo", "location: accessor returns the location");
    check(throws([&]{ w.location("ghost"); }), "location: missing throws");
  }

  std::cout << "\nALL " << passed << " CHECKS PASSED\n";
  return 0;
}
