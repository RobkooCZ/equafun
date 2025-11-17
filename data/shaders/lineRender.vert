#version 330 core

layout (location = 0) in vec3 pointPos;
uniform mat4 graphProjection;

void main(){
  gl_Position = graphProjection * vec4(pointPos, 1.0f);
}
