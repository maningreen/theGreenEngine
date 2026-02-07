#include "world.hpp"

#include <inttypes.h>

#include <vector>

World World::world;

World::World() {}
World::~World() {}

World& World::get() {
    return world;
}

void World::process(float delta) {
    for(int i = 0; i < world.entities.size(); i++) {
        Entity* en = world.entities[i];
        en->process(delta);
        if(!en->getValid()) {
            en->death();
            if(!en->getValid()) world.entities.erase(world.entities.begin() + i--);
        }
    }
}

void World::render() {
    for(Entity* en : world.entities) en->render();
}

Entity* World::getEntity(typeof(Entity::id) id) {
    for(int i = 0; i < world.entities.size(); i++)
        if(world.entities[i]->id == id) return world.entities[i];
    return nullptr;
}

void World::addEntity(Entity* en) {
    world.entities.push_back(en);
}

std::vector<unsigned> World::getAllEntitiesWithTag(Tags x) {
    std::vector<unsigned> ret;
    for(Entity* en : world.entities)
        if(en->hasTag(x)) ret.push_back(en->id);
    return ret;
}
