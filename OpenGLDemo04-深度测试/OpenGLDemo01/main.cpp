/*
 实现功能：演示了OpenGL背面剔除，深度测试，和多边形模式
 */

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

////设置角色帧，作为相机
GLFrame             viewFrame;
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch; /// 批次类
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
///几何变换的管道
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

// 此函数在呈现上下文中进行任何必要的初始化。.
// 这是第一次做任何与opengl相关的任务。
void SetupRC() {
    // 1.设置背景颜色
    glClearColor(0.5, 0.3, 0.3, 1);
    
    //2.初始化着色器管理器
   shaderManager.InitializeStockShaders();
   
    //3.将相机向后移动7个单元：肉眼到物体之间的距离
    // 观测者不动 物体动
    // 观察者动，物体不动
    viewFrame.MoveForward(10.0);
   
   //4.创建一个甜甜圈
   //void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
   //参数1：GLTriangleBatch 容器帮助类
   //参数2：外边缘半径
   //参数3：内边缘半径
   //参数4、5：主半径和从半径的细分单元数量
   // 把顶点数据放在这个三角形批次类里面
   gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
   
   //5.点的大小(方便点填充时,肉眼观察)
   glPointSize(4.0f);
    
}


// 召唤场景
void RenderScene(void) {
    // 1.清除窗口和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (iCull) {
        glEnable(GL_CULL_FACE);
//        glFrontFace(GL_CCW);// 指定正面, 系统已经默认正面
        glCullFace(GL_BACK); //指定消除背面
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    // 打开关闭深度测试
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    //2.把摄像机矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    //3.设置绘图颜色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    //4.
     //使用平面着色器
     //参数1：平面着色器
     //参数2：模型视图投影矩阵
     //参数3：颜色
     //shaderManager.UseStockShader(GLT_SHADER_FLAT,
     //                             transformPipeline.GetModelViewProjectionMatrix(),
     //                             vRed);
     
     //使用默认光源着色器
     //通过光源、阴影效果跟提现立体效果
     //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
     //参数2：模型视图矩阵
     //参数3：投影矩阵
     //参数4：基本颜色值
     shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,
                                  transformPipeline.GetModelViewMatrix(),
                                  transformPipeline.GetProjectionMatrix(),
                                  vRed);
    //5.绘制
    torusBatch.Draw();

    //6.出栈 绘制完成恢复
    modelViewMatix.PopMatrix();
    
    //7.交换缓存区
    glutSwapBuffers();
}


//特殊键位处理（上、下、左、右移动）
void SpecialKeys(int key, int x, int y) {
    //1.判断方向
    if(key == GLUT_KEY_UP)
        //2.根据方向调整观察者位置
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //3.重新刷新
    glutPostRedisplay();
}



// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    //1.防止h变为0
    if(h == 0) h = 1;
    
    //2.设置视口窗口尺寸
    glViewport(0, 0, w, h);
    
    //3.3.setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
    // 设置透视模式，初始化其透视矩阵- 立体图像使用SetPerspective
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    
    //4.把透视矩阵加载到透视矩阵对阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //5.初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

// 右击菜单栏
void ProcessMenu(int value) {
    switch (value) {
        case 1:
            iDepth = !iDepth;
            break;
        case 2:
            iCull = !iCull;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("案例演示");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //特殊键位函数（上下左右）
    glutSpecialFunc(SpecialKeys);
    //显示函数
    glutDisplayFunc(RenderScene);
    // 注册右击菜单栏
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("深度测试",1);
    glutAddMenuEntry("正背面剔除",2);
    glutAddMenuEntry("线填充", 3);
    glutAddMenuEntry("点填充", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    //判断一下是否能初始化glew库，确保项目能正常使用OpenGL 框架
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    //绘制
    SetupRC();
    
    //runloop运行循环
    glutMainLoop();
    return 0;
}

