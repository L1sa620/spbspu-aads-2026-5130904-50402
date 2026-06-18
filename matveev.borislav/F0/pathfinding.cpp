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

bool matveev::findItemRoute(const World& world, const std::string& from, const std::string& to,
  const List< std::string >& items, List< std::string >& path_out, unsigned long long& cost_out)
{
  const unsigned long long infinity = std::numeric_limits< unsigned long long >::max();
  const std::size_t none = std::numeric_limits< std::size_t >::max();
  const std::size_t one = 1;

  HashTable< std::string, char, StringHash, StringEqual > seen(16);
  Array< std::string > target_names;

  for (LCIter< std::string > it = items.begin(); it != items.end(); ++it)
  {
    std::string location;
    if (!world.findItem(*it, location))
    {
      return false;
    }

    if (!seen.has(location))
    {
      seen.add(location, 0);
      target_names.pushBack(location);
    }
  }

  std::size_t m = target_names.size();

  if (m == 0)
  {
    return findShortestPath(world, from, to, path_out, cost_out);
  }

  Array< unsigned long long > cost_s(m, infinity);
  Array< unsigned long long > cost_e(m, infinity);
  Array< unsigned long long > cost_t(m * m, infinity);

  for (std::size_t i = 0; i < m; ++i)
  {
    List< std::string > leg;
    unsigned long long c = 0;
    if (findShortestPath(world, from, target_names[i], leg, c))
    {
      cost_s[i] = c;
    }

    List< std::string > leg_e;
    unsigned long long ce = 0;
    if (findShortestPath(world, target_names[i], to, leg_e, ce))
    {
      cost_e[i] = ce;
    }

    for (std::size_t j = 0; j < m; ++j)
    {
      if (i == j)
      {
        cost_t[i * m + j] = 0;
        continue;
      }

      List< std::string > leg_t;
      unsigned long long ct = 0;
      if (findShortestPath(world, target_names[i], target_names[j], leg_t, ct))
      {
        cost_t[i * m + j] = ct;
      }
    }
  }

  std::size_t num_masks = one << m;
  Array< unsigned long long > dp(num_masks * m, infinity);
  Array< std::size_t > parent(num_masks * m, none);

  for (std::size_t i = 0; i < m; ++i)
  {
    if (cost_s[i] != infinity)
    {
      dp[(one << i) * m + i] = cost_s[i];
    }
  }

  for (std::size_t mask = 1; mask < num_masks; ++mask)
  {
    for (std::size_t i = 0; i < m; ++i)
    {
      if ((mask & (one << i)) == 0)
      {
        continue;
      }

      unsigned long long current = dp[mask * m + i];
      if (current == infinity)
      {
        continue;
      }

      for (std::size_t j = 0; j < m; ++j)
      {
        if ((mask & (one << j)) != 0)
        {
          continue;
        }

        unsigned long long edge = cost_t[i * m + j];
        if (edge == infinity)
        {
          continue;
        }

        std::size_t next_mask = mask | (one << j);
        unsigned long long candidate = current + edge;
        if (candidate < dp[next_mask * m + j])
        {
          dp[next_mask * m + j] = candidate;
          parent[next_mask * m + j] = i;
        }
      }
    }
  }

  std::size_t full = num_masks - 1;
  unsigned long long best = infinity;
  std::size_t best_end = none;

  for (std::size_t i = 0; i < m; ++i)
  {
    if (dp[full * m + i] == infinity || cost_e[i] == infinity)
    {
      continue;
    }

    unsigned long long total = dp[full * m + i] + cost_e[i];
    if (total < best)
    {
      best = total;
      best_end = i;
    }
  }

  if (best_end == none)
  {
    return false;
  }

  cost_out = best;

  Array< std::size_t > order;
  std::size_t mask = full;
  std::size_t node = best_end;
  while (true)
  {
    order.pushBack(node);
    std::size_t previous = parent[mask * m + node];
    if (previous == none)
    {
      break;
    }

    mask ^= (one << node);
    node = previous;
  }

  Array< std::string > sequence;
  sequence.pushBack(from);
  for (std::size_t k = order.size(); k-- > 0; )
  {
    sequence.pushBack(target_names[order[k]]);
  }
  sequence.pushBack(to);

  for (std::size_t k = 0; k + 1 < sequence.size(); ++k)
  {
    List< std::string > leg;
    unsigned long long leg_cost = 0;
    findShortestPath(world, sequence[k], sequence[k + 1], leg, leg_cost);

    LIter< std::string > p = leg.begin();
    if (k != 0 && p != leg.end())
    {
      ++p;
    }

    for (; p != leg.end(); ++p)
    {
      appendBack(path_out, *p);
    }
  }

  return true;
}
