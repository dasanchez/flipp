#include "glplotterwidget.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

GLPlotterWidget::GLPlotterWidget(QWidget *parent)
    : QOpenGLWidget(parent),
     shaderProgram(0)
{
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
//    dataLimit = 100;
//    yValues.reserve(dataLimit);

    // For anti-aliasing:
    QSurfaceFormat sFormat;
    sFormat.setSamples(8);
    this->setFormat(sFormat);

    // Control variables:
    vertCount = ACTIVE_VERTICES;
    prepareXPos(vertCount*2);
    vertPointer=0;
    color[0]=1;
    color[1]=0;
    color[2]=0;
    plotTimer = new QTimer;
    connect(plotTimer,SIGNAL(timeout()),this,SLOT(update()));
}

GLPlotterWidget::~GLPlotterWidget()
{
    cleanup();
}

QSize GLPlotterWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLPlotterWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLPlotterWidget::cleanup()
{
    makeCurrent();
    vao->destroy();
    vbo->destroy();
    delete vao;
    delete vbo;
    delete shaderProgram;
    shaderProgram = 0;
    doneCurrent();
}

void GLPlotterWidget::initializeGL()
{
    QString vShaderSource;
    QString fShaderSource;
    QFile vShader(vPath);
    QFile fShader(fPath);
    if(vShader.open(QIODevice::ReadOnly))
    {
        vShaderSource.append(vShader.readAll());
        vShader.close();
    }
    else
    {
        qDebug() << "Vertex file not found";
        exit(0);
    }
    if(fShader.open(QIODevice::ReadOnly))
    {
        fShaderSource.append(fShader.readAll());
        fShader.close();
    }
    else
    {
        qDebug() << "Fragment file not found";
        exit(0);
    }

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLPlotterWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    shaderProgram = new QOpenGLShaderProgram;
    if(!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vShaderSource))
    {
        qDebug() << "Vertex shader could not be loaded";
        exit(0);
    }
    else
    {
        qDebug() << "Vertex file was loaded successfully";
    }
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fShaderSource);
    shaderProgram->bind();
    shaderProgram->link();

    vColorLocation = shaderProgram->attributeLocation("vColor");
    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -2);
    m_world.setToIdentity();

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo->bind();
    vbo->allocate(verts,(vertCount*2)*2*sizeof(GLfloat));

    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    vbo->release();
    vao->release();
    shaderProgram->release();

    QMatrix4x4 tempmat;
    tempmat.setToIdentity();
    tempmat.ortho(1,2,0,2,0.01f,200.0f);
    tempmat.translate(1,1,1);
    m_proj = tempmat;

    plotTimer->start(ANIMSPEED);
}

void GLPlotterWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);

    shaderProgram->bind();
    vao->bind();
    vbo->bind();

    vbo->allocate(verts,(vertCount*2)*2*sizeof(GLfloat));

    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);
    shaderProgram->setAttributeValue(vColorLocation,color,1,3);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP,0,(vertCount*2));

    vbo->release();
    vao->release();
    shaderProgram->release();
}

//void GLPlotterWidget::resizeGL(int w, int h)
//{
//    m_proj.setToIdentity();
//    m_proj.ortho(1,2,0,2,0.01f,200.0f);
//    m_proj.translate(1,1,1);
//}

void GLPlotterWidget::prepareXPos(quint16 vertexCount)
{
    // We will fill up the x position of each vert depending on the amount of vertices
    // This function gets the total amount of vertices required (window*2)
    GLfloat stepSize = 2/double(vertexCount-2);

    for(quint16 i=0;i<vertexCount*2;i+=2)
    {
        verts[i] = stepSize*(i/2);
        verts[i+1] = 0.0f;
    }
}
void GLPlotterWidget::addDataPoint(double newPoint)
{
    // Increase x position pointer
    vertPointer++;
    if(vertPointer>vertCount)
    {
        vertPointer=0;
    }
    // Assign y value
    // 1. Assign it at the "front" of the queue: the current point on the right.
    // The queue can be organized as follows:
    // || [0][1][2]...[vertCount-1]||[vertCount][vertCount+1]...[vertCount*2-1]
    verts[2*(vertPointer+vertCount-1)+1] = newPoint;

    if(vertPointer>1)
    {
        verts[2*(vertPointer-2)+1]=newPoint;
    }

    moveCamera();
}

void GLPlotterWidget::moveCamera()
{
    // Move camera
    m_camera.setToIdentity();
    m_camera.translate(0,0,-3);
    m_camera.translate(-verts[2*vertPointer],0,0);
}

