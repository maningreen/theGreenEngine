return {
  onInit = function(player)
    player.speed = player.speed * 2
    local dash = player:getDashManager()
    dash.speed = player.speed
    dash.control = 100
    print(global.getFriction() * 100)
    global.setFriction(1)
    print(global.getFriction()* 100)
  end
}
