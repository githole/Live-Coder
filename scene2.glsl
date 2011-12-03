uniform float lowFreq;
uniform vec2 resolution;
uniform float time;

float mandel(vec2 pos) {
  vec2 z = vec2(0.0, 0.0);
  const int limit = 32;
  int count;
  for (count = 0; count < limit; count ++) {
    if (length(z) > 2.0 * lowFreq * 10.0)
      break;
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + pos;
  }
  return float(count) / float(limit);
}

void main() {
  vec2 z = vec2(0.0, 0.0);
  vec2 pos = vec2(-(1.0 - gl_FragCoord.x / resolution.x) / 0.5, 
                  -(0.5 - gl_FragCoord.y / resolution.y) / 0.75);
  
  float r = mandel(pos);
  
  for (int i = 0; i < 8; i ++) {
    float a = sin(time / 0.1) * lowFreq * lowFreq;
    vec2 p = pos + a * vec2(sin(i / 8.0 * 6.29), cos(i / 8.0 * 6.28));
    
    r += mandel(p);
  }
  
  r /= 8.0;
  
  gl_FragColor = vec4(r, r/2.5, r/8.0, 1.0);
}
