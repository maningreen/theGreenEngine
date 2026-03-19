#include "world.hpp"

#include <inttypes.h>

#include <vector>

template <typename T>
Listener<T>::Listener(unsigned id, std::function<void(Entity*, T)> callback)
  : callback(callback), id(id) {}

template <typename T>
Listener<T>::~Listener() {}

template <typename T>
bool Listener<T>::call(std::tuple<T> args) {
    if(auto entity = World::getEntity(id)) {
        std::apply(callback, std::tuple_cat(std::tuple<Entity*>(entity), args));
        return true;
    } else
        return false;
}

template <typename T>
Event<T>::Event() {}

template <typename T>
Event<T>::~Event() {}

template <typename T>
void Event<T>::call(std::tuple<T> args) {
    for(int i = 0; i < listeners.size(); i++) {
        const bool result = listeners[i].call(args);
        if(!result) {
            listeners[i] = listeners[listeners.size() - 1];
            listeners.pop_back();
        }
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

template <typename T>
void World::callEvent(std::string name, std::tuple<T> args) {
    const int count = world->events.count(name);
    if(count == 0)
        return;
    else
        world->events[name].call(args);
}
