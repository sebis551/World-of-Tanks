#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform int hp;
uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;



void main()
{
    // TODO(student): Compute world space vectors

    float deformation = (5-hp)/40.0f;
    vec3 deformed_position = v_position + deformation * v_normal;
    
    // TODO(student): Send color light output to fragment shader
    color = object_color;
   

    gl_Position = Projection * View * Model * vec4(deformed_position, 1.0);
}
