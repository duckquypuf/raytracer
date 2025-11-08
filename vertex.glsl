#version 330 core
layout (location = 0) in vec2 aPos; // Vertex Position (0 to 1)
layout (location = 1) in vec2 aTex; // Texture Coordinate (0 to 1)

out vec2 texCoord;

uniform float aspectRatio;

void main()
{
    vec2 pos = aPos * 2.0 - 1.0; 
    pos.x *= aspectRatio;

    gl_Position = vec4(pos, 0.0, 1.0);
    texCoord = aTex;
}
