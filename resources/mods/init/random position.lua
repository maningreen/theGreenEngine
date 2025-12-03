math.randomseed(os.time())

return {
  onInit = function(plr)
    plr.position.x = math.random(-Border.length, Border.length)
    plr.position.y = math.random(-Border.length, Border.length)
    return 1
  end
}
