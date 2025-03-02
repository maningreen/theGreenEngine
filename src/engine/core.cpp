#include "entity.hpp"
#include "core.h"
#include <string>
#include <vector>
#include <iostream>

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
    out = searchTreeForEntity(&item->Children, name);
    if(out != nullptr)
      return out;
  }
  return nullptr;
}

std::vector<Entity*> Engine::getAllChildrenWithTag(Entity* en, std::string tag) {
  std::vector<Entity*> out;
  for(Entity* child : en->Children) {
    if(entityHasTag(child, tag))
      out.push_back(child);
  }
  return out;
}

std::vector<Entity*> Engine::getAllChildrenWithTagRecursive(Entity* en, std::string tag) {
  std::vector<Entity*> out;
  for(Entity* child : en->Children) {
    std::vector<Entity*> buff = getAllChildrenWithTagRecursive(child, tag);
    out.insert(out.end(), buff.begin(), buff.end());
  }
  return out;
}

Entity* Engine::getFirstEntityIndexWithName(std::vector<Entity*> entities, std::string name) {
  for(Entity* item : entities)
    if(item->Name == name)
      return item;
  return nullptr;
}

void Engine::addEntity(std::vector<Entity*>* entities, Entity* entity) {
  entities->push_back(entity);
}

bool Engine::entityHasTag(Entity* entity, std::string tag) {
  for(std::string item : entity->Tags)
    if(item == tag)
      return true;
  return false;
}

void Engine::killEntity(Entity* en) {
  std::vector<Entity*> kids(en->Children);
  delete en;
  while(!kids.empty()) {
    killEntity(kids.back());
    kids.pop_back();
  }
}
