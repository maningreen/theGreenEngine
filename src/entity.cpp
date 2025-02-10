#include "entity.hpp"
#include <raylib.h>
#include <string>

Entity2D::Entity2D(const std::string& name, Vector2 position) : Entity(name) {
  Name = name;
  Position = position;
}

Entity::~Entity() {
}

Entity::Entity(const std::string& name) {
  Name = name;
  entityArr = nullptr;
  valid = true;
}

void Entity::addChild(Entity* child) {
  Children.push_back(child);
  Children.back()->entityArr = &Children;
}

void Entity::addTag(std::string tag) {
  Tags.push_back(tag);
}
