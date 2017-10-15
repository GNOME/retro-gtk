#version 150

in vec4 position;
in vec2 texCoord;

out Vertex {
  vec2 texCoord;
  // Define some calculations that will be used in fragment shader.
  vec2 one;
  float mod_factor;
} vertexOut;

uniform float relative_aspect_ratio;
uniform vec4 targetSize;
uniform vec2 outputSize;
uniform vec4 sourceSize[];

void main() {
  vec2 aspect_ratio = (relative_aspect_ratio > 1.0f) ?
    vec2 (1.0 / relative_aspect_ratio, 1.0) :
    vec2 (1.0, relative_aspect_ratio);

  vertexOut.texCoord = texCoord;
  gl_Position = vec4 (position.xy * aspect_ratio, 0.0, 1.0);
  // gl_Position = vec4 (position);

   // The size of one texel, in texture-coordinates.
   vertexOut.one = 1.0 / sourceSize[0].xy;

   // Resulting X pixel-coordinate of the pixel we're drawing.
   vertexOut.mod_factor = texCoord.x * targetSize.x * aspect_ratio.x;
}
