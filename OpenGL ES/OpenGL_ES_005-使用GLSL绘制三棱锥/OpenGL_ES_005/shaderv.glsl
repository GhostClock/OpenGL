attribute vec4 position; // 顶点
attribute vec4 positionColor; // 顶点颜色

uniform mat4 projectionMatrix; // 投影矩形
uniform mat4 modelViewMatrix; // 模型视图矩阵

varying lowp vec4 varyColor;

void main() {
    varyColor = positionColor;
    vec4 vPos;
    vPos = projectionMatrix * modelViewMatrix * position;
    gl_Position = vPos;
}
