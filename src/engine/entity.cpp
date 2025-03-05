#include "entity.hpp"
#include "../include.h"
#include <string>
#include <vector>
#include <iostream>

Entity* Entity::root = nullptr;

bool Entity::setRoot(Entity* r) {
  if(root != nullptr)
    return false;
  root = r;
  return true;
}

Entity* Entity::getRoot() {
  return root;
}

Entity2D::Entity2D(const std::string& name, Entity* par, Vector2 position) : Entity(name, par), Position(position) {
}

Entity2D::Entity2D(const std::string& name, Vector2 position) : Entity(name), Position(position) {
}

Entity::~Entity() {
}

Entity::Entity(const std::string& name, Entity* parent) : Parent(parent), Name(name), valid(true) {
}

Entity::Entity(const std::string& name) : Parent(nullptr), Name(name), valid(true) {
}

void Entity::addChild(Entity* child) {
  child->Parent = this;
  child->Init();
  Children.push_back(child);
}

void Entity::addTag(std::string tag) {
  Tags.push_back(tag);
}

Entity* Entity::getParent() {
  return Parent;
}

bool Entity::removeTag(std::string tag) {
  for(int i = 0; i < Tags.size(); i++)
    if(Tags[i] == tag) {
      Tags[i] = Tags.back();
      Tags.pop_back();
      return true;
    }
  return false;
}

bool Entity::hasTag(std::string tag) {
  for(int i = 0; i < Tags.size(); i++)
    if(Tags[i] == tag)
      return true;
  return false;
}

void Entity::printAllChildren() {
  //so, what we wants to do here is print a bunch of sthuff, in particular all of the children *recursively*
  std::cout << Name << '\n';
  for(Entity* child : Children) {
    child->printAllChildren();
  }
}

void Entity::kill() {
  Death();
  while(!Children.empty()) {
    Children.back()->kill();
    Children.pop_back();
  }
  delete this;
}
