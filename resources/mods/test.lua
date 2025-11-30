return {
  onInit = function (player)
    print(player.speed)
  end,
  onKill = function (player)
    player.speed = player.speed * 3
  end
}
