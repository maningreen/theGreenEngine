Dasher = {
  states = {
    following = 1,
    winding = 2,
    recovery = 3,
    dashing = 4,
  },
  speed = 2100,
  windupTime = .5,
  windupSpeed = 200,
  attackDist = 400,
  recoveryTime = 1,
  recoveryTau = 260,
  dashTime = .5,
  dashSpeed = 200000,
  damage = 4,
}

local dasher = {
  radius = 40,
  color = color(127, 106, 79, 255),
  initialState = Dasher.states.following,
  name = "Dasher",
  onSpawn = function(this)
    this.data = {
      dashVector = vector2(0, 0)
    }
  end,
  dropHealth = function()
  end,
  manageState = function(this, delta)
    local plr = Global.getPlayer()
    local vecToPlr = Border.getShortestPathToPoint(this.position, plr.position)
    local distToPlr = vecToPlr:length()
    vecToPlr = vecToPlr:normalize()
    if this.state == Dasher.states.following then
      local velToAdd = vecToPlr:normalize():scale(Dasher.speed)
      this.velocity = this.velocity:add(velToAdd:scale(delta))
      if distToPlr < Dasher.attackDist then
        this.state = Dasher.states.winding
      end
    elseif this.state == Dasher.states.winding then
      this.velocity = this.velocity:add(vecToPlr:scale(-this:getStateTime() * Dasher.windupSpeed))
      if this:getStateTime() > Dasher.windupTime then
        this.data.dashVector = vecToPlr;
        this.state = Dasher.states.dashing
        this.velocity = vector2(0, 0)
      end
    elseif this.state == Dasher.states.dashing then
      this.velocity = this.data.dashVector:scale(Dasher.dashSpeed * delta)

      if this:getStateTime() > Dasher.dashTime then
        this.state = Dasher.states.recovery
      end

      if distToPlr < this.radius + Player.hitBoxRadius then
        local health = plr:getHealth()
        health:applyDamage(Dasher.damage)

        this.state = Dasher.states.recovery

        -- reflect
        local scaledOffset = plr.position:subtract(this.position):normalize()
        scaledOffset = vector2(scaledOffset.x > 0 and -scaledOffset.x or scaledOffset.x,
          scaledOffset.y > 0 and -scaledOffset.y or scaledOffset.y)
        this.velocity = vector2(scaledOffset.x * this.velocity.x, scaledOffset.y * this.velocity.y)
      end
    elseif this.state == Dasher.states.recovery then
      -- if this.velocity:lengthSqr() > Dasher.recoveryTau * Dasher.recoveryTau then
        -- this:resetStateTime()
      if this:getStateTime() > .3 then
        this.state = Dasher.states.following
      end
    end
  end,
  onDeath = function() end,
  maxHealth = 1
}

return {
  description = "adds the base enemies",
  onInit = function()
    CustomEnemy.addEnemy(dasher)
    -- CustomEnemy.spawnEnemy(dasher.name, vector2(100, 100))
  end
}
