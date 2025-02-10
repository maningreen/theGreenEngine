#ifndef entity_hpp
#define entity_hpp

#include <raylib.h>
#include <string>
#include <vector>

class Entity {
  public:
    std::string Name;
    std::vector<Entity*> Children;
    std::vector<Entity*>* entityArr;
    std::vector<std::string> Tags;
    bool valid;

    Entity(const std::string& name);
    virtual ~Entity();

    void addChild(Entity* child);

    void addTag(std::string tag);

    virtual void Process(float delta) {};
    virtual void Render() {};
};

class Entity2D : public Entity {
  public:
    Vector2 Position;

    Entity2D(const std::string& name, Vector2 position);

    virtual void Process(float delta) {};
    virtual void Render() {};
};

#endif
