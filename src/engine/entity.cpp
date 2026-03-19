#include "entity.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "../include.h"

float Entity2D::friction = .97;
unsigned Entity::currentId = 0;

Entity2D::Entity2D(const std::string& name, Vector2 position) : Entity(name), position(position) {
    tags = Tags::empty;
}

Entity::~Entity() {}

Entity::Entity(const std::string& name) : name(name), valid(true), id(currentId++) {
    tags = Tags::empty;
}

void Entity::addTag(Tags tag) {
    tags = (Tags)((uint64_t)tags | (uint64_t)tag);
}

void Entity::removeTag(Tags tag) {
    if(tags & tag) tags = (Tags)(tags ^ tag);
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

unsigned Entity::getId() const {
    return id;
}
