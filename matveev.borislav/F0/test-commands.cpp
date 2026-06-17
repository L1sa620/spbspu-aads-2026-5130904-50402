#include "commandHandler.hpp"
#include "parserUtils.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
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

  std::string run(const std::vector< std::string >& lines)
  {
    matveev::World world;
    std::ostringstream out;

    for (std::size_t i = 0; i < lines.size(); ++i)
    {
      matveev::List< std::string > tokens = matveev::splitLine(lines[i]);
      matveev::executeCommand(out, world, tokens);
    }

    return out.str();
  }

  bool contains(const std::string& haystack, const std::string& needle)
  {
    return haystack.find(needle) != std::string::npos;
  }

  std::string exec(matveev::World& world, const std::string& line)
  {
    std::ostringstream out;
    matveev::List< std::string > tokens = matveev::splitLine(line);
    matveev::executeCommand(out, world, tokens);
    return out.str();
  }
}

int main()
{
  check(run({ "make forest", "make dungeon", "locations" }) == "forest, dungeon\n", "locations: creation order");
  check(run({ "locations" }) == "\n", "locations: empty world prints blank line");
  check(run({ "make forest", "make forest" }) == "<INVALID COMMAND>\n", "make: duplicate is invalid");
  check(run({ "make forest dungeon" }) == "<INVALID COMMAND>\n", "make: wrong argument count is invalid");
  check(run({ "bogus" }) == "<INVALID COMMAND>\n", "unknown command is invalid");
  check(run({ "" }) == "", "empty line produces no output");

  std::string help = run({ "help" });
  check(contains(help, "make <location>") && contains(help, "shortest-path"), "help: lists commands");

  check(run({ "make a", "make b", "make c" }) == "", "make: success is silent");

  {
    matveev::World w;
    w.createLocation("forest");
    w.createLocation("dungeon");
    w.addItem("forest", "sword", "weapon");
    w.addItem("forest", "axe", "weapon");
    w.connect("forest", "dungeon", 10);
    check(exec(w, "show forest") == "Items: 2\n  sword (weapon)\n  axe (weapon)\nConnections: dungeon\n", "show: full format");
    check(exec(w, "show dungeon") == "Items: 0\nConnections: forest\n", "show: no items but a connection");
  }

  {
    matveev::World w;
    w.createLocation("empty");
    check(exec(w, "show empty") == "Items: 0\nConnections: none\n", "show: empty location");
    check(exec(w, "show ghost") == "Location not found\n", "show: missing location");
    check(exec(w, "show a b") == "<INVALID COMMAND>\n", "show: wrong arg count");
  }

  {
    matveev::World w;
    w.createLocation("doomed");
    check(exec(w, "delete doomed") == "", "delete: success is silent");
    check(exec(w, "show doomed") == "Location not found\n", "delete: location is gone");
    check(exec(w, "delete doomed") == "Location not found\n", "delete: missing location");
    check(exec(w, "delete a b") == "<INVALID COMMAND>\n", "delete: wrong arg count");
  }

  check(run({ "make a", "rename a x", "locations" }) == "x\n", "rename: success is silent, name changes");
  check(run({ "rename ghost y" }) == "Location not found\n", "rename: missing source");
  check(run({ "make a", "make b", "rename a b" }) == "<INVALID COMMAND>\n", "rename: target name taken is invalid");
  check(run({ "rename a" }) == "<INVALID COMMAND>\n", "rename: wrong arg count");

  {
    matveev::World w;
    w.createLocation("forest");
    w.addItem("forest", "sword", "weapon");
    w.addItem("forest", "axe", "weapon");
    check(exec(w, "clear forest") == "", "clear: success is silent");
    check(exec(w, "show forest") == "Items: 0\nConnections: none\n", "clear: items removed");
    check(exec(w, "clear ghost") == "Location not found\n", "clear: missing location");
    check(exec(w, "clear a b") == "<INVALID COMMAND>\n", "clear: wrong arg count");
  }

  std::cout << "\nALL " << passed << " CHECKS PASSED\n";
  return 0;
}
