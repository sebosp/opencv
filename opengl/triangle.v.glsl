attribute vec3 coord3d;
attribute vec4 v_color;
varying vec4 f_color;
uniform mat4 mvp;
void main(void) {
	gl_Position = mvp * vec4(coord3d, 1.0);
	f_color = v_color;
}
