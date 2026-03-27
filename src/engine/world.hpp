#ifndef WORLD_H
#define WORLD_H

#include <inttypes.h>

#include <cstdarg>
#include <functional>
#include <vector>

#include "entity.hpp"
#include "tags.hpp"

template <typename T>
struct Listener {
    void (*callback)(Entity*, T);
    const unsigned id;

    Listener<T>(unsigned id, void (*callback)(Entity*, T)) : callback(callback), id(id) {}
    ~Listener<T>() = default;

    Listener<T>& operator=(Listener<T>& a) {
        return a;
    }

    /// if fails to call returns false
    bool call(std::tuple<T>);
};

/// Dummy class for events
struct EventI {
    EventI() = default;
    ~EventI() = default;
};

template <typename T>
struct Event : EventI {
    std::vector<Listener<T>> listeners;

    Event<T>() {}
    ~Event<T>() {}

    void call(std::tuple<T> args) {
        for(int i = 0; i < listeners.size(); i++) {
            if(!listeners[i].call(args)) {
                listeners[i--] = listeners.back();
                listeners.pop_back();
            }
        }
    }
};

class World {
  private:
    World();
    ~World();

    static World* world;

  public:
    std::vector<Entity*> entities;
    std::unordered_map<std::string, EventI*> events;

    static void init();
    static void deinit();

    static World& get();

    static void process(float delta);
    static void render();

    static Entity* getEntity(unsigned);

    static void addEntity(Entity*);
    static std::vector<unsigned> getAllEntitiesWithTag(Tags x);

    template <typename T>
    static void callEvent(std::string name, std::tuple<T> args) {
        const int count = world->events.count(name);
        ((Event<T>*)world->events[name])->call(args);
    }

    template <typename T>
    static void createEvent(std::string name) {
        world->events.emplace(std::pair<std::string, EventI*>(name, new Event<T>()));
    }

    template <typename T>
    static void
    listenEvent(std::string eventName, void (*callback)(Entity*, T), const unsigned id) {
        auto t = world->events.find(eventName);
        if(t == world->events.end())
            createEvent<T>(eventName);
        ((Event<T>*)world->events[eventName])->listeners.push_back(Listener(id, callback));
    }
};

template <typename T>
bool Listener<T>::call(std::tuple<T> args) {
    if(auto entity = World::getEntity(id)) {
        std::apply(callback, std::tuple_cat(std::tuple<Entity*>(entity), args));
        return true;
    } else
        return false;
}

#endif
