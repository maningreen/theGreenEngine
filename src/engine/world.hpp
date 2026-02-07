#ifndef WORLD_H
#define WORLD_H

#include <inttypes.h>
#include <vector>
#include "entity.hpp"
#include "tags.hpp"

class World {
  private:
    std::vector<Entity*> entities;

    World();
    ~World();

    static World world;

  public:
    static World& get();

    static void process(float delta);
    static void render();

    static Entity* getEntity(typeof(Entity::id));

    static void addEntity(Entity*);
    static std::vector<unsigned> getAllEntitiesWithTag(Tags x);
};

#endif
