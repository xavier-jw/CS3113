#version 330

// Input attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor; // Get color passed from Raylib

// Output to fragment shader
out vec2 fragTexCoord;
out vec4 fragColor;  // Pass color down to fragment shader

uniform mat4 mvp;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor; // Pass the color
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}