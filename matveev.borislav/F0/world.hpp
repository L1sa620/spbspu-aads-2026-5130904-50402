#ifndef MATVEEV_GAMEWORLD_WORLD_HPP
#define MATVEEV_GAMEWORLD_WORLD_HPP

#include "location.hpp"
#include "hashFunctions.hpp"
#include "hash-table.hpp"

#include "../common/list.hpp"

#include <string>

namespace matveev
{
class World
{
public:
  World();

  void swap(World& other) noexcept;

  bool hasLocation(const std::string& name) const;
  const Location& location(const std::string& name) const;
  const List< std::string >& order() const noexcept;

  void createLocation(const std::string& name);
  void removeLocation(const std::string& name);
  void renameLocation(const std::string& name, const std::string& new_name);

private:
  HashTable< std::string, Location, StringHash, StringEqual > locations_;
  List< std::string > order_;

  void removeFromOrder(const std::string& name);
  void renameInOrder(const std::string& name, const std::string& new_name);
};
}

#endif
