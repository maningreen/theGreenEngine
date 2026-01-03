-- returns (-1) -> 1
local function getRand()
  return (math.random() - .5) * 2
end

return {
  onInit = function(plr)
    plr.position.x = getRand() * Border.getLength()
    plr.position.y = getRand() * Border.getLength()
    return 1
  end
}
