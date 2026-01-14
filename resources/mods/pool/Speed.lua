return {
  description = "Double player speed,\ncuts dash speed",
  onInit = function(plr)
    plr.speed = plr.speed * 2
    plr:getDash().speed = plr:getDash().speed / 1.5
    return 1
  end
}
