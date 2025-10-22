#ifndef inputManager_h
#define inputManager_h

#include "engine/entity.hpp"
#include <functional>
#include <vector>

// A type representing a keybind
struct keybind {
  // the key (only standard keys, no mouse) on which to call callback
  Key key;
  // the function to be called on key press
  std::function<void(void)> callback;
  // wether or not it's a mouse button
  bool mouseButton;

  // a simple constructor
  keybind(int key, std::function<void(void)> callback);
  keybind(int key, std::function<void(void)> callback, bool isMouseButton);
};

typedef std::vector<keybind> keybinds;

class InputManager : public Entity {
  public:
    keybinds binds;

    // binds of which to call on an input-vector input
    std::vector<std::function<void(Vector2)>> vectorBinds;

    void Process(float delta) override;

    void addBind(const keybind& bind);
    void removeBind(int index);

    Key up;
    Key down;
    Key left;
    Key right;

    InputManager(Key up, Key down, Key left, Key right, std::vector<keybind> binds);
    InputManager();
    ~InputManager();
};

#endif
