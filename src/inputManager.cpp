#include "inputManager.hpp"

#include "engine/entity.hpp"
#include "include.h"
#include "raylib.h"
#include "raymath.h"

#include <functional>
#include <vector>

keybind::keybind(int k, std::function<void(void)> c) : key(k), callback(c), mouseButton(false) {}
keybind::keybind(int k, bool m, std::function<void(void)> c) : key(k), mouseButton(m), callback(c) {}

InputManager::InputManager(Key u, Key d, Key l, Key r, std::vector<keybind> b)
  : Entity("Input Manager"), binds(b), up(u), down(d), left(l), right(r) {}

InputManager::InputManager() : Entity("InputManager") {}

InputManager::~InputManager() {}

void InputManager::addBind(const keybind& bind) { binds.push_back(bind); }

void InputManager::removeBind(int i) {
  if(i >= 0 && i < binds.size())
    binds.erase(binds.begin() + i);
}

void InputManager::Process(float delta) {
  for(int i = 0; i < binds.size(); i++)
    if(binds[i].mouseButton) {
      if(IsMouseButtonDown(binds[i].key))
        binds[i].callback();
    } else if(IsKeyPressed(binds[i].key))
      binds[i].callback();

  Vector2 input = getInputVector(up, down, left, right);
  if(input != Vector2Zero())
    for(int i = 0; i < vectorBinds.size(); i++)
      vectorBinds[i](input);
}
