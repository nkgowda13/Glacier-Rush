#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 viewProjectionMatrix;

void main() {
    gl_Position = viewProjectionMatrix * vec4(position, 1.0);
}