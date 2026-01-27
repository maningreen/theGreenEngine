#include "entity.hpp"
#include "core.h"
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>

int Engine::getEntityIndex(std::vector<Entity*> entities, Entity* entity) {
  int i = 0;
  for(Entity* item : entities)
    if(item == entity)
      return i;
  return -1;
}

Entity* Engine::searchTreeForEntity(std::vector<Entity*>* entities, std::string name) {
  if(entities == nullptr)
    return nullptr;
  Entity* out;
  out = getFirstEntityIndexWithName(*entities, name);
  if(out != nullptr)
    return out;
  for(Entity* item : *entities) {
    out = searchTreeForEntity(&item->children, name);
    if(out != nullptr)
      return out;
  }
  return nullptr;
}

std::vector<Entity*> Engine::getAllChildrenWithTag(Entity* en, std::string tag) {
  std::vector<Entity*> out;
  for(Entity* child : en->children) {
    if(entityHasTag(child, tag))
      out.push_back(child);
  }
  return out;
}

std::vector<Entity*> Engine::getAllChildrenWithTagRecursive(Entity* en, std::string tag) {
  std::vector<Entity*> out;
  for(Entity* child : en->children) {
    std::vector<Entity*> buff = getAllChildrenWithTagRecursive(child, tag);
    if(child->hasTag(tag))
      buff.push_back(child);
    out.insert(out.end(), buff.begin(), buff.end());
  }
  return out;
}

Entity* Engine::getFirstEntityIndexWithName(std::vector<Entity*> entities, std::string name) {
  for(Entity* item : entities)
    if(item->name == name)
      return item;
  return nullptr;
}

void Engine::addEntity(std::vector<Entity*>* entities, Entity* entity) {
  entities->push_back(entity);
}

bool Engine::entityHasTag(Entity* entity, std::string tag) {
  for(std::string& item : entity->tags)
    if(item == tag)
      return true;
  return false;
}
