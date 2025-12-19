#include "entity.hpp"
#include "../include.h"
#include <iostream>
#include <string>
#include <vector>

Entity* Entity::root = nullptr;
float Entity2D::friction = .97;

bool Entity::setRoot(Entity* r) {
  if(root != nullptr)
    return false;
  root = r;
  return true;
}

Entity* Entity::getRoot() {
  return root;
}

Entity2D::Entity2D(const std::string& name, Entity* par, Vector2 position) : Entity(name, par), position(position) {}

Entity2D::Entity2D(const std::string& name, Vector2 position) : Entity(name), position(position) {}

Entity::~Entity() {}

Entity::Entity(const std::string& name, Entity* parent) : Parent(parent), name(name), valid(true) {}

Entity::Entity(const std::string& name) : Parent(nullptr), name(name), valid(true) {
}

void Entity::addChild(Entity* child) {
  child->Parent = this;
  children.push_back(child);
  child->init();
}

void Entity::addTag(std::string tag) {
  tags.push_back(tag);
}

Entity* Entity::getParent() {
  return Parent;
}

bool Entity::removeTag(std::string tag) {
  for(int i = 0; i < tags.size(); i++)
    if(tags[i] == tag) {
      tags[i] = tags.back();
      tags.pop_back();
      return true;
    }
  return false;
}

bool Entity::hasTag(std::string tag) {
  for(int i = 0; i < tags.size(); i++)
    if(tags[i] == tag)
      return true;
  return false;
}

void Entity::printAllChildren() {
  //so, what we wants to do here is print a bunch of sthuff, in particular all of the children *recursively*
  for(Entity* child : children)
    child->printAllChildren();
}

void Entity::kill() {
  death();

  if(valid) return; // maybe revived itself

  int len = children.size();
  for(int i = 0; i < len; i++) {
    children.back()->kill();

    if(children.back()->getValid()) delete children.back();

    children.pop_back();
  }
  delete this;
}

bool Entity::getValid() {
  return valid;
}

void Entity::killDefered() {
  valid = false; // :p
}
