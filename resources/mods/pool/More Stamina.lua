return {
  description = "Adds another dash",
  onInit = function(plr)
    plr:getDash().maxDashCount = plr:getDash().maxDashCount + 1
    return 1
  end
}
