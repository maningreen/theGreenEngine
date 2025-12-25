-- returns (-1) -> 1
local function getRand()
  local x = math.random()
  local xPrime = x
  return (xPrime - .5) * 2
end

return {
  onInit = function(plr)
    plr.position.x = getRand() * Border.length()
    plr.position.y = getRand() * Border.length()
    print("YADLAKJSDLKAJSDl;")
    return 1
  end
}
