#version 330 core
out vec4 FragColor;

in vec2 localPos;
in vec2 worldPos;

void main()
{
    // 1. Create a "misty" radial glow for the particle itself
    float distToParticleCenter = length(localPos);
    // Smooth falloff from center to edge (range 0 to 1)
    float particleAlpha = 1.0 - smoothstep(0.0, 1.0, distToParticleCenter);
    
    // 2. Galaxy effect: Brighter at center, dimmer as we move out
    float distToGalaxyCenter = length(worldPos);
    // Calculate brightness falloff from the origin (0,0)
    float galaxyBrightness = 1.0 / (1.0 + distToGalaxyCenter * 1.5);
    
    // 3. Final color: White/Blueish tint with calculated brightness and misty alpha
    vec3 color = vec3(0.8, 0.9, 1.0) * galaxyBrightness;
    FragColor = vec4(color, particleAlpha * 0.7);
}
