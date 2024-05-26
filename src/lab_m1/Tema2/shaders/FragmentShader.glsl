#version 330

// Input
// TODO(student): Get color value from vertex shader
in vec3 color;
uniform int hp;
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    float c = (5-hp)/10.0f;
    
    out_color = vec4(color[0] - c, color[1] - c, color[2] - c, 1);

}
