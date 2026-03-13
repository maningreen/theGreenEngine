#ifndef WORLD_H
#define WORLD_H

#include <inttypes.h>
#include <cstdarg>

#include <vector>
#include <functional>

#include "entity.hpp"
#include "tags.hpp"

class Listener {
    // std::function<void(...)> callback;
    unsigned id;
    Listener(unsigned id, std::function<void(...)> callback);
    ~Listener();
};

template<typename T>
class Event {
    // std::vector<Listener<T>> listeners;
    void call(T args);
};

class EventWrapper {
};

class World {
  private:
    World();
    ~World();

    static World world;

  public:
    std::vector<Entity*> entities;
    static World& get();

    static void process(float delta);
    static void render();

    static Entity* getEntity(unsigned);

    static void addEntity(Entity*);
    static std::vector<unsigned> getAllEntitiesWithTag(Tags x);
    // static std::vector<Event*> call;
};

#endif
