#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 screenDems;

// Output fragment color
out vec4 finalColor;

uniform float pixelLength = 7.0;

void main()
{
    vec2 pos = fragTexCoord * screenDems;

    pos = vec2(floor(pos.x / pixelLength) * pixelLength, floor(pos.y / pixelLength) * pixelLength);

    pos = pos / screenDems;

    finalColor = texture(texture0, pos);
}
