#include "world.hpp"

#include <inttypes.h>

#include <iostream>
#include <optional>
#include <vector>

World* World::world = nullptr;

World::World() {}
World::~World() {
    for(Entity* en : entities) {
        en->death();
        delete en;
    }
    for(auto& item : events) delete(Event<void*>*)item.second;
}

void World::init() {
    World::world = new World();
}

void World::deinit() {
    delete world;
}

World& World::get() {
    return *world;
}

void World::process(float delta) {
    for(int i = 0; i < world->entities.size(); i++) {
        Entity* en = world->entities[i];
        en->process(delta);
        if(!en->getValid()) {
            en->death();
            if(!en->getValid()) {
                delete en;
                world->entities.erase(world->entities.begin() + i--);
            }
        }
    }
}

void World::render() {
    for(Entity* en : world->entities) en->render();
}

Entity* World::getEntity(unsigned key) {
    for(Entity* en : world->entities)
        if(en->getId() == key)
            return en;
    return nullptr;
}

void World::addEntity(Entity* en) {
    world->entities.push_back(en);
}

std::vector<unsigned> World::getAllEntitiesWithTag(Tags x) {
    std::vector<unsigned> ret;
    for(Entity* en : world->entities)
        if(en->hasTag(x)) ret.push_back(en->getId());
    return ret;
}
