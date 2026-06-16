#include "world.hpp"

#include <stdexcept>

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

matveev::World::World():
  locations_(16),
  order_()
{}

void matveev::World::swap(World& other) noexcept
{
  locations_.swap(other.locations_);
  order_.swap(other.order_);
}

bool matveev::World::hasLocation(const std::string& name) const
{
  return locations_.has(name);
}

const matveev::Location& matveev::World::location(const std::string& name) const
{
  return locations_.at(name);
}

const matveev::List< std::string >& matveev::World::order() const noexcept
{
  return order_;
}

void matveev::World::removeFromOrder(const std::string& name)
{
  LIter< std::string > prev = order_.beforeBegin();
  LIter< std::string > it = order_.begin();

  while (it != order_.end())
  {
    if (*it == name)
    {
      order_.eraseAfter(prev);
      return;
    }

    ++prev;
    ++it;
  }
}

void matveev::World::renameInOrder(const std::string& name, const std::string& new_name)
{
  for (LIter< std::string > it = order_.begin(); it != order_.end(); ++it)
  {
    if (*it == name)
    {
      *it = new_name;
      return;
    }
  }
}

void matveev::World::createLocation(const std::string& name)
{
  if (locations_.has(name))
  {
    throw std::logic_error("location already exists");
  }

  locations_.add(name, Location(name));
  appendBack(order_, name);
}

void matveev::World::removeLocation(const std::string& name)
{
  if (!locations_.has(name))
  {
    throw std::out_of_range("location not found");
  }

  removeFromOrder(name);
  locations_.drop(name);
}

void matveev::World::renameLocation(const std::string& name, const std::string& new_name)
{
  if (!locations_.has(name))
  {
    throw std::out_of_range("location not found");
  }

  if (locations_.has(new_name))
  {
    throw std::logic_error("target location already exists");
  }

  Location updated = locations_.at(name);
  updated.name = new_name;

  renameInOrder(name, new_name);
  locations_.drop(name);
  locations_.add(new_name, updated);
}
