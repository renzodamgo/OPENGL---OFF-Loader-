#version 330 core
layout (location = 0) in vec3 pPosicion;
layout (location = 1) in vec3 pColor;
out vec4 color_vertice;
uniform vec3 color;
uniform mat4 transformacion;
void main(){
	gl_Position =  transformacion*vec4(pPosicion, 1.0) ;
	color_vertice = vec4(color, 1.0);
}
