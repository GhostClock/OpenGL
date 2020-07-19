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

GLFrame             cameraFrame;        // 角色帧 照相机角色帧

static int const NUM_MIN_SPHERES = 50; // 随机小球的个数
GLFrame spheres[NUM_MIN_SPHERES];

// 此函数在呈现上下文中进行任何必要的初始化。.
// 这是第一次做任何与opengl相关的任务。
void SetupRC() {
    //1.清理颜色并初始化
    glClearColor(0, 0, 0, 1);
    shaderManager.InitializeStockShaders();
    
    //2.开启深度测试
    glEnable(GL_DEPTH_TEST);// 看旋转的话可以关闭深度测试
    
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
    
    //4.设置大球数据
    /*
     gltMakeSphere(三角形批次类, 弧度数, 片数, 堆栈数)
     */
    gltMakeSphere(torusBatch, 0.4, 40, 80);
    
    //5.设置小球
    gltMakeSphere(sphereBatch, 0.1, 15, 26);
    
    //6.创建随机小球
    // 在同一个平面，Y值是一样的,X和Z产生随机值
    for (int i = 0; i < NUM_MIN_SPHERES; i ++) {
        // X和Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1);
        
        spheres[i].SetOrigin(x, 0.0, z); // 球的顶点
    }
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
    // 1.定义
    // 地板颜色
    static GLfloat vFloorColor[] = {0.5, 1.5, 0.5, 1.0};
    // 定义大球颜色
    static GLfloat vTorusColor[] = {0.5, 0.5, 1.0, 1.0};
    // 定义小球颜色
    static GLfloat vSphereColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
    
    // 2.清理颜色缓冲区和深度缓冲区，因为上面使用了深度测试，所以要清理深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 基于时间的计时器，用作旋转动画
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0;
    
    // 开始压栈
    // 压栈的目的: 如果一个单元矩阵需要旋转，移动，缩放，完成后再出栈,当次的工作已经完毕，需要还原成原始的状态。
    // 下次进来也是基于开始的原地进行操作
    // 保证每次的操作不会出现冲突
    // push1
    modelViewMatrix.PushMatrix();
    
    // 设置观察者矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    // push5
    modelViewMatrix.PushMatrix(mCamera);
    
    // 3.绘制地面
    shaderManager.UseStockShader(GLT_SHADER_FLAT,
                                 transformPipeline.GetModelViewProjectionMatrix(),
                                 vFloorColor);
    // 4.开始绘制
    // 绘制地板
    floorBath.Draw();
    
    // 绘制大球
    // 5.设置点光源
    M3DVector4f vLightPos = {0, 10, 5, 1};
    // 6.往z轴移动3个像素
    // 笛卡尔坐标系屏幕左右是x，上下是y，垂直屏幕是z，且屏幕向内为负，相外为正
    modelViewMatrix.Translate(0, 0, -3);
    // 7.设置旋转
    // 压栈
    // push2
    modelViewMatrix.PushMatrix();
    // 8.开始旋转
    modelViewMatrix.Rotate(yRot, 0, 1, 0);
    // 9.指定合适的着色器
    // 过点光源的方式绘制
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
                                 transformPipeline.GetModelViewMatrix(),
                                 transformPipeline.GetProjectionMatrix(),
                                 vLightPos,
                                 vTorusColor);
    // 绘制大球
    torusBatch.Draw();
    // 出栈:入栈几次，就要出栈几次
    // pop1
    modelViewMatrix.PopMatrix();
    
    // 绘制50个小球:注意需要在第一个push里面,保证独立性
    for (int i = 0; i < NUM_MIN_SPHERES; i ++) {
        // push3
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        // 还是用过点光源的方式绘制
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
                                     transformPipeline.GetModelViewMatrix(),
                                     transformPipeline.GetProjectionMatrix(),
                                     vLightPos,
                                     vSphereColor);
        // 开始绘制
        sphereBatch.Draw();
        // pop3
        modelViewMatrix.PopMatrix();
    }
    
    // 绘制一个小球围绕大球旋转(围绕y轴旋转)
    // PS: 因为这是最后一个绘制出来的图像，不会影响到其他的图像，所以就不需要push和pop
    // modelViewMatrix.PushMatrix(); // push4
    // Rotate(每次旋转的度数, x, y, z)，
    modelViewMatrix.Rotate(yRot * -1, 0, 1, 0); // yRot*1可以调节快慢和方向，正为逆时针，负为顺时针
    // 防止和大球重叠，需要平移一段距离
    modelViewMatrix.Translate(1.0, 0, 0);
    // 用过点光源的方式绘制
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
                                 transformPipeline.GetModelViewMatrix(),
                                 transformPipeline.GetProjectionMatrix(),
                                 vLightPos,
                                 vSphereColor);
    // 绘制
    sphereBatch.Draw();
    // modelViewMatrix.PopMatrix(); // pop4
    
    // pop5
    modelViewMatrix.PopMatrix();
    // pop2
    modelViewMatrix.PopMatrix();
    
    // 10.执行缓冲区交换
    glutSwapBuffers();
    // 11.每次提交渲染
    glutPostRedisplay();
}

// 实现键位控制
void SpeacialKey(int key, int x, int y) {
    // 设置移动的步长
    float linear = 0.1;
    // 设置移动的角度
    float angular = float(m3dDegToRad(5.0));
    
    if (key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(linear);
    } else if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    } else if (key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angular, 0.0, 1, 0.0);
    } else if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0.0, 1, 0.0);
    } else {
        // Other
    }
    // glutPostRedisplay(); // 可以不用写，因为最终会调用RenderScene函数里面，里面调用了该函数
}

int main(int argc, char* argv[]) {
   
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
   glutInitWindowSize(800, 600);
    
   glutCreateWindow("综合案例");
   glutReshapeFunc(ChangeSize);
   glutDisplayFunc(RenderScene);
   glutSpecialFunc(SpeacialKey);
   
   GLenum err = glewInit();
   if (GLEW_OK != err) {
       fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
       return 1;
   }
   
   SetupRC();
   
   glutMainLoop();
   return 0;
}

