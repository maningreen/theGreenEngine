#ifndef WORLD_H
#define WORLD_H

#include <inttypes.h>

#include <cstdarg>
#include <functional>
#include <vector>

#include "entity.hpp"
#include "tags.hpp"

template <typename T>
class Listener {
  public:
    std::function<void(Entity*, T)> callback;
    const unsigned id;
    Listener(unsigned id, std::function<void(Entity*, T)> callback);
    ~Listener();

    /// if fails to call returns false
    bool call(std::tuple<T> args);
};

template <typename T>
class Event {
  public:
    std::vector<Listener<T>> listeners;
    void call(std::tuple<T> args);
    Event();
    ~Event();
};

class World {
  private:
    World();
    ~World();

    static World* world;

  public:
    std::vector<Entity*> entities;
    std::unordered_map<std::string, Event<void>> events;

    static void init();
    static void deinit();

    static World& get();

    static void process(float delta);
    static void render();

    static Entity* getEntity(unsigned);

    static void addEntity(Entity*);
    static std::vector<unsigned> getAllEntitiesWithTag(Tags x);

    template <typename T>
    static void callEvent(std::string name, std::tuple<T> args);

    template <typename T>
    static void createEvent(std::string name, Event<T> event);

    template <typename T>
    static void
    listenEvent(std::string eventName, std::function<void(T)> callback, const unsigned id);
};

#endif
