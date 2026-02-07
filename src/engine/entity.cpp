#include "entity.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "../include.h"

float Entity2D::friction = .97;
unsigned Entity::currentId = 0;

Entity2D::Entity2D(const std::string& name, Entity* par, Vector2 position)
  : Entity(name, par), position(position) {}

Entity2D::Entity2D(const std::string& name, Vector2 position) : Entity(name), position(position) {}

Entity::~Entity() {}

Entity::Entity(const std::string& name, Entity* parent)
  : Parent(parent), name(name), valid(true), id(currentId++) {
    std::cout << id << '\n';
}

Entity::Entity(const std::string& name)
  : Parent(nullptr), name(name), valid(true), id(currentId++) {
    std::cout << id << '\n';
}

void Entity::addTag(Tags tag) {
    tags = tag;
}

Entity* Entity::getParent() const {
    return Parent;
}

void Entity::removeTag(Tags tag) {
    if(tags & tag)
        tags = (Tags)(tags ^ tag);
}

bool Entity::hasTag(Tags tag) const {
    return tags & tag;
}

bool Entity::getValid() const {
    return valid;
}

void Entity::killDefered() {
    valid = false;  // :p
}

Entity* Entity::getRoot() {
    if(Parent)
        return Parent->getRoot();
    else
        return this;
}
