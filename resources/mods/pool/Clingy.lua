local dist = 300

return {
  description = "Enemies spawn closer to you",
  onSpawn = function(player, en)
    local ePos = en.position
    local pPos = player.position
    local delta = ePos:subtract(pPos):normalize():scale(dist)
    en.position = pPos:add(delta)
  end
}
