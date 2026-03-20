#ifndef WORLD_H
#define WORLD_H

#include <inttypes.h>

#include <cstdarg>
#include <functional>
#include <vector>

#include "entity.hpp"
#include "tags.hpp"

struct Listener {
    void (*callback)(Entity*, void*);
    const unsigned id;

    Listener(unsigned id, void (*callback)(Entity*, void*));
    ~Listener();

    Listener operator=(Listener& a);

    /// if fails to call returns false
    bool call(void*);
};

struct Event {
    std::vector<Listener> listeners;

    Event();
    ~Event();

    void call(void*);
};

class World {
  private:
    World();
    ~World();

    static World* world;

  public:
    std::vector<Entity*> entities;
    std::unordered_map<std::string, Event> events;

    static void init();
    static void deinit();

    static World& get();

    static void process(float delta);
    static void render();

    static Entity* getEntity(unsigned);

    static void addEntity(Entity*);
    static std::vector<unsigned> getAllEntitiesWithTag(Tags x);

    static void callEvent(std::string name, void* args);

    static void createEvent(std::string name, Event event);

    static void
    listenEvent(std::string eventName, void (*callback)(Entity*, void*), const unsigned id);
};

#endif
