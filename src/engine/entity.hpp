#ifndef entity_hpp
#define entity_hpp

#include <string>
#include <vector>

#include "../include.h"
#include "tags.hpp"

class Entity {
  private:
    static unsigned currentId;
    unsigned id;

  protected:
    bool valid;

  public:
    std::string name;
    Tags tags;

    Entity(const std::string& name);
    virtual ~Entity();

    unsigned getId() const;

    void addTag(Tags tag);
    void removeTag(Tags tag);
    bool hasTag(Tags tag) const;

    void killDefered();

    bool getValid() const;

    virtual void process(float delta) {};
    virtual void render() {};
    virtual void postProcessingRender() {};
    virtual void init() {};
    virtual void death() {};
};

class Entity2D : public Entity {
  public:
    Vector2 position;

    Entity2D(const std::string& name, Vector2 position);

    static float friction;
};

#endif
