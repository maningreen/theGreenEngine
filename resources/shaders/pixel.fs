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

uniform float pixelLength = 5.0;

void main()
{
    vec2 scaledPos = fragTexCoord * screenDems;

    vec2 roundedPos = vec2(floor(scaledPos.x / pixelLength) * pixelLength, floor(scaledPos.y / pixelLength) * pixelLength);

    vec2 normalizedPos = roundedPos / screenDems;

    finalColor = texture(texture0, normalizedPos);
}
