/* shader to render simple particle system points */
#version 400 core 

in float opacity;
uniform sampler2D tex; // optional. enable point-sprite coords to use
out vec4 frag_color;

const vec4 particle_color = vec4 (0.2, 0.5, 0.8, 0.8); 

void main () {
  // using point texture coordinates which are pre-defined over the point
  //vec2 texcoord = vec2 (gl_PointCoord.s, 1.0 - gl_PointCoord.t);
  //vec4 texel = texture (tex, texcoord);
  frag_color.a = opacity * particle_color.a;
  frag_color.rgb = particle_color.rgb ;
}