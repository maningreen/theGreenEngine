#include "entity.hpp"
#include "core.h"
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

void Engine::popEntityFromChildren(std::vector<Entity*>* children, int index, std::vector<Entity*>::iterator child) {
  if((*children)[index]->Children.size() != 0) {
    for(int i = 0; i < (*children)[index]->Children.size(); i++)
      Engine::popEntityFromChildren(&(*children)[i]->Children, i, children->begin());
  }
  children->erase(child);
}
