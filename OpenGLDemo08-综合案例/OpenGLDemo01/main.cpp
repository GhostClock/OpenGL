/*
 实现功能：综合案例
 */

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "GLBatch.h"
#include "StopWatch.h"

#include <math.h>
#include <glut/glut.h>

GLShaderManager     shaderManager;      // 着色器
GLMatrixStack       modelViewMatrix;    // 模型视图矩阵
GLMatrixStack       projectionMatrix;   // 投影矩阵
GLFrustum           viewFrustum;        // 视景体
GLGeometryTransform transformPipeline;  // 几何图形变换管道

GLTriangleBatch     torusBatch;         // 大球
GLTriangleBatch     sphereBatch;        // 小球
GLBatch             floorBath;          // 地板

// 此函数在呈现上下文中进行任何必要的初始化。.
// 这是第一次做任何与opengl相关的任务。
void SetupRC() {
   
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    
}

//召唤场景
void RenderScene(void)
{
  
}

int main(int argc, char* argv[]) {
   
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
   glutInitWindowSize(800, 600);
    
   glutCreateWindow("综合案例");
   glutReshapeFunc(ChangeSize);
   glutDisplayFunc(RenderScene);
   
   GLenum err = glewInit();
   if (GLEW_OK != err) {
       fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
       return 1;
   }
   
   SetupRC();
   
   glutMainLoop();
   return 0;
}

