return {
  description = "Spiraler fires another bullet,\nbut less health",
  onInit = function()
    Spiraler.bulletCount = Spiraler.bulletCount + 1
    Spiraler.startingHealth = Spiraler.startingHealth / 2
    return 1
  end
}
