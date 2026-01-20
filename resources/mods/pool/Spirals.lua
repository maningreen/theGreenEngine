return {
  description = "Spiraler fires another bullet,\nbut less health",
  onInit = function()
    Spiraler.bulletCount = Spiraler.bulletCount + 1
    Spiraler.startingHealth = Spiraler.startingHealth / 2
    CustomEnemy.spawnEnemy("spiraler", vector2(0, 0))
    return 1
  end
}
