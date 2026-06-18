#include "pathfinding.hpp"

#include "array.hpp"
#include "hash-table.hpp"
#include "hashFunctions.hpp"
#include "location.hpp"

#include <limits>

namespace
{
template< class T >
void appendBack(matveev::List< T >& list, const T& value)
{
  matveev::LIter< T > prev = list.beforeBegin();
  matveev::LIter< T > it = list.begin();

  while (it != list.end())
  {
    ++prev;
    ++it;
  }

  list.insertAfter(prev, value);
}
}

bool matveev::findShortestPath(const World& world, const std::string& from, const std::string& to,
  List< std::string >& path_out, unsigned long long& cost_out)
{
  const unsigned long long infinity = std::numeric_limits< unsigned long long >::max();
  const std::size_t none = std::numeric_limits< std::size_t >::max();

  HashTable< std::string, std::size_t, StringHash, StringEqual > index_of(16);
  Array< std::string > names;

  std::size_t count = 0;
  for (LCIter< std::string > it = world.order().begin(); it != world.order().end(); ++it)
  {
    index_of.add(*it, count);
    names.pushBack(*it);
    ++count;
  }

  std::size_t from_index = index_of.at(from);
  std::size_t to_index = index_of.at(to);

  Array< unsigned long long > dist(count, infinity);
  Array< bool > visited(count, false);
  Array< std::size_t > previous(count, none);
  dist[from_index] = 0;

  for (std::size_t step = 0; step < count; ++step)
  {
    std::size_t current = none;
    unsigned long long best = infinity;

    for (std::size_t i = 0; i < count; ++i)
    {
      if (!visited[i] && dist[i] < best)
      {
        best = dist[i];
        current = i;
      }
    }

    if (current == none)
    {
      break;
    }

    visited[current] = true;

    const Location& loc = world.location(names[current]);
    for (LCIter< Connection > c = loc.connections.begin(); c != loc.connections.end(); ++c)
    {
      std::size_t neighbour = index_of.at(c->to);
      if (!visited[neighbour] && dist[current] + c->cost < dist[neighbour])
      {
        dist[neighbour] = dist[current] + c->cost;
        previous[neighbour] = current;
      }
    }
  }

  if (dist[to_index] == infinity)
  {
    return false;
  }

  Array< std::size_t > reverse_path;
  std::size_t cursor = to_index;
  reverse_path.pushBack(cursor);

  while (cursor != from_index)
  {
    cursor = previous[cursor];
    reverse_path.pushBack(cursor);
  }

  for (std::size_t k = reverse_path.size(); k-- > 0; )
  {
    appendBack(path_out, names[reverse_path[k]]);
  }

  cost_out = dist[to_index];
  return true;
}
