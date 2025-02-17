#ifndef entity_hpp
#define entity_hpp

#include <raylib.h>
#include <string>
#include <vector>

class Entity {
  private:
    Entity* Parent;

  public:
    std::string Name;
    std::vector<Entity*> Children;
    std::vector<std::string> Tags;
    bool valid;

    Entity(const std::string& name, Entity* parent);
    Entity(const std::string& name);
    virtual ~Entity();

    void addChild(Entity* child);

    void addTag(std::string tag);
    bool removeTag(std::string tag);
    bool hasTag(std::string tag);

    Entity* getParent();
    Entity* getRoot();

    virtual void Process(float delta) {};
    virtual void Render() {};
    virtual void Init() {};
};

class Entity2D : public Entity {
  public:
    Vector2 Position;

    Entity2D(const std::string& name, Entity* Parent, Vector2 position);
    Entity2D(const std::string& name, Vector2 position);

    virtual void Process(float delta) {};
    virtual void Render() {};
    virtual void Init() {};
};

#endif
