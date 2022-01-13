
#ifdef __Vertex_Shader__
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 color;
layout(location = 2) in vec3 texCoord;

out vec4 ourColor;
out vec2 TexCoord;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
    ourColor = color;
    TexCoord = texCoord;
}

#endif

//--------------------

#ifdef __Fragment_Shader__
#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
 color = texture(ourTexture, TexCoord);
}
#endif





