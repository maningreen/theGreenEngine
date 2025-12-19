#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float leng = 10;

// Output fragment color
out vec4 finalColor;

uniform float pixelLength = 5;

vec4 to4Bit(vec4 i) {
  // we do 15 becase 2^4 - 1 is 15, so there're 15 shades of any colour in 4 bit
  return floor(i * 15) / 15;
}

void main()
{
  vec2 pos = fragTexCoord;

  pos = vec2(floor(fragTexCoord.x / (pixelLength / leng)) * pixelLength / leng, floor(fragTexCoord.y / (pixelLength / leng)) * pixelLength / leng);

  vec4 col = texture(texture0, pos);

  finalColor = vec4(to4Bit(col));
}
