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

  std::vector< std::string > itemNames(const matveev::World& world, const std::string& location)
  {
    std::vector< std::string > result;
    const matveev::Location& loc = world.location(location);
    for (matveev::LCIter< matveev::Item > it = loc.items.begin(); it != loc.items.end(); ++it)
    {
      result.push_back(it->name);
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

  {
    matveev::World w;
    w.createLocation("forest");
    w.createLocation("cave");
    w.addItem("forest", "sword", "weapon");
    w.addItem("forest", "axe", "weapon");
    check(itemNames(w, "forest") == std::vector< std::string >({ "sword", "axe" }), "add-item: order preserved");
    check(throws([&]{ w.addItem("cave", "sword", "tool"); }), "add-item: globally duplicate name throws");
    check(throws([&]{ w.addItem("nowhere", "bow", "weapon"); }), "add-item: missing location throws");

    std::string where;
    check(w.findItem("sword", where) && where == "forest", "find: locates item globally");
    check(!w.findItem("ghost", where), "find: missing item returns false");
  }

  {
    matveev::World w;
    w.createLocation("a");
    w.createLocation("b");
    w.addItem("a", "sword", "weapon");

    w.renameItem("a", "sword", "blade");
    std::string where;
    check(w.findItem("blade", where) && where == "a" && !w.findItem("sword", where), "rename-item: index updated");
    check(throws([&]{ w.renameItem("a", "ghost", "x"); }), "rename-item: missing item throws");

    w.moveItem("blade", "a", "b");
    check(itemNames(w, "a").empty() && itemNames(w, "b") == std::vector< std::string >({ "blade" }), "move-item: moved between lists");
    check(w.findItem("blade", where) && where == "b", "move-item: index points to new location");
    check(throws([&]{ w.moveItem("blade", "a", "b"); }), "move-item: missing source item throws");

    w.removeItem("b", "blade");
    check(itemNames(w, "b").empty() && !w.findItem("blade", where), "remove-item: gone from list and index");
    check(throws([&]{ w.removeItem("b", "blade"); }), "remove-item: missing throws");
  }

  {
    matveev::World w;
    w.createLocation("a");
    w.addItem("a", "sword", "weapon");
    w.addItem("a", "axe", "weapon");
    w.clearLocation("a");
    check(itemNames(w, "a").empty(), "clear: items removed");
    std::string where;
    check(!w.findItem("sword", where) && !w.findItem("axe", where), "clear: index updated");
    w.addItem("a", "sword", "weapon");
    check(w.findItem("sword", where), "clear: name reusable after clear");
  }

  {
    matveev::World w;
    w.createLocation("vault");
    w.addItem("vault", "relic", "treasure");
    w.removeLocation("vault");
    std::string where;
    check(!w.findItem("relic", where), "remove-location: items dropped from index");
    w.createLocation("other");
    w.addItem("other", "relic", "treasure");
    check(w.findItem("relic", where) && where == "other", "remove-location: freed item name reusable");
  }

  {
    matveev::World w;
    w.createLocation("a");
    w.addItem("a", "sword", "weapon");
    w.renameLocation("a", "x");
    std::string where;
    check(w.findItem("sword", where) && where == "x", "rename-location: item index follows the new name");
  }

  std::cout << "\nALL " << passed << " CHECKS PASSED\n";
  return 0;
}
