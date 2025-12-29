# Lua api documentation

This document will go over how the lua modding framework is structured, and it's capabilties with interfacing with the game.

## Structure of a mod

There are two types of mods, and up to 5 functions with unique triggers in each mod
  - pool
    - added at runtime
    - `resources/mods/pool`
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
    end,
    onDash = function()
    end,
    onFire = function()
    end,
    onSpawn = function()
    end,
    onKill = function()
    end
}
```

Here's another example mod,
this one prints the players speed, then doubles the player's speed
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
    - A: Yes, otherwise the program will throw a hissy fit (crash)

  - Q: Do I *really* need to define an onInit?
    - A: Yeah, sorry... If you hate it, nag me, and in the meantime use `onInit = function() end`

  - Q: What if I have an item that's not specified in the return?
    - A: We ignore it

  - Q: What did that question ask?
    - A: say i have a table, say there's a member `x`, if i return that table as a mod, will that cause a problem? (No, we just ignore it)
    - Example:
    ```lua
    return {
        a = "something!"
        onInit = function() end
    }
    ```

  - Q: Can I do things with these arguments? What are they?
    - A: Later.

### Basic mod template

```lua
return {
    onInit = function(player)
        print("Hello, World!")
    end,

    onDash = function(player)
    end,

    onFire = function(player, bullet)
    end,

    onKill = function(player, enemy)
    end,

    onSpawn = function(player, enemy)
    end
}
```

This template provides every function, and argument. And prints "Hello, world".


### Requirements of a mod

Mods should be placed in either of the two paths, with a `.lua` extention.<br>
Examples:
  - `resources/mods/init/setFriction.lua`
  - `resources/mods/init/increase\ speed.lua`
  - `resources/mods/pool/increaseDashCount.lua`
Every mod has its own name.
Mod names are derivated from the file name, example: `setFriction.lua` -> `setFriction`

Every mod must have an `onInit` function.
If it doesn't, the mod is ignored.

Every other function is not necessary to be defined and completely optional (onKill, onDash...).

### Loading a mod

You may have noticed the `return {...}` in every example, this is because the way the game parses this is by evaluation. The code is evaluated and then the table is parsed into an internal class.

### Unloading a mod

If you want you mod to denitialize itself, return `1`, from any function, when it's called, the mod will be released.
Here's an example of a mod which will only run for one dash
```lua
return {
    onInit = function() end
    onDash = function()
        print("hey, you dashed!")
        return 1
    end
}
```

If you return, say a string, or a float, or `0`, nothing will happen. Only if you return `1`.

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

#### Dash (manager)

Represents dashes, an abstract undefined resource, which is used for dashes.

`speed :: Float`
- the speed of the dash (px / s)

`control :: Float`
- the amount of 'control' the input has on the dash

`regenDelay :: Float`
- the amount of time (s) after a dash one must wait for it to begin recharging, does *not* include the dash itself

`regenRate :: FLoat`
- the rate at which the dash regenerates (dashes / s)

`length :: Float`
- the amount of time (s) a dash sould last.

`maxDashCount :: Float`
- the maximum amount of 'dashes' you can have, sets the maximum of dashProgress

`getAvailableDashes :: DashManager -> Int`
- returns the amount of 'dashes' available, equivalent to the floor of getDashProgress

`isDashing :: DashManager -> Bool`
- returns whether or not it's in a dashing state.

`getDashProgress :: DashManager -> Float`
- returns the amount of "dashProgress" is availabe, dashProgress is an abstract unit used to represent dashes, 1 dashProgress is 1 available dash.

`removeDash :: DashManager -> Void`
- subtracts one from the dashProgress

`addDash :: DashManager -> Void`
- adds one to the dashProgress

`getDashVelocity :: DashManager -> Vector2`
- returns the velocity of the dash, equivalent to `getDashDirection() * speed`

`getDashDirection :: DashManager -> Vector2`
- returns the directon of the dash in a normalized vector

`getDeltaDash :: DashManager -> Float`
- returns the "deltaDash", a variable used to represent the elapsed time since a dash began.

`canDash :: DashManager -> Bool`
- returns whether or not the DashManager is in a state which meets the criteria for dashing.

#### InputManager

##### Its youngest child Key

Key is a number used to represent (get this) a key on the keyboard or a button on the mouse.
"How do i find the value?", 2 ways:
  1. For key A, use 'a', this will supply the ascii code
  2. Comb through [./keys.md]

##### Its son keybind

Keybind is a struct used to represent a (guess what) keybind.

`keybind() :: Key -> Bool -> (Void -> Void)`
- `keybind(key, isMouse, function)` is how it's defined, give it the key index, whether or not it's a mouse button, and a function.
- example:
```lua
keybind(
    66, -- the number in keys.md for the 'b' key
    false, -- it is not a mouse key
    function()
        print("you pressed B!")
    end
)
```

```lua
keybind(
    1, -- the number in keys.md for right mouse button
    true, -- it is a mouse key
    function()
        print("you clicked your right mouse button!")
    end
)

```

##### Its daughter keybindAlt

keybindAlt is a struct used to represent an alternate keybind, which takes in the input vector as an argument to it's function.

`keybindAlt() :: Key -> Bool -> (Vector2 -> Void)`
- give it the same keys as keybind, and whether or not its a mouse key, and an alternative function, which takes in the input vector.

##### The Father.

InputManager manages keybinds.

The input vector
- keys 
  - `up :: Key`     Default `w`
  - `down :: Key`   Default `s`
  - `left :: Key`   Default `a`
  - `right :: Key`  Default `d`
- you can change any of these properties (*as long as you set them to keyboard keys*).
- how the game figures out where the player wants to move the player


`addVectorBind() :: InputManager -> (Vector2 -> Void) -> Void`
- A vector bind is a function, which is called every frame with the inputVector.

`addBind() :: (Keybind or keybindAlt) -> Void`
- adds a keybind to the list of keybinds

`removeVectorBind() :: Int -> Void`
- removes a vectorBind at the given index (0 base)

`removeBind() :: Int -> Void`
- removes a keybind or keybindAlt from the binds list, at the given index (0 base)

#### Enemy

Enemies are internally state machines.
This affects them largely.

`position :: Vector2`
- position of the enemy.

`velocity :: Vector2`
- velocity of the enemy.

`radius :: float`
- radius of the enemy, visual and effective

`color :: Color`
- color of the enemy.

`getStateTime() :: Void -> Void`
- returns the amount of time in a given state

`resetStateTime() :: Void -> Void`
- rests the amount of time in a given state to 0

`setState() :: Int -> Void`
- sets the internal state of an enemy. 
- WARNING: to avoid undefined behavior it's suggested to figure out all of the state of an enemy
- states are generally numberd 0..n, where n is the index of the 'last' state

`getState() :: Void -> Int`
- returns the current state.

`getHealth() :: Void -> HealthManager`
- returns the health manager of the enemy.

`dropHealthPack() :: Void -> Void` or `dropHealthPack :: Float -> Void`
- drops a health pack with a default HP, or a provided HP

#### NodeBullet

The NodeBullet is a damageless bullet, which spawns an attack nod at its destination. This is the bullet the player fires.

`theta :: Float`
- the rotation of the bullet.

`lifetime :: Float`
- the amount of time (s) the bullet has been alive.

`targetLifetime :: Float`
- the amount of time (s) the bullet will be alive.

#### Maybe N

A table that may or may not have another type.

`valid :: Bool`
- whether or not the type is valid

`value :: N`
- will cause an error if called unsafely.

### Custom Enemies

If you want to add a custom enemy, this is your guide.
Enemies have some internal functions you must define first, then you must tell the game "hey please add this enemy into the rotation k? thx <3"

Here's how to go about it.

#### Defining an enemy

Custom enemies are simply tables with a couple of internal functions,
but in order to fully flesh out an enemy you must call a constructor,
which takes in a table,
the table must have the following attributes.
Unless stated otherwise,
all of the following are necessary.
If you don't supply all required,
it will inform you in the console.

```lua
{
    manageStates = function(self, delta) end,
    dropHealth = function(self) end,
    onSpawn = function(self) end, -- OPTIONAL
    onDeath = function(self) end, -- OPTIONAL
    name = "example", -- the name used to find the enemy. *** MUST BE UNIQUE ***, if not, spawns undefined behavior
    color = color(255, 255, 255),
    radius = 100,
    maxHealth = 10,
    initialState = 1,
}
```

Custom enemies also have a `data` value, so that way you can store items in a table, or a variable, it's any type.

This is a minimal example of a valid enemy.
Beyond this, treat it like a standard enemy.

But how do i add this enemy to the global pool? See the `CustomEnemy` table

## Global tables

Global tables generally have internal static variables, and or functions.
These tables are effectively namespaces.

### Global

`setFriction() :: Float -> Void`
  - sets the global frictional constant
  - example: `setFriction(1)`
`getFriction() :: Float`
  - gets the global frictional constant
  - example: `getFriction()`
`getRoot() :: Entity`
  - gets the "root" entity

### Enemy
`addDeathHook :: (Enemy -> Void) -> void`

  - Provide a function, which takes in an `enemy` and returns `void`,
  and this function will be called every time an enemy is killed
  - example: 
    ```lua
    Enemy.addDeathHook(function(enemy)
       print("Enemy killed!")
    end)
    ```
`addSpawnHook() :: (Enemy -> Void) -> Void`
  - Similar to `addDeathHook`, Provide a function, which takes an `enemy`, and returns `void`
  this function will be called every time an enemy is spawned
  - example:
  ```lua
  Enemy.addSpawnHook(function(enemy)
    print("Enemy Spawned!")
  end)
  ```
`spawnEnemy() :: Vector2 -> Enemy`
  - This function spawns a dummy enemy at the coordinates specified
  - Enemy returned is automatically added to the entity heirarchy
`spawnSpiraler() :: Vector2 -> Enemy`
  - This function spawns a spiraler enemy at the coordinates specified
  - Enemy returned is automatically added to the entity heirarchy
`spawnDasher() :: Vector2 -> Enemy`
  - This function spawns a dasher enemy at the coordinates specified
  - Enemy returned is automatically added to the entity heirarchy
`spawnSniper() :: Vector2 -> Enemy`
  - This function spawns a dasher enemy at the coordinates specified
  - Enemy returned is automatically added to the entity heirarchy

### Border
The border table is a set of abstract functions used to implemet wrapping.

`length :: Float`
  - the lenght from the origin (center) to one side. The total length of one side is `2 * length`

`wrapEntity() :: Entity -> Void`
  - if an "entity" (a base internal class) is out of bounds, it's position will be wrapped around to be inside the border.

`wrapPos() :: Vector2 -> Vector2`
  - if a vector2 is out of bounds, it's position will be wrapped around to be inside the border.

`wrapPosX() :: Vector2 -> Vector2`
  - if a vector2's X position is out of bounds, it's X position will be wrapped around to be inside the border.

`wrapPosY() :: Vector2 -> Vector2`
  - if a vector2's Y position is out of bounds, it's Y position will be wrapped around to be inside the border.

`getShortestPathToPoint() :: Vector2 -> Vector2 -> Vector2`
 - returns the effective equivilent of `b - a`, with the vector to, wrapped.

`getDistance() :: Vector2 -> Vector2 -> Float`
  - returns the effective distance between two points.

### NodeBullet

`speed :: Float`
  - the speed (px / s) of all the bullets.

`radius :: Float`
  - the visual radius of the bullets.

`color :: Color`
 - the color of the bullets.

### Player

`defaultMaxHealth :: Float`
  - the default maximum health of the player
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`defaultDashSpeed :: Float`
  - the default dash speed (px / s) of the player
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`defaultDashTime :: Float`
  - the default length a dash lasts (s) for the player
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`defaultDashControl :: Float`
  - the default 'control' the player has over the direction a dash. (arbitrary, bigger == more)
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`defaultMaxDashCount :: Int`
  - the default maximum amount of dashes the player has.
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`defaultDashCooldown :: Float`
  - the default minimum amount of time between ending a dash and beginning a new one.
  - READ ONLY (if you try writing it *will* explode, and it *is* your fault)

`particleSpawnTime :: Float`
  - The time (s) between spawning the cool little exhaust particles.

### CustomEnemy

`addCustomEnemy() :: EnemyTable -> Void`
  - adds a custom enemy to the global enemy pool.

`spawnEnemy() :: String -> Vector2 -> Maybe Enemy`
  - spawns an enemy at the given coordinates, searches for name.
  - returns nothing if the given enemy's name was not found

### Maybe

`just :: a -> Maybe a`
  - returns a maybe with a value.

`nothing :: Maybe a`
  - a maybe with no value.

## A few more examples

This mod will give one free dash.
```lua
return {
  onInit = function() end,
  onDash = function(player)
    local dash = player:getDash() -- gets the internal dash manager
    dash:addDash() -- adds the dash
    return 1 -- sends the "free this mod" signal
  end
}
```
This mod can be edited to give infinite dashes, by removing the `return 1` line.

This mod would teleport an enemy near the player when spawned.
```lua
local dist = 200 -- set the target distance to 200 px

return {
  onInit = function() end,
  onSpawn = function(player, enemy)
    -- get the vector to the player
    local vecTo = Border.getShortestPathToPoint(player.position, enemy.position)
    -- scale the vector
    local scaled = vecTo:normalize():scale(dist)
    local endPos = player.position:add(scaled)
    enemy.position = endPos
  end
}
```

This mod will double the speed of dashing.
```lua
return {
  onInit = function(player)
    local dash = player:getDash()
    dash.speed = Player.defaultDashSpeed * 2
  end
}
```

This mod will crash the game.
```lua
return {
  onInit = function(player)
    Global.setFricition(-3)
    return 1
  end
}
```

This mod is some simple debugging
```lua
return {
  onInit = function()
    print("game init!")
  end,
  onSpawn = function(_, en)
    print("Enemy spawned! ", en.position.x, " ", en.position.y)
  end,
  onKill = function(_, _)
    print("Enemy killed!")
  end
}
```

This mod will kill every enemy, not fun, but you could do it.
```lua
return {
  onInit = function() end,
  onSpawn = function(_, en)
    local healthManager = en:getHealth()
    healthManager:applyDamage(healthManager:getMaxHealth())
  end
}
```

This mod will make every enemy immortal.
```lua
return {
  onInit = function() end,
  onKill = function(_, en)
    local healthManger = en:getHealth()
    healthManger:applyHealing(healthManger:getMaxHealth())
  end
}
```

This mod will make you feel bad.
```lua
return {
  onInit = function() 
    print("you suck")
  end
}
```

This mod will randomize the players position
```lua
math.randomseed(os.time())

return {
  onInit = function(plr) 
    plr.position.x = math.random(-Border.length, Border.length)
    plr.position.y = math.random(-Border.length, Border.length)
  end
}
```

## Credits

The amazing library used for the lua runtime, is [sol](https://github.com/ThePhD/sol), if you are taking any inspiration, I highly recommend this library.
