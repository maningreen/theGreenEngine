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
    for(auto& item: events)
        delete item.second;
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
    unsigned low = 0;
    unsigned high = world->entities.size() - 1;
    while(low <= high) {
        unsigned avg = (high + low) / 2;
        const unsigned got = world->entities[avg]->getId();
        if(got < key) {
            low = avg + 1;
        } else if(got > key) {
            high = avg - 1;
        } else {
            return world->entities[avg];
            break;
        }
    }
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
