#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float length = 100;

// Output fragment color
out vec4 finalColor;

uniform float pixelLength = 5;

vec4 pallette(vec4 i) {
  // we want to greyscale it first, well get the brightness
  float brightness = (i.r + i.g + i.b + i.a) / 4;
  if(brightness > .75)
    return vec4(.908, .843, .65, 1);
  else
    return i;
}

vec2 pixelize(vec2 x) {
  float ratio = length / pixelLength;
  return vec2(
    floor(x.x * ratio) / ratio,
    floor(x.y * ratio) / ratio
  );
}

void main() {
  vec2 pos = pixelize(fragTexCoord);

  vec4 col = pallette(texture(texture0, pos));

  finalColor = col;
}
