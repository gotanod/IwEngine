#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

in vec3 vert;

uniform mat4 model;

void main() {
	gl_Position = proj * view * model * vec4(vert, 1);
}
