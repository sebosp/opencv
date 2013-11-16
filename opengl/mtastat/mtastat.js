var gl;
var canvas;
var squareVerticesBuffer;
var mvMatrix;
var shaderProgram;
var squareVertexPositionBuffer;
var squareVertexColorBuffer;
var perspectiveMatrix;
var horizAspect = 480.0/640.0;
var vertices;

function drawScene() {
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  perspectiveMatrix = makePerspective(15, 640.0/480.0, 0.1, 100.0);
  loadIdentity();
  mvTranslate([-0.0, 0.0, -6.0]);
  gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);
  gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,
                         squareVertexPositionBuffer.itemSize,
                         gl.FLOAT,
                         false,
                         0,
                         0);
  gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexColorBuffer);
  gl.vertexAttribPointer(shaderProgram.vertexColorAttribute,
                         squareVertexColorBuffer.itemSize,
                         gl.FLOAT,
                         false,
                         0,
                         0);
  setMatrixUniforms();
  gl.drawArrays(gl.TRIANGLES, 0, squareVertexPositionBuffer.numItems);
}

function initBuffers() {
  squareVertexPositionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);

vertices = [
-0.805232,0,0.0,
-0.805232,0.05,0.0,
-0.999861,0.05,0.0,
-0.805232,0.05,0.0,
-0.999861,0.05,0.0,
-0.999861,0,0.0,
-0.999861,0.05,0.0,
-0.999861,0,0.0,
-0.805232,0,0.0,
-0.743506,0,0.0,
-0.743506,0.05,0.0,
-0.764498,0.05,0.0,
-0.743506,0.05,0.0,
-0.764498,0.05,0.0,
-0.764498,0,0.0,
-0.764498,0.05,0.0,
-0.764498,0,0.0,
-0.743506,0,0.0,
-0.649667,0,0.0,
-0.649667,0.05,0.0,
-0.658425,0.05,0.0,
-0.649667,0.05,0.0,
-0.658425,0.05,0.0,
-0.658425,0,0.0,
-0.658425,0.05,0.0,
-0.658425,0,0.0,
-0.649667,0,0.0,
-0.518292,0,0.0,
-0.518292,0.05,0.0,
-0.526912,0.05,0.0,
-0.518292,0.05,0.0,
-0.526912,0.05,0.0,
-0.526912,0,0.0,
-0.526912,0.05,0.0,
-0.526912,0,0.0,
-0.518292,0,0.0,
-0.151416,0,0.0,
-0.151416,0.05,0.0,
-0.348965,0.05,0.0,
-0.151416,0.05,0.0,
-0.348965,0.05,0.0,
-0.348965,0,0.0,
-0.348965,0.05,0.0,
-0.348965,0,0.0,
-0.151416,0,0.0,
-0.262355,0.05,0.0,
-0.262355,0.1,0.0,
-0.283903,0.1,0.0,
-0.262355,0.1,0.0,
-0.283903,0.1,0.0,
-0.283903,0.05,0.0,
-0.283903,0.1,0.0,
-0.283903,0.05,0.0,
-0.262355,0.05,0.0,
-0.0743982,0,0.0,
-0.0743982,0.05,0.0,
-0.111517,0.05,0.0,
-0.0743982,0.05,0.0,
-0.111517,0.05,0.0,
-0.111517,0,0.0,
-0.111517,0.05,0.0,
-0.111517,0,0.0,
-0.0743982,0,0.0,
0.0236116,0,0.0,
0.0236116,0.05,0.0,
0.0106827,0.05,0.0,
0.0236116,0.05,0.0,
0.0106827,0.05,0.0,
0.0106827,0,0.0,
0.0106827,0.05,0.0,
0.0106827,0,0.0,
0.0236116,0,0.0,
0.0213873,0.05,0.0,
0.0213873,0.1,0.0,
0.012629,0.1,0.0,
0.0213873,0.1,0.0,
0.012629,0.1,0.0,
0.012629,0.05,0.0,
0.012629,0.1,0.0,
0.012629,0.05,0.0,
0.0213873,0.05,0.0,
0.295676,0,0.0,
0.295676,0.05,0.0,
0.101463,0.05,0.0,
0.295676,0.05,0.0,
0.101463,0.05,0.0,
0.101463,0,0.0,
0.101463,0.05,0.0,
0.101463,0,0.0,
0.295676,0,0.0,
0.148313,0.05,0.0,
0.148313,0.1,0.0,
0.139694,0.1,0.0,
0.148313,0.1,0.0,
0.139694,0.1,0.0,
0.139694,0.05,0.0,
0.139694,0.1,0.0,
0.139694,0.05,0.0,
0.148313,0.05,0.0,
0.278854,0.05,0.0,
0.278854,0.1,0.0,
0.269818,0.1,0.0,
0.278854,0.1,0.0,
0.269818,0.1,0.0,
0.269818,0.05,0.0,
0.269818,0.1,0.0,
0.269818,0.05,0.0,
0.278854,0.05,0.0,
0.359069,0,0.0,
0.359069,0.05,0.0,
0.339606,0.05,0.0,
0.359069,0.05,0.0,
0.339606,0.05,0.0,
0.339606,0,0.0,
0.339606,0.05,0.0,
0.339606,0,0.0,
0.359069,0,0.0,
0.407032,0,0.0,
0.407032,0.05,0.0,
0.384371,0.05,0.0,
0.407032,0.05,0.0,
0.384371,0.05,0.0,
0.384371,0,0.0,
0.384371,0.05,0.0,
0.384371,0,0.0,
0.407032,0,0.0,
0.441092,0,0.0,
0.441092,0.05,0.0,
0.436643,0.05,0.0,
0.441092,0.05,0.0,
0.436643,0.05,0.0,
0.436643,0,0.0,
0.436643,0.05,0.0,
0.436643,0,0.0,
0.441092,0,0.0,
0.746243,0,0.0,
0.746243,0.05,0.0,
0.551613,0.05,0.0,
0.746243,0.05,0.0,
0.551613,0.05,0.0,
0.551613,0,0.0,
0.551613,0.05,0.0,
0.551613,0,0.0,
0.746243,0,0.0,
0.561762,0.05,0.0,
0.561762,0.1,0.0,
0.557313,0.1,0.0,
0.561762,0.1,0.0,
0.557313,0.1,0.0,
0.557313,0.05,0.0,
0.557313,0.1,0.0,
0.557313,0.05,0.0,
0.561762,0.05,0.0,
0.650457,0.05,0.0,
0.650457,0.1,0.0,
0.641977,0.1,0.0,
0.650457,0.1,0.0,
0.641977,0.1,0.0,
0.641977,0.05,0.0,
0.641977,0.1,0.0,
0.641977,0.05,0.0,
0.650457,0.05,0.0,
0.738875,0.05,0.0,
0.738875,0.1,0.0,
0.730255,0.1,0.0,
0.738875,0.1,0.0,
0.730255,0.1,0.0,
0.730255,0.05,0.0,
0.730255,0.1,0.0,
0.730255,0.05,0.0,
0.738875,0.05,0.0,
0.806439,0,0.0,
0.806439,0.05,0.0,
0.78781,0.05,0.0,
0.806439,0.05,0.0,
0.78781,0.05,0.0,
0.78781,0,0.0,
0.78781,0.05,0.0,
0.78781,0,0.0,
0.806439,0,0.0,
0.838692,0,0.0,
0.838692,0.05,0.0,
0.830211,0.05,0.0,
0.838692,0.05,0.0,
0.830211,0.05,0.0,
0.830211,0,0.0,
0.830211,0.05,0.0,
0.830211,0,0.0,
0.838692,0,0.0,
0.889295,0,0.0,
0.889295,0.05,0.0,
0.880815,0.05,0.0,
0.889295,0.05,0.0,
0.880815,0.05,0.0,
0.880815,0,0.0,
0.880815,0.05,0.0,
0.880815,0,0.0,
0.889295,0,0.0,
0.900139,0,0.0,
0.900139,0.05,0.0,
0.891659,0.05,0.0,
0.900139,0.05,0.0,
0.891659,0.05,0.0,
0.891659,0,0.0,
0.891659,0.05,0.0,
0.891659,0,0.0,
0.900139,0,0.0,
0.9,0,0.0,
0.9,0.05,0.0,
0.951994,0.05,0.0,
0.9,0.05,0.0,
0.951994,0.05,0.0,
0.951994,0,0.0,
0.951994,0.05,0.0,
0.951994,0,0.0,
0.9,0,0.0,
  ];
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
  squareVertexPositionBuffer.itemSize = 3;
  squareVertexPositionBuffer.numItems = vertices.length/3;
  squareVertexColorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexColorBuffer);
  colors = [];
  for (var i=0; i < squareVertexPositionBuffer.numItems; i++) {
    colors = colors.concat([0.5, 0.5, 1.0, 1.0]);
  }
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);
  squareVertexColorBuffer.itemSize = 4;
  squareVertexColorBuffer.numItems = squareVertexPositionBuffer.numItems;
}

function initShaders() {
  var fragmentShader = getShader(gl, "shader-fs");
  var vertexShader = getShader(gl, "shader-vs");
  shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);
  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert("Unable to initialize the shader program.");
  }
  gl.useProgram(shaderProgram);
  shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
  gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
  shaderProgram.vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aVertexColor");
  gl.enableVertexAttribArray(shaderProgram.vertexColorAttribute);
  shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
  shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
}
function initWebGL(canvas) {
  // Initialize the global variable gl to null.
  gl = null;
  try {
    // Try to grab the standard context. If it fails, fallback to experimental.
    gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
  }
  catch(e) {}
  if (!gl) {
    console.log("Unable to initialize WebGL. Your browser may not support it.");
  }
}
function startGL() {
  canvas = document.getElementById("glcanvas");
  initWebGL(canvas);
  if (gl) {
    gl.clearColor(0.0, 0.0, 0.0, 1.0);                      // Set clear color to black, fully opaque
    gl.enable(gl.DEPTH_TEST);                               // Enable depth testing
    gl.depthFunc(gl.LEQUAL);                                // Near things obscure far things
    gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);      // Clear the color as well as the depth buffer.
    initShaders();
    initBuffers();
    setInterval(drawScene, 15);
  }
}
function loadIdentity() {
  mvMatrix = Matrix.I(4);
}
function multMatrix(m) {
  mvMatrix = mvMatrix.x(m);
}
function mvTranslate(v) {
  multMatrix(Matrix.Translation($V([v[0], v[1], v[2]])).ensure4x4());
}
function setMatrixUniforms() {
  var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
  gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix.flatten()));
  var mvUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
  gl.uniformMatrix4fv(mvUniform, false, new Float32Array(mvMatrix.flatten()));
}
function getShader(gl, id) {
  var shaderScript, theSource, currentChild, shader;
  shaderScript = document.getElementById(id);
  if (!shaderScript) {
    return null;
  }
  theSource = "";
  currentChild = shaderScript.firstChild;
  while(currentChild) {
    if (currentChild.nodeType == currentChild.TEXT_NODE) {
      theSource += currentChild.textContent;
    }
    currentChild = currentChild.nextSibling;
  }
  if (shaderScript.type == "x-shader/x-fragment") {
    shader = gl.createShader(gl.FRAGMENT_SHADER);
  } else if (shaderScript.type == "x-shader/x-vertex") {
    shader = gl.createShader(gl.VERTEX_SHADER);
  } else {
     // Unknown shader type
     return null;
  }
  gl.shaderSource(shader, theSource);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      console.log("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
      return null;
  }
  return shader;
}
