#include "entity.hpp"
#include "core.h"
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>
#include <iostream>

bool checkCollisionCircleTriangle(Vector2 circleCenter, float radius, Vector2 triangleVertexA, Vector2 triangleVertexB, Vector2 triangleVertexC) {
  // get the average point in the triangle
  Vector2 trigCenter = Vector2Scale(Vector2Add(triangleVertexA, Vector2Add(triangleVertexB, triangleVertexC)), 1.0f/3.0f);
  // then we get the vertex from the center of the circle to the trigCenter
  Vector2 vecTo = Vector2Normalize(Vector2Subtract(trigCenter, circleCenter));
  return CheckCollisionPointTriangle(Vector2Add(Vector2Scale(vecTo, radius), circleCenter), triangleVertexA, triangleVertexB, triangleVertexC);
}

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
