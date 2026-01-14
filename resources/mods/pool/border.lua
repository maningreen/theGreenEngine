return {
  description = "Double border length",
  onInit = function()
    Border.setLength(Border.getLength() * 2)
    return 1
  end
}
