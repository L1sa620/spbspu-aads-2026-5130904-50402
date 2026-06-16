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

  std::cout << "\nALL " << passed << " CHECKS PASSED\n";
  return 0;
}
