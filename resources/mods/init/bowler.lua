local speed = 1000
local damage = 3

local function normalize(x)
  if x < 0 then
    return -1
  else
    if x > 0 then
      return 1
    end
    return 0
  end
end

local en = {
  radius = 100,
  color = color(255, 255, 255, 20),
  initialState = 1,
  name = "bowling",
  onSpawn = function(self)
    local theta = math.random() * 2 * math.pi
    self.velocity.x = math.cos(theta) * speed
    self.velocity.y = math.sin(theta) * speed
  end,
  dropHealth = function(self) end,
  manageState = function(self, delta)
    -- manage wrapping
    if math.abs(self.position.x) >= Border.getLength() - 10 then
      self.velocity.x = -self.velocity.x
      self.position.x = normalize(self.position.x) * (Border.getLength() - 10)
    end
    if math.abs(self.position.y) >= Border.getLength() - 10 then
      self.velocity.y = -self.velocity.y
      self.position.y = normalize(self.position.y) * (Border.getLength() - 10)
    end
    self.velocity = self.velocity:normalize():scale(speed)

    -- damage check
    local plr = Global.getPlayer()
    local dist = Border.getDistance(plr.position, self.position)
    if dist < self.radius then
      local hlth = plr:getHealth()
      hlth:applyDamage(damage)
      -- get the difference in position
      local vecToPlr = Border.getShortestPathToPoint(self.position, plr.position):normalize()
      -- apply a velocity
      plr.velocity = vecToPlr:scale(speed):add(self.velocity)
    end
  end,
  onDeath = function(self)
    plr = Global.getPlayer()
    plr.velocity = plr.velocity:add(self.velocity:normalize():scale(speed))
  end,
  maxHealth = 10,
}

return {
  onInit = function()
    CustomEnemy.addEnemy(en)
    -- CustomEnemy.spawnEnemy("bowling", vector2(300, 300))
    return 1
  end
}
