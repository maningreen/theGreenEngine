#ifndef inputManager_h
#define inputManager_h

#include "engine/entity.hpp"
#include <functional>
#include <variant>
#include <vector>

// A type representing a keybind
struct keybind {
  // the key on which to call callback
  Key key;
  // the function to be called on key press
  std::function<void(void)> callback;
  // wether or not it's a mouse button
  bool mouseButton;

  // a simple constructor (default not a mouse button)
  keybind(int key, std::function<void(void)> callback);
  keybind(int key, bool isMouseButton, std::function<void(void)> callback);
};

// called with the input vector 
struct keybindAlt {
  // the key on which to call callback
  Key key;
  // the function to be called on key press
  std::function<void(Vector2)> callback;
  // wether or not it's a mouse button
  bool mouseButton;

  // a simple constructor (default not a mouse button)
  keybindAlt(int key, std::function<void(Vector2)> callback);
  keybindAlt(int key, bool isMouseButton, std::function<void(Vector2)> callback);
};

typedef std::vector<std::variant<keybind, keybindAlt>> keybinds;

class InputManager : public Entity {
  public:
    keybinds binds;

    // binds of which to call on an input-vector input
    std::vector<std::function<void(float, Vector2)>> vectorBinds;

    void Process(float delta) override;

    void addBind(const keybind& bind);
    void addBind(const keybindAlt& bind);

    void addVectorBind(std::function<void(float, Vector2)> bind);
    void removeBind(int index);
    void removeVectorBind(int index);

    Key up;
    Key down;
    Key left;
    Key right;

    InputManager(Key up, Key down, Key left, Key right, keybinds binds);
    InputManager();
    ~InputManager();
};

#endif
