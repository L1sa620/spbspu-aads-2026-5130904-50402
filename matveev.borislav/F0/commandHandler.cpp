#include "commandHandler.hpp"

#include "commands.hpp"
#include "parserUtils.hpp"

#include <exception>
#include <ostream>

namespace
{
void printHelp(std::ostream& out)
{
  out << "help\n";
  out << "locations\n";
  out << "make <location>\n";
  out << "show <location>\n";
  out << "delete <location>\n";
  out << "rename <location> <new-name>\n";
  out << "clear <location>\n";
  out << "add-item <location> <item> <type>\n";
  out << "delete-item <location> <item>\n";
  out << "rename-item <location> <item> <new-name>\n";
  out << "move-item <item> <from> <to>\n";
  out << "merge <new-location> <location-1> <location-2>\n";
  out << "find <item> <location>...\n";
  out << "filter <location> <type>\n";
  out << "connect <location-1> <location-2> <cost>\n";
  out << "disconnect <location-1> <location-2>\n";
  out << "path <from> <to>\n";
  out << "shortest-path <from> <to> <count> <item>...\n";
  out << "collect <from> <to> <count> <type>\n";
}

void printLocations(std::ostream& out, const matveev::World& world)
{
  const matveev::List< std::string >& order = world.order();
  bool first = true;

  for (matveev::LCIter< std::string > it = order.begin(); it != order.end(); ++it)
  {
    if (!first)
    {
      out << ", ";
    }

    out << *it;
    first = false;
  }

  out << '\n';
}
}

bool matveev::executeCommand(std::ostream& out, World& world, const List< std::string >& tokens)
{
  if (tokens.begin() == tokens.end())
  {
    return true;
  }

  LCIter< std::string > it = tokens.begin();
  const std::string& command = *it;

  if (command == HELP_COMMAND)
  {
    if (!hasArgCount(tokens, 1))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    printHelp(out);
    return true;
  }

  if (command == MAKE_COMMAND)
  {
    if (!hasArgCount(tokens, 2))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& name = *it;

    try
    {
      world.createLocation(name);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == LOCATIONS_COMMAND)
  {
    if (!hasArgCount(tokens, 1))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    printLocations(out, world);
    return true;
  }

  out << INVALID_COMMAND << '\n';
  return false;
}
