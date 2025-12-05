#version 330

uniform sampler2D texture0;
uniform float flashFactor; // Hit flash intensity (0.0 - 1.0)

in vec2 fragTexCoord;
in vec4 fragColor; // Receive color from vertex shader

out vec4 finalColor;

void main()
{
    // Multiply texture color with vertex color.
    // For Lines/Shapes: texture0 is white, fragColor is Green -> Result is Green
    // For Sprites: texture0 is image color, fragColor is usually white -> Result is original image color.
    vec4 texColor = texture(texture0, fragTexCoord) * fragColor;
    
    // Logic for damage red flash effect
    vec3 redColor = vec3(1.0, 0.0, 0.0);
    vec3 finalRGB = mix(texColor.rgb, redColor, flashFactor);
    
    finalColor = vec4(finalRGB, texColor.a);
}