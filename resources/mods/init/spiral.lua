Spiraler = {
  states = {
    aproaching = 1,
    firing = 2
  },
  spinLength = 3,
  defaultRadius = 100,
  spiralSpeed = math.pi / 3.0,
  startingHealth = 3,
  maxTargetDistance = 50,
  targetDist = 500,
  speed = 2100,
  bulletLifetime = .5,
  bulletLifetimeGrowth = .15,
  bulletCount = 3,
  shotTime = .1,
}

local function getNewTarget(this)
  this:getClosestPointToPlayerWithDistance(Spiraler.targetDist)
end

local spiral = {
  radius = Spiraler.defaultRadius,
  color = color(0, 0, 255, 255),
  initialState = Spiraler.states.aproaching,
  name = "spiraler",
  onSpawn = function(this)
    this.data = {
      targetPosition = getNewTarget(this)
    }
    this.state = Spiraler.states.firing
  end,
  dropHealth = function()
  end,
  manageState = function(this, delta)
    if this.state == Spiraler.states.aproaching then
      local vecToTarget = Border.getShortestPathToPoint(this.position, this.data.targetPosition)
      local vPrime = vecToTarget:normalize()
      local vLen = vecToTarget:length()
      if vLen <= Spiraler.maxTargetDistance then
        this.state = Spiraler.states.firing
      else
        this.velocity = this.velocity:add(vPrime:scale(Spiraler.speed * delta))
      end
    elseif this.state == Spiraler.states.firing then
      if math.fmod(this:getStateTime(), Spiraler.shotTime) < delta then
        local baseTheta = this:getStateTime() * Spiraler.spiralSpeed
        for i = 0, Spiraler.bulletCount, 1 do
          this:fireBullet(
            baseTheta + i * 2 * math.pi / Spiraler.bulletCount,
            Spiraler.bulletLifetime + Spiraler.bulletLifetimeGrowth * this:getStateTime(),
            color(0, 0, 255, 255)
          )
        end
        if this:getStateTime() >= Spiraler.spinLength then
          this.data.targetPosition = this:getClosestPointToPlayerWithDistance(Spiraler.targetDist)
          this.state = Spiraler.states.aproaching
        end
      end
    end
  end,
  onDeath = function() end,
  maxHealth = 1
}

return {
  description = "adds the spiraler enemies",
  onInit = function()
    CustomEnemy.addEnemy(spiral)
    CustomEnemy.spawnEnemy("spiraler", vector2(1000, 1000))
  end
}
