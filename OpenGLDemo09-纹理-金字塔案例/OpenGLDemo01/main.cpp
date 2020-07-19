/*
 实现功能：纹理-金字塔案例
 */

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLShaderManager.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "GLBatch.h"

#include <math.h>
#include <glut/glut.h>

GLShaderManager     shaderManager;      // 着色器
GLMatrixStack       modelViewMatrix;    // 模型视图矩阵
GLMatrixStack       projectionMatrix;   // 投影矩阵
GLFrame             cameraFrame;        // 观察者
GLFrame             objectFrame;        // 视图
GLFrustum           viewFrustum;        // 视景体
GLGeometryTransform transformPipeline;  // 几何图形变换管道
GLBatch             pyramidBatch;       // 金字塔
GLuint              textureID;          // 纹理变量，一般使用无符号整型
M3DMatrix44f        shadowMatrix;

bool LoadTAGTexture(const char *szFileName, GLenum minFilter,
                    GLenum magFilter, GLenum wrapMode) {
    GLbyte *pBits;
    int nWidth, nHeight, nComponents; // 宽，高，颜色组件
    GLenum eFormat;
    
    //1.读取纹理的像素
    pBits = gltReadTGABits(szFileName, &nWidth,
                           &nHeight, &nComponents, &eFormat);
    if (pBits == NULL) {
        return false;
    }
    
    //2.设置纹理参数
    //设置环绕模式 因为是2D所以只要s和t
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //3.设置纹理的过滤方式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    //4.载入纹理
    /*
     //参数1：纹理维度
     //参数2：mip贴图层次
     //参数3：纹理单元存储的颜色成分（从读取像素图是获得）
     //参数4：加载纹理宽
     //参数5：加载纹理高
     //参数6：加载纹理的深度
     //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
     //参数8：指向纹理图像数据的指针
     */
    glTexImage2D(GL_TEXTURE_2D, 0,
                 nComponents, nWidth,
                 nHeight, 0, eFormat,
                 GL_UNSIGNED_BYTE,
                 pBits);
    free(pBits);
    
    // 客户端开发可以不用写
    //只有minFilter 等于以下四种模式，才可以生成Mip贴图
    //GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    //GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    //GL_LINEAR_MIPMAP_LINEAR 和GL_NEAREST_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除他们之间的过滤痕迹。
    //GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图。纹理过滤的黄金准则，具有最高的精度。
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST) {
        //5.纹理生成所有的Mip层
        //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    return true;
}

//绘制金字塔
void MakePyramid(GLBatch& pyramidBatch) {
    /*1、通过pyramidBatch组建三角形批次
      参数1：类型
      参数2：顶点数
      参数3：这个批次中将会应用1个纹理
      注意：如果不写这个参数，默认为0。
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
    /***前情导入
     
     2)设置纹理坐标
     void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
     参数1：texture，纹理层次，对于使用存储着色器来进行渲染，设置为0
     参数2：s：对应顶点坐标中的x坐标
     参数3：t:对应顶点坐标中的y
     (s,t,r,q对应顶点坐标的x,y,z,w)
     
     pyramidBatch.MultiTexCoord2f(0,s,t);
     
     3)void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
      void Vertex3fv(M3DVector3f vVertex);
     向三角形批次类添加顶点数据(x,y,z);
      pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
     */
    
    //塔顶
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f,  -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f,  -1.0f, -1.0f };
    
    //金字塔底部
    //底部的四边形 = 三角形X + 三角形Y
    //三角形X = (vBackLeft,vBackRight,vFrontRight)
    //vBackLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vBackRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //vFrontRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    
    //三角形Y =(vFrontLeft,vBackLeft,vFrontRight)
    //vFrontLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //vBackLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vFrontRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);

    
    // 金字塔前面
    //三角形：（Apex，vFrontLeft，vFrontRight）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);

    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);

    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    //金字塔左边
    //三角形：（vApex, vBackLeft, vFrontLeft）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //金字塔右边
    //三角形：（vApex, vFrontRight, vBackRight）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);

    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //金字塔后边
    //三角形：（vApex, vBackRight, vBackLeft）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //结束批次设置
    pyramidBatch.End();
}

// 此函数在呈现上下文中进行任何必要的初始化。.
// 这是第一次做任何与opengl相关的任务。
void SetupRC() {
    //1.清理颜色并初始化
    glClearColor(0.7, 0.7, 0.7, 1);
    shaderManager.InitializeStockShaders();
    
    //2.开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //3.分配纹理对象
    glGenTextures(1, &textureID);
    
    //4.绑定纹理 使用2D纹理
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    //5.加载纹理
    //参数1：纹理文件名称
    //参数2&参数3：需要缩小&放大的过滤器
    //参数4：纹理坐标环绕模式
    LoadTAGTexture("brick.tga",
                   GL_LINEAR_MIPMAP_NEAREST,
                   GL_LINEAR,
                   GL_CLAMP_TO_EDGE);
    
    //6.创建金字塔
    MakePyramid(pyramidBatch);
    
    //7.向Z方向移动一点观察者
    cameraFrame.MoveForward(-10);
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h) {
    //1.设置视口
    glViewport(0, 0, w, h);
    
    //2.创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    
    //viewFrustum.GetProjectionMatrix()  获取viewFrustum投影矩阵
    //并将其加载到投影矩阵堆栈上
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //3.设置变换管道以使用两个矩阵堆栈（变换矩阵modelViewMatrix ，投影矩阵projectionMatrix）
    //初始化GLGeometryTransform 的实例transformPipeline.通过将它的内部指针设置为模型视图矩阵堆栈 和 投影矩阵堆栈实例，来完成初始化
    //当然这个操作也可以在SetupRC 函数中完成，但是在窗口大小改变时或者窗口创建时设置它们并没有坏处。而且这样可以一次性完成矩阵和管线的设置。
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
}

//召唤场景
void RenderScene(void) {
    // 1.清理颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 2.开始压栈
    modelViewMatrix.PushMatrix();
    
    // 3.添加观察者矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    
    // 4.创建图形矩阵
    M3DMatrix44f mObjectFrame;
    objectFrame.GetCameraMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    // 5.绑定纹理
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // 6.纹理替换矩阵着色器
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,
                                 transformPipeline.GetModelViewProjectionMatrix(),
                                 0); // 这里0是纹理层
    // 7.绘制
    pyramidBatch.Draw();
    
    // 8.pop
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}



// 实现键位控制
void SpeacialKey(int key, int x, int y) {
    
    if (key == GLUT_KEY_UP) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0), 1.0, 0.0, 0.0);
    }
    
    if (key == GLUT_KEY_DOWN) {
        objectFrame.RotateWorld(m3dDegToRad(5.0), 1.0, 0.0, 0.0);
    }
    
    if (key == GLUT_KEY_LEFT) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0, 1.0, 0.0);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0, 1.0, 0.0);
    }
     glutPostRedisplay(); 
}

int main(int argc, char* argv[]) {
   
   gltSetWorkingDirectory(argv[0]);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
   glutInitWindowSize(800, 600);
    
   glutCreateWindow("纹理-金字塔");
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

