//
// Post FX

#ifdef GL_ES
precision highp float;
#endif
uniform sampler2D texture0;
uniform vec2 resolution;
uniform float lowFreq;
uniform float editorCursorY;

void main( void ) {
  vec2 pos = gl_FragCoord.xy;
  vec2 res = resolution;
  int i = 0;
  int cnt = 32;
  gl_FragColor = texture2D(texture0, vec2(pos.x/res.x, pos.y/res.y));
    
  for (i = 0; i < cnt; i ++) {
    float a = 0.03;
    float offset = 0.0;
    float offset2 = 1500.0 * lowFreq * lowFreq * (float(i) - 16.0) / 16.0;
    
    a = (16.0 - abs(float(i) - 16.0)) / 128.0;
  
    if (abs(editorCursorY - (pos.y + offset2)) < 5.0)    
      gl_FragColor += a * texture2D(texture0, vec2((pos.x + offset) / res.x, (pos.y + offset2) / res.y));  
  }
  
}
