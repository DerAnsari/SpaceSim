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

    // 3. Vibrant Color Palette
    // Calculate angle for color variation
    float angle = atan(worldPos.y, worldPos.x);

    // Base colors for our vibrant galaxy
    vec3 coreColor = vec3(1.0, 0.9, 0.7);// Warm white/yellow core
    vec3 midColor  = vec3(0.8, 0.2, 0.9);// Purple/Magenta mid-regions
    vec3 outerColor = vec3(0.1, 0.4, 1.0);// Deep blue outer regions

    // Interpolate colors based on distance from center
    vec3 colorVariation = mix(coreColor, midColor, smoothstep(0.0, 0.4, distToGalaxyCenter));
    colorVariation = mix(colorVariation, outerColor, smoothstep(0.4, 1.0, distToGalaxyCenter));

    // Add some color shifts based on the angle for extra vibrance
    colorVariation += 0.15 * vec3(sin(angle), cos(angle), sin(angle * 0.5));

    // 4. Combine: Multiply by brightness and ensure it pops on black
    // We boost the brightness slightly to make it more visible
    vec3 finalColor = colorVariation * galaxyBrightness * 1.5;

    // Increase alpha slightly for better visibility against black, while keeping misty feel
    FragColor = vec4(finalColor, particleAlpha * 0.85);
}
