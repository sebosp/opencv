varying vec4 f_color;
void main(void) {
   gl_FragColor = f_color;
   gl_FragColor[2] = 0.0f;
   gl_FragColor[3] = floor(mod(gl_FragCoord.y, 2.0));
}
