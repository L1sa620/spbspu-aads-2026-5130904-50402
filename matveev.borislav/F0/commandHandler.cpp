#include "commandHandler.hpp"

#include "commands.hpp"
#include "parserUtils.hpp"
#include "pathfinding.hpp"

#include <cstddef>
#include <exception>
#include <ostream>
#include <stdexcept>

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

void printShow(std::ostream& out, const matveev::World& world, const std::string& name)
{
  const matveev::Location& loc = world.location(name);

  std::size_t item_count = 0;
  for (matveev::LCIter< matveev::Item > it = loc.items.begin(); it != loc.items.end(); ++it)
  {
    ++item_count;
  }

  out << "Items: " << item_count << '\n';

  for (matveev::LCIter< matveev::Item > it = loc.items.begin(); it != loc.items.end(); ++it)
  {
    out << "  " << it->name << " (" << it->type << ")\n";
  }

  out << "Connections:";

  bool any = false;
  for (matveev::LCIter< matveev::Connection > it = loc.connections.begin(); it != loc.connections.end(); ++it)
  {
    out << ' ' << it->to;
    any = true;
  }

  if (!any)
  {
    out << " none";
  }

  out << '\n';
}

void printFilter(std::ostream& out, const matveev::World& world, const std::string& location, const std::string& type)
{
  const matveev::Location& loc = world.location(location);

  out << "Type " << type << ":";

  bool any = false;
  for (matveev::LCIter< matveev::Item > it = loc.items.begin(); it != loc.items.end(); ++it)
  {
    if (it->type == type)
    {
      out << (any ? ", " : " ") << it->name;
      any = true;
    }
  }

  if (!any)
  {
    out << " none";
  }

  out << '\n';
}

void printPath(std::ostream& out, const matveev::List< std::string >& path, unsigned long long cost)
{
  bool first = true;
  for (matveev::LCIter< std::string > it = path.begin(); it != path.end(); ++it)
  {
    if (!first)
    {
      out << " => ";
    }

    out << *it;
    first = false;
  }

  out << " (cost " << cost << ")\n";
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

  if (command == SHOW_COMMAND)
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
      printShow(out, world, name);
    }
    catch (const std::exception&)
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    return true;
  }

  if (command == DELETE_COMMAND)
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
      world.removeLocation(name);
    }
    catch (const std::exception&)
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    return true;
  }

  if (command == RENAME_COMMAND)
  {
    if (!hasArgCount(tokens, 3))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& name = *it;
    ++it;
    const std::string& new_name = *it;

    try
    {
      world.renameLocation(name, new_name);
    }
    catch (const std::out_of_range&)
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }
    catch (const std::logic_error&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == CLEAR_COMMAND)
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
      world.clearLocation(name);
    }
    catch (const std::exception&)
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    return true;
  }

  if (command == ADD_ITEM_COMMAND)
  {
    if (!hasArgCount(tokens, 4))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& location = *it;
    ++it;
    const std::string& item = *it;
    ++it;
    const std::string& type = *it;

    if (!world.hasLocation(location))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.addItem(location, item, type);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == DELETE_ITEM_COMMAND)
  {
    if (!hasArgCount(tokens, 3))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& location = *it;
    ++it;
    const std::string& item = *it;

    if (!world.hasLocation(location))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.removeItem(location, item);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == RENAME_ITEM_COMMAND)
  {
    if (!hasArgCount(tokens, 4))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& location = *it;
    ++it;
    const std::string& item = *it;
    ++it;
    const std::string& new_name = *it;

    if (!world.hasLocation(location))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.renameItem(location, item, new_name);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == MOVE_ITEM_COMMAND)
  {
    if (!hasArgCount(tokens, 4))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& item = *it;
    ++it;
    const std::string& from = *it;
    ++it;
    const std::string& to = *it;

    if (!world.hasLocation(from) || !world.hasLocation(to))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.moveItem(item, from, to);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == FIND_COMMAND)
  {
    if (countArgs(tokens) < 3)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& item = *it;
    ++it;

    for (LCIter< std::string > loc = it; loc != tokens.end(); ++loc)
    {
      if (!world.hasLocation(*loc))
      {
        out << LOCATION_NOT_FOUND << '\n';
        return false;
      }
    }

    std::string item_location;
    bool found = false;

    if (world.findItem(item, item_location))
    {
      for (LCIter< std::string > loc = it; loc != tokens.end(); ++loc)
      {
        if (*loc == item_location)
        {
          found = true;
          break;
        }
      }
    }

    if (found)
    {
      out << item_location << '\n';
    }
    else
    {
      out << "not found" << '\n';
    }

    return true;
  }

  if (command == FILTER_COMMAND)
  {
    if (!hasArgCount(tokens, 3))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& location = *it;
    ++it;
    const std::string& type = *it;

    if (!world.hasLocation(location))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    printFilter(out, world, location, type);
    return true;
  }

  if (command == CONNECT_COMMAND)
  {
    if (!hasArgCount(tokens, 4))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& first = *it;
    ++it;
    const std::string& second = *it;
    ++it;
    const std::string& cost_text = *it;

    if (!world.hasLocation(first) || !world.hasLocation(second))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    unsigned long long cost = 0;
    if (!parseUnsignedLongLong(cost_text, cost))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    try
    {
      world.connect(first, second, cost);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == DISCONNECT_COMMAND)
  {
    if (!hasArgCount(tokens, 3))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& first = *it;
    ++it;
    const std::string& second = *it;

    if (!world.hasLocation(first) || !world.hasLocation(second))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.disconnect(first, second);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == MERGE_COMMAND)
  {
    if (!hasArgCount(tokens, 4))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& new_name = *it;
    ++it;
    const std::string& first = *it;
    ++it;
    const std::string& second = *it;

    if (!world.hasLocation(first) || !world.hasLocation(second))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    try
    {
      world.mergeLocations(new_name, first, second);
    }
    catch (const std::exception&)
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    return true;
  }

  if (command == PATH_COMMAND)
  {
    if (!hasArgCount(tokens, 3))
    {
      out << INVALID_COMMAND << '\n';
      return false;
    }

    ++it;
    const std::string& from = *it;
    ++it;
    const std::string& to = *it;

    if (!world.hasLocation(from) || !world.hasLocation(to))
    {
      out << LOCATION_NOT_FOUND << '\n';
      return false;
    }

    List< std::string > path;
    unsigned long long cost = 0;

    if (!findShortestPath(world, from, to, path, cost))
    {
      out << "No path" << '\n';
      return false;
    }

    printPath(out, path, cost);
    return true;
  }

  out << INVALID_COMMAND << '\n';
  return false;
}
