#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in float aScale;

uniform mat4 projection;

out vec2 localPos;
out vec2 worldPos;

void main()
{
    // localPos helps calculate the "misty" radial glow for each circle
    localPos = aPos.xy; 
    
    // worldPos helps calculate global brightness based on distance from the center
    vec2 positioned = (aPos.xy * aScale) + aOffset;
    worldPos = positioned;
    
    gl_Position = projection * vec4(positioned, 0.0, 1.0);
}