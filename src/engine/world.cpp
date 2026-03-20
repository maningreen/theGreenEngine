#include "world.hpp"

#include <inttypes.h>

#include <vector>

Listener::Listener(unsigned id, void (*callback)(Entity*, void*)) : callback(callback), id(id) {}

Listener::~Listener() {}

bool Listener::call(void* args) {
    if(auto entity = World::getEntity(id)) {
        callback(entity, args);
        return true;
    } else
        return false;
}

Event::Event() {}

Event::~Event() {}

void Event::call(void* args) {
    for(Listener& listener: listeners) {
        listener.call(args);
    }
}

World* World::world = nullptr;

World::World() {}
World::~World() {
    for(Entity* en : entities) {
        en->death();
        delete en;
    }
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

Entity* World::getEntity(unsigned id) {
    for(int i = 0; i < world->entities.size(); i++)
        if(world->entities[i]->getId() == id) return world->entities[i];
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

void World::createEvent(std::string name, Event event) {
    world->events.emplace(std::pair<std::string, Event>(name, Event()));
}

void World::listenEvent(std::string eventName, void (*callback)(Entity*, void*), const unsigned id) {
    auto it = world->events.find(eventName);
    it->second.listeners.push_back(Listener(id, callback));
}

void World::callEvent(std::string name, void* args) {
    const int count = world->events.count(name);
    world->events[name].call(args);
}
