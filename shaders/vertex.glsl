#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform vec2 offset;
uniform float scale;

out vec2 localPos;
out vec2 worldPos;

void main()
{
    // localPos helps calculate the "misty" radial glow for each circle
    localPos = aPos.xy; 
    
    // worldPos helps calculate global brightness based on distance from the center
    vec2 positioned = (aPos.xy * scale) + offset;
    worldPos = positioned;
    
    gl_Position = projection * vec4(positioned, 0.0, 1.0);
}
