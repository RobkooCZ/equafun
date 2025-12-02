#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 uiProjection;

out vec3 uiColor;

void main(){
  uiColor = color;
  gl_Position = uiProjection * vec4(pos, 1.0f);
}
