# Lua api documentation

This document will go over how the lua modding framework is structured, and it's capabilties with interfacing with the game.

## Structure of a mod

There are two types of mods, and up to 5 functions with unique triggers in each mod
  - pool
    - added at runtime
    - `resources/mods/init`
  - init
    - added when the game is loading
    - `resources/mods/init`
It's up to you to decide which you want

Mods should be put into a particular path depending on what you want them to be<br>
Paths are mentioned above.

### Functions in a mod

The following is a possible (but useless) mod.
```lua
return {
    onInit = function()
    end
    onDash = function()
    end
    onFire = function()
    end
    onSpawn = function()
    end
    onKill = function()
    end
}
```

Here's another example mod,
this one doubles the player's speed
```lua
return {
    onInit = function(player) 
        print(player.speed)
        player.speed = player.speed * 2
    end
}
```

But these functions don't have arguments!, they do.

Here is a disection of all these functions:
- onInit
  - called when mod is first added to the mod list, **required**
  - called once
  - provides in arguments: player
- onDash
  - called every time the player has a valid dash
  - provides in arguments: player
- onFire
  - called every time a valid bullet is shot
  - provides in arguments: player, bullet
- onSpawn
  - called every time an enemy is spawned
  - provides in arguments: player, enemy
- onKill
  - called every time an enemy is killed
  - provides in arguments: player, enemy

FAQ:
  - Q: Do I need to define the arguments when defining the function?
    - A: No
  - Q: Do I need to define the arguments in order when defining the function?
    - A: Yes
  - Q: What if I have an item that's ignored?
    - A: We ignore it
  - Q: Can I do things with these arguments? What are they?
    - A: Later.

### Requirements of a mod

Mods should be placed in either of the two paths, with a `.lua` extention.<br>
Examples:
  - `resources/mods/init/setFriction.lua`
  - `resources/mods/init/increase\ speed.lua`
  - `resources/mods/pool/increaseDashCount.lua`
Every mod has its own name.
Mod names are derivated from the file name, example: `setFriction.lua` -> `setFriction`

Every mod must have an `onInit` function.
If it doesn't, the function is ignored

Every function is not necessary to be defined and completely optional

### Basic mod template

```lua
return {
    onInit = function(player)
        print("Hello, World!")
    end

    onDash = function(player)
    end

    onFire = function(player, bullet)
    end

    onKill = function(player, enemy)
    end

    onSpawn = function(player, enemy)
    end
}
```

This template provides every function, and argument.

### Loading a mod

You may have noticed the `return {...}` in every example, this is because the way the game parses this is by evaluation. The code is evaluated and then the table is parsed into an internal class.

## Types

There are multiple custom types defined, including:
  - vector2
  - color
  - player
  - healthManager
  - dashManager
  - inputManager
    - keybind
    - keybindAlt
  - enemy
  - player bullet

There are also global tables defined
  - Global
    - `setFriction() :: Float -> Void`
      - sets the global frictional constant
      - example: `setFriction(1)`
    - `getFriction() :: Float`
      - gets the global frictional constant
      - example: `getFriction()`
  - Enemy
    - `addDeathHook :: (Enemy -> Void) -> void`
      - Provide a function, which takes in an `enemy` and returns `void`,
      and this function will be called every time an enemy is killed
      - example: 
        ```lua
        addDeathHook(function(enemy)
            print("Enemy killed!")
        end)
        ```
    - `addSpawnHook :: (Enemy -> Void -> void)`
      - Similar to `addDeathHook`, Provide a function, which takes an `enemy`, and returns `void`
      this function will be called every time an enemy is spawned
      - example:
      ```lua
        addSpawnHook(function(enemy)
            print("Enemy Spawned!")
        end)
      ```

### Types, in depth
As mentioned there are multiple "types", implimented as classes in the back, but as tables for you, this is just if your curious, but doesn't amount to much.

The following are the various methods and properties these classes hold.

#### Vector2

`x :: Float`
- a number representing the 'x' position on a plane
- example: `aVector.x`

`y :: Float`
- a number representing the 'y' position on a plane
- example: `aVector.y`

`vector2() :: Float -> Float -> Vector2`
- a *global* function, takes in two floats, corropsonding to the `x` and `y` of a vector2, returns the vector2
- example: `local aVector = vector2(0, 0)`

`normalize() :: Vector2 -> Vector2`
- a member function, returns a new vector2 with a scale of 1, and angle of the input
- example: `aVector:normalize()`

`add() :: Vector2 -> Vector2 -> Vector2`
- a member function, returns a new vector2 with summed items
- example: `vector2(100, 100) == vector2(50, 50):add(vector2(50, 50))` is `true`
`scale() :: Vector2 -> Float -> Vector2`
- a member function, scales the `x` and `y` of a vector2, and returns a new one with the new values
- example: `local scaledVector = aVector:scale(3)`

`lerp() :: Vector2 -> Vector2 -> Float -> Vector2`
- a member function, runs a linear interpretation between two vectors, with a provided float, `t`
- example:
  ```lua
  local a = vector2(0, 0)
  local b = vector2(1, 1)
  local lerped = a:lerp(b, .5) -- == vector2(.5, .5)
  ```

#### Color

`r :: u8`
- a value (0-255) representing the *r*ed in a colour
- example: `colour1.r`

`g :: u8`
- a value (0-255) representing the *g*reen in a colour
- example: `colour1.g`

`b :: u8`
- a value (0-255) representing the *b*lue in a colour
- example: `colour1.b`

`a :: u8`
- a value (0-255) representing the *a*lpha in a colour
- the 'alpha' is the transperancy of a colour.
- example: `colour1.a`

`color() :: u8 -> u8 -> u8 -> u8 -> Color`
- a *global* function, given the colour values, returns a Color with the coorsponding values.
- example: `local white = color(255, 255, 255, 255)`

`lerp() :: Color -> Color -> Float -> Color`
- a member function, when called with the arguments, will return a new colour, with a lerp of the r, g, b, and a values respectively in each colour.
- example: `local lerped = color1:lerp(color2, .5)`

#### Player

`position :: Vector2`
- a vector, representing the position of the player
- examples: `player.position = vector2(100, 100)` sets the player's position
            `local pos = player.position`

`speed :: Float`
- a floating point value, representing the speed (in pixels / second ^ 2) of the player's input.
- examples: `player.speed = 100` sets the speed of the player
            `local speed = player.speed`

`velocity() :: Vector2`
- a vector representing the velocity (in px / second) of the player
- example: `local vel = player.velocity`

`getHealth() :: Player -> HealthManager`
- a member function, when called provides the healthManager of the player.
- example: `local health = player:getHealth()` 

`getInput() :: Player -> InputManager`
- a member function, when called provides the inputManager of the player.
- example: `local input = player:getInput()` 

`fireBullet() :: Player -> Void`
- a member function, when called fires a bullet at the cursors position.
- example `player:fireBullet()`

#### HealthManager

The HealthManager is used under the hood to manage the health enemies, and the player.

`getHealth() :: HealthManager -> Float`
- gets the current health
- example: `local currentHP = healthManager:getHealth()`

`setHealth() :: HealthManager -> Float -> Void`
- sets the current health
- example: `healthManager:setHealth(1)`, sets the health to 1

`applyDamage() :: HealthManager -> Float -> Void`
- removes `x` amount of health
- example: `healthManager:applyDamage(3)`, applies three hitpoints of damage, or in other words, removes 3 hitpoints of health

`applyHealing() :: HealthManager -> Float -> Void`
- The inverse of `applyDamage`
- adds `x` amount of health
- example: `healthManager:applyHealing(3)`, applies three hitpoints of healing, or in other words, adds 3 hitpoints of health

`isDead() :: HealthManager -> Bool`
- tells you if it's dead.
- example `healthManager:isDead()`

`getMaxHealth() :: HealthManager -> Float`
- returns the maximum HP.
- example: `local maxHealth = healthManager:getMaxHealth()`

`setMaxHealth() :: HealthManager -> Float -> Void`
- sets the maximum HP.
- example: `healthManager:setMaxHealth(10)`

<!-- #### Dash (manager)

Represents dashes, an abstract undefined resource, which is used for dashes.

`speed :: Float`
`control :: Float`
`regenDelay :: Float`
`length :: Float`
`maxDashCount :: Float`
 `maxDashCount :: Float` -->

## Credits

The amazing library used for the lua, is [sol](https://github.com/ThePhD), if you are taking any inspiration, I highly recommend this library.
