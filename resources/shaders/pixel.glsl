#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 textureSize;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float borderLength = 100;

// Output fragment color
out vec4 finalColor;

uniform float pixelLength = 5;

vec2 pixelize(vec2 x) {
  float ratio = borderLength / pixelLength;
  return vec2(
    floor(x.x * ratio) / ratio,
    floor(x.y * ratio) / ratio
  );
}

void main() {
  finalColor = texture(texture0, pixelize(fragTexCoord));
}
