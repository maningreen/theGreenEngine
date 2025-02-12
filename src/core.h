#ifndef core_h
#define core_h

#include "entity.hpp"
#include <string>
#include <vector>

namespace Engine {
  //returns -1 if no such entity is in the vector
  int getEntityIndex(std::vector<Entity*> entities, Entity* entity);
  Entity* getFirstEntityIndexWithName(std::vector<Entity*> entities, std::string name);
  Entity* searchTreeForEntity(std::vector<Entity*>* entities, std::string name);
  void addEntity(std::vector<Entity*>* entities, Entity* entity);
  bool entityHasTag(Entity* en, std::string tag);
  void popEntityFromChildren(std::vector<Entity*>* children, int index, std::vector<Entity*>::iterator child);
}

#endif
