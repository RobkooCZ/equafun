#version 330 core

layout (location = 0) in vec3 pointPos;

void main(){
  gl_Position = vec4(pointPos.x, pointPos.y, pointPos.z, 1.0f);
}
