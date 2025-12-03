#include "inputManager.hpp"

#include "engine/entity.hpp"
#include "include.h"
#include "raylib.h"

#include <functional>
#include <vector>

keybind::keybind(int k, std::function<void(void)> c)
  : key(k), callback(c), mouseButton(false) {}
keybind::keybind(int k, bool m, std::function<void(void)> c)
  : key(k), mouseButton(m), callback(c) {}

keybindAlt::keybindAlt(int k, std::function<void(Vector2)> c)
  : key(k), callback(c), mouseButton(false) {}
keybindAlt::keybindAlt(int k, bool m, std::function<void(Vector2)> c)
  : key(k), mouseButton(m), callback(c) {}

InputManager::InputManager(Key u, Key d, Key l, Key r, keybinds b)
  : Entity("Input Manager"), binds(b), up(u), down(d), left(l), right(r) {}

InputManager::InputManager() : Entity("InputManager") {}

InputManager::~InputManager() {}

void InputManager::addBind(const keybind& bind) { binds.push_back(bind); }
void InputManager::addBind(const keybindAlt& bind) { binds.push_back(bind); }

void InputManager::addVectorBind(std::function<void(float, Vector2)> bind) {
  vectorBinds.push_back(bind);
}

void InputManager::removeBind(int i) {
  if(i >= 0 && i < binds.size())
    binds.erase(binds.begin() + i);
}

void InputManager::removeVectorBind(int i) {
  if(i >= 0 && i < vectorBinds.size())
    vectorBinds.erase(vectorBinds.begin() + i);
}

void InputManager::process(float delta) {
  Vector2 input = getInputVector(up, down, left, right);
  for(int i = 0; i < binds.size(); i++) {
    // it's keybind
    if(binds[i].index() == 0) {
      keybind bind = std::get<keybind>(binds[i]);
      if(bind.mouseButton && IsMouseButtonPressed(bind.key))
        bind.callback();
      else if(!bind.mouseButton && IsKeyPressed(bind.key))
        bind.callback();
    } else {
      keybindAlt bind = std::get<keybindAlt>(binds[i]);
      if(bind.mouseButton && IsMouseButtonPressed(bind.key))
        bind.callback(input);
      else if(!bind.mouseButton && IsKeyPressed(bind.key))
        bind.callback(input);
    }
  }
  for(int i = 0; i < vectorBinds.size(); i++)
    vectorBinds[i](delta, input);
}
