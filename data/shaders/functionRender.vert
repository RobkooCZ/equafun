#version 330 core
layout (location = 0) in vec2 pos;
uniform mat4 functionProjection;

void main(){
  gl_Position = functionProjection * vec4(pos, 0.0f, 1.0f);
}
