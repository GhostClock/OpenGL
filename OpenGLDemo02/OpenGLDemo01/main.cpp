//
//  main.m
//  OpenGLDemo01
//
//  Created by GhostClock on 2020/7/2.
//  Copyright © 2020 GhostClock. All rights reserved.
//

/*
 GLTool.h头文件包含了大部分GLTool中类似C语言的独立函数
 */
#include "GLTools.h"

/*
 在Mac 系统下，`#include<glut/glut.h>`
 在Windows 和 Linux上，我们使用freeglut的静态库版本并且需要添加一个宏
 */
#include <GLUT/GLUT.h>

/*
 导入GLTool着色器GLShaderManager类，没有着色器就不能在OpenGL进行着色，着色器不仅允许我们创建并管理着着色器，还提供一组‘存储着色器’，他们能够进行一个初步的渲染操作
 */
#include "GLShaderManager.h"

GLBatch triangleBatch;
GLShaderManager shaderManager;

//blockSize 边长
// 因为视口是[-1,1],所以blockSize = 0.2就是
GLfloat blockSize = 0.2f;

//正方形的4个点坐标
GLfloat vVerts[] = {
        -blockSize,-blockSize,0.0f,
        blockSize,-blockSize,0.0f,
        blockSize,blockSize,0.0f,
        -blockSize,blockSize,0.0f
};

// x方向移动的距离
GLfloat xPos = 0.0f;
// y方向移动的距离
GLfloat yPos = 0.0f;

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
void ChangeSize(int w,int h) {
    glViewport(0,0, w, h);
}

//开始渲染
void RenderScene(void) {
    
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f,0.5f,0.5f,1.0f};
    
    M3DMatrix44f mTransfromMatrix;
    
    //平移
    m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0.0f);
    
    // 平面着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mTransfromMatrix, vRed);
    
    /*
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    */
     
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

//为程序作一次性的设置
void SetupRC()
{
    //设置背影颜色
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    
    //批次处理
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    
    triangleBatch.CopyVertexData3f(vVerts);
    
    triangleBatch.End();
    
}

void SpecialKeys(int key, int x, int y){
    GLfloat stepSize = 0.025f;
    
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
    }
    
    //碰撞检测 平移的距离
    if (xPos < (-1.0f + blockSize)) {
        xPos = -1.0f + blockSize;
    }
    
    if (xPos > (1.0f - blockSize)) {
        xPos = 1.0f - blockSize;
    }
    
    if (yPos < (-1.0f + blockSize)) {
        yPos = -1.0f + blockSize;
    }
    
    if (yPos > (1.0f - blockSize)) {
        yPos = 1.0f - blockSize;
    }
    
    
    /*
    GLfloat stepSize = 0.025f;
    
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[10];
    
    printf("v[0] = %f\n",blockX);
    printf("v[10] = %f\n",blockY);
    
    
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }

    //触碰到边界（4个边界）的处理
    
    //当正方形移动超过最左边的时候
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    
    //当正方形移动到最右边时
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (blockX > (1.0 - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    
    //当正方形移动到最下面时
    //-1.0 - blockSize * 2 = Y（负轴边界） - 正方形边长 = 最下面点的位置
    if (blockY < -1.0f + blockSize * 2 ) {
        blockY = -1.0f + blockSize * 2;
    }
    
    //当正方形移动到最上面时
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }

    printf("blockX = %f\n",blockX);
    printf("blockY = %f\n",blockY);
    
    // Recalculate vertex positions
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize*2;
    printf("(%f,%f)\n",vVerts[0],vVerts[1]);
    
    vVerts[3] = blockX + blockSize*2;
    vVerts[4] = blockY - blockSize*2;
    printf("(%f,%f)\n",vVerts[3],vVerts[4]);
    
    vVerts[6] = blockX + blockSize*2;
    vVerts[7] = blockY;
    printf("(%f,%f)\n",vVerts[6],vVerts[7]);
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    printf("(%f,%f)\n",vVerts[9],vVerts[10]);
    
    triangleBatch.CopyVertexData3f(vVerts);
    */
    glutPostRedisplay();
}

int main(int argc,char* argv[]) {
    
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    glutInitWindowSize(500,500);
    glutCreateWindow("正方形");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    //注册监控键盘函数
    glutSpecialFunc(SpecialKeys);
    
    //驱动程序的初始化中没有出现任何问题。
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    //调用SetupRC
    SetupRC();
    glutMainLoop();
    
    return 0;
    
}


