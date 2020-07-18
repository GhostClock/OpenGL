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
    //1.清理颜色并初始化
    glClearColor(0, 0, 0, 1);
    shaderManager.InitializeStockShaders();
    
    //2.开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //3.设置地板顶点数据
    //基于物体坐标系的正方体
    floorBath.Begin(GL_LINES, 324); // 用线段连接，一共324个顶点
    for (GLfloat x = -20; x <= 20.0; x += 0.5) {
        floorBath.Vertex3f(x, -0.55, 20.0);
        floorBath.Vertex3f(x, -0.55, -20.0);
        
        floorBath.Vertex3f(20.0, -0.55, x);
        floorBath.Vertex3f(-20.0, -0.55, x);
    }
    floorBath.End();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h) {
    //1.设置视口
    glViewport(0, 0, w, h);
    
    // 2.创建投影矩阵.
    // SetPerspective 是创建viewFrustum投影矩阵
    viewFrustum.SetPerspective(35.0, float(w) / float(h), 1.0, 100.0);
    
    // GetProjectionMatrix 是获取viewFrustum投影矩阵
    // 并将其加载到投影矩阵堆栈上
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 3. 设置变换管道已使用两个矩阵(投影矩阵：modelViewMatrix，变换矩阵projectionMatrix)
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

//召唤场景
void RenderScene(void) {
    // 1.定义地板颜色
    static GLfloat vFloorColor[] = {0.5, 1.5, 0.5, 1.0};
    
    // 2.清理颜色缓冲区和深度缓冲区，因为上面使用了深度测试，所以要清理深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 3.绘制地面
    shaderManager.UseStockShader(GLT_SHADER_FLAT,
                                 transformPipeline.GetModelViewProjectionMatrix(),
                                 vFloorColor);
    // 4.开始绘制
    floorBath.Draw();
    
    // 5.执行缓冲区交换
    glutSwapBuffers();
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

