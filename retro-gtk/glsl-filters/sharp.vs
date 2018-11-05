#version 150 core

in vec2 position;
in vec2 texCoord;

out Vertex {
  vec2 texCoord;
} vertexOut;

uniform float relative_aspect_ratio;

void main ()
{
  vec2 aspect_ratio = (relative_aspect_ratio > 1.0f) ?
    vec2 (1.0 / relative_aspect_ratio, 1.0) :
    vec2 (1.0, relative_aspect_ratio);

  vertexOut.texCoord = texCoord;
  gl_Position = vec4 (position.xy * aspect_ratio, 0.0, 1.0);
}
